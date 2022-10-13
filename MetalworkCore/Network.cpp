// NAT traversal papers STUN\ICE protocol
// https://bford.info/pub/net/p2pnat/
// https://github.com/paullouisageneau/libjuice
// https://libnice.freedesktop.org/
// https://github.com/paullouisageneau/violet

#include <chrono>
#include <future>
#include <numeric>
#include <ranges>

#include "asio/buffer.hpp"
#include "asio/use_awaitable.hpp"
#include "asio/experimental/awaitable_operators.hpp"
#include "asio/detached.hpp"
#include "asio/cancellation_signal.hpp"
#include "asio/bind_cancellation_slot.hpp"
#include "asio/read.hpp"

#include "Network.h"

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace std::ranges::views;
using namespace asio::ip;
using namespace asio::experimental;
using namespace asio::experimental::awaitable_operators;

static auto rethrow = [](exception_ptr e){ if (e) rethrow_exception(e); };

Network::Network(vector<Target> targets, int player, Logger logger, stop_token st) : player(player), udp(io)
{
	if (!logger.log) logger.log = [](string){};
	if (!logger.error) logger.error = [](string){};
	this->logger = logger;

	udp.open(udp::v4());
	udp.bind(udp::endpoint(udp::v4(), 22150 + player % 2));

	if (player % 2)
	{
		auto listen = [&](Target& target) -> awaitable<void>
		{
			tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 22150 + player % 2));
			steady_timer timeout(io, steady_clock::now() + 5s);

			tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
			peers.emplace_back(target.player, move(socket), udp::endpoint(target.ip, target.port.udp));

	/*		auto result = co_await (acceptor.async_accept(use_awaitable) || timeout.async_wait(use_awaitable));
			if (!holds_alternative<tcp::socket>(result))
				throw runtime_error("tcp socket listen failed");

			peers.emplace_back(target.player, move(get<tcp::socket>(result)), udp::endpoint(target.ip, target.port.udp));*/
		};

		for (Target& target : targets)
			co_spawn(io, bind(listen, target), rethrow);
	}
	else
	{
		auto connect = [&](Target& target) -> awaitable<void>
		{
			tcp::socket socket(io);
			tcp::endpoint tcp_endpoint(target.ip, target.port.tcp);
			auto deadline = steady_clock::now() + 5s;

			for (;;)
			{
				try
				{
					co_await socket.async_connect(tcp_endpoint, use_awaitable);
					break;
				}
				catch (asio::system_error& e)
				{
					if (e.code() != error::connection_refused) throw;
					if (steady_clock::now() > deadline)
						throw runtime_error(format("Remote tcp socket connection timeout ({})", target.ip.to_string()));
					logger.log("Remote connection refused. Trying again.");
				}
			}

			peers.emplace_back(target.player, move(socket), udp::endpoint(target.ip, target.port.udp));
		};

		for (Target& target : targets)
			co_spawn(io, bind(connect, target), rethrow);
	}

	if (!st.stop_requested())
	{
		stop_callback stop(st, [&]{ io.stop(); });
		io.run();
		Start();
	}
}

awaitable<void> Network::ListenUDP()
{
	for (;;)
	{
		co_await udp.async_wait(udp::socket::wait_read, use_awaitable);

		package_buffer buf(udp.available());
		udp::endpoint from;
		udp.receive_from(buffer(buf), from);

		try
		{
			auto peer = ranges::find_if(peers, [&](Peer& peer){ return from == peer.udp_endpoint; });
			if (peer == peers.end())
				throw runtime_error(format("Package from unknown source {}", from.address().to_string()));

			ChunkType chunk_type = buf.pop_front<ChunkType>();

			while (chunk_type != ChunkType::End)
			{
				auto ReadBody = [&]<typename... Types>(int type, variant<Types...>& msg)
				{
					if (type >= sizeof...(Types))
						throw runtime_error(format("Invalid message type: {}", type));

					auto reader = []<typename Type>(package_buffer& buf){ return MessageVar(buf.pop_front<Type>()); };
					static array readers = {&decltype(reader)::template operator()<Types>...};
					msg = (reader.*readers[type])(buf);
				};

				if (chunk_type == ChunkType::RawMessage)
				{
					RawHeader header = buf.pop_front<RawHeader>();
					MessageVar msg;
					ReadBody(header.type, msg);
					ProcessMessage(move(msg), *peer);
				}
				else if (chunk_type == ChunkType::SeqMessage)
				{
					SeqHeader header = buf.pop_front<SeqHeader>();
					MessageVar msg;
					ReadBody(header.type, msg);

					if (header.seq_id > peer->last_seqid) // new message
					{
						for (unsigned i = peer->last_seqid + 1; i < header.seq_id; ++i)
							peer->missed_ids.push_back(i);
						peer->last_seqid = header.seq_id;
						PushMessage(peer->player, move(msg));
					}
					else // missed one?
					{ 
						auto missed = ranges::find(peer->missed_ids, header.seq_id);
						if (missed != peer->missed_ids.end())
						{
							peer->missed_ids.erase(missed);
							PushMessage(peer->player, move(msg));
						}
					}
				}
				else if (chunk_type == ChunkType::Report)
				{
					ReportMsg report = buf.pop_front<ReportMsg>();
					peer->last_reported_seqid = report.last_seqid;

					// release confirmed messages
					unsigned confirmed_seqid = accumulate(peers.begin(), peers.end(), report.last_seqid, 
						[](unsigned id, Peer& p){ return std::min(id, p.last_reported_seqid); });

					auto first_unconfirmed = ranges::find_if(packages, [&](Package& p){ return p.seq_id > confirmed_seqid; });
					if (first_unconfirmed != packages.end())
						packages.erase(packages.begin(), first_unconfirmed);

					// get missed message sequence ids
					vector<unsigned> missed(report.missing_count);
					if (udp.available() < sizeof(unsigned) * report.missing_count)
						throw runtime_error(format("Unexpected end of message reading vector<unsigned>"));
					udp.receive(buffer(missed));

					// resend reported missed messages
					shared_ptr missed_pak = make_shared<package_buffer>();

					for (unsigned id : missed)
					{
						auto pak = ranges::find_if(packages, [&](Package& pak){ return pak.seq_id == id; });
						if (pak != packages.end())
							missed_pak->insert(missed_pak->end(), pak->buffer->begin(), pak->buffer->end());
					}

					if (!missed_pak->empty())
					{
						missed_pak->emplace_back(ChunkType::End);
						auto handler = [missed_pak](const asio::error_code& er, size_t){ if (er) asio::detail::throw_error(er); };
						udp.async_send_to(buffer(*missed_pak), peer->udp_endpoint, handler);
					}
				}

				chunk_type = buf.pop_front<ChunkType>();
			}
		}
		catch (exception& e)
		{
			logger.error("Error reading UDP package: "s + e.what());
		}
	}
}

awaitable<void> Network::ListenTCP(Peer& peer)
{
	for (;;)
	{
		ChunkType chunk_type;
		co_await async_read(peer.socket, buffer(&chunk_type, sizeof(chunk_type)), use_awaitable);

		auto ReadBody = [&]<typename... Types>(int type, variant<Types...>& msg) -> awaitable<void>
		{
			if (type >= sizeof...(Types))
				throw runtime_error(format("Invalid message type: {}", type));

			auto reader = []<typename Type>(Network* net, tcp::socket& s, variant<Types...>& msg) -> awaitable<void>
				{ msg = co_await net->ReadTCP<Type>(s); };
			static array readers = {&decltype(reader)::template operator()<Types>...};
			co_await (reader.*readers[type])(this, peer.socket, msg);
		};

		if (chunk_type == ChunkType::RawMessage)
		{
			RawHeader header = co_await ReadTCP<RawHeader>(peer.socket);
			MessageVar msg;
			co_await ReadBody(header.type, msg);
			ProcessMessage(msg, peer);
		}
		else if (chunk_type != ChunkType::End)
			throw runtime_error(format("Unexpected chunk type {}", int(chunk_type)));
	}
}

awaitable<void> Network::Countdown(seconds secs)
{
	static int countdown_id = 0;
	
	steady_timer::time_point now = steady_clock::now();
	steady_timer::time_point end = now + secs;
	int num = 0;

	do
	{
		int msecs_left = duration_cast<milliseconds>(end - steady_clock::now()).count();
		SendRawUDP(CountdownMsg(countdown_id, ++num, msecs_left));
		now += 100ms;
		co_await steady_timer(io, now).async_wait(use_awaitable);
	}
	while (now < end);

	++countdown_id;
}

void Network::Start()
{
	thread = jthread([&]
	{
		co_spawn(io, bind(&Network::ListenUDP, this), rethrow);

		for (Peer& peer : peers)
			co_spawn(io, bind(&Network::ListenTCP, this, ref(peer)), rethrow);

		auto ping = [&]() -> awaitable<void>
		{
			for (;;)
			{
				auto now_ms = time_point_cast<milliseconds>(steady_clock::now());
				auto time = now_ms.time_since_epoch().count();

				SendRawUDP(PingMsg(time));

				steady_timer timer(io, steady_clock::now() + 333ms);
				co_await timer.async_wait(use_awaitable);
			}
		};

		//cancellation_signal cancel_sig;
		//co_spawn(io, ping, bind_cancellation_slot(cancel_sig, rethrow));

		co_spawn(io, ping, rethrow);

		try
		{
			io.restart();
			io.run();
		}
		catch (exception& e)
		{
			string err = e.what();
			DebugBreak();
		}
	});
}

void Network::BeginCountdown(seconds secs)
{
	io.post([=]{ co_spawn(io, bind(&Network::Countdown, this, secs), rethrow); });
}

void Network::WaitMessage(stop_token st, optional<steady_timer::time_point> until)
{
	unique_lock lock(message_mtx);
	if (until)
		message_cv.wait_until(lock, st, *until, [&]{ return !messages.empty(); });
	else
		message_cv.wait(lock, st, [&]{ return !messages.empty(); });
}

pair<int, MessageVar> Network::NextMessage()
{
	pair<int, MessageVar> msg;
	messages.pop(msg);
	return msg;
}

void Network::SendSeqUDP(MessageVar msg_var)
{
	auto send = [&](auto& msg)
	{
		shared_ptr package = make_shared<package_buffer>();
		package->emplace_back(ChunkType::SeqMessage, SeqHeader(msg_var.index(), seq_id), msg);

		io.post([=, id = seq_id++]() mutable
		{
			packages.emplace_back(id, package);

			for (Peer& peer : peers)
			{
				// attach nACK report
				shared_ptr buf = make_shared<package_buffer>(*package);
				unsigned last_clean_seqid = peer.missed_ids.empty() ? peer.last_seqid : (peer.missed_ids.front() - 1);
				buf->emplace_back(ChunkType::Report, ReportMsg(last_clean_seqid, peer.missed_ids.size()));
				for (unsigned missed_id : peer.missed_ids)
					buf->emplace_back(missed_id);
				buf->emplace_back(ChunkType::End);

				auto handler = [buf](const asio::error_code& er, size_t){ if (er) asio::detail::throw_error(er); };
				udp.async_send_to(buffer(*buf), peer.udp_endpoint, handler);
			}
		});
	};

	visit(send, msg_var);
}

void Network::SendRawUDP(MessageVar msg_var)
{
	auto send = [&](auto& msg)
	{
		shared_ptr buf = make_shared<package_buffer>();
		buf->emplace_back(ChunkType::RawMessage, RawHeader(msg_var.index()), msg, ChunkType::End);

		io.post([=]
		{
			auto handler = [buf](const asio::error_code& er, size_t){ if (er) asio::detail::throw_error(er); };
			for (Peer& peer : peers)
				udp.async_send_to(buffer(*buf), peer.udp_endpoint, handler);
		});
	};

	visit(send, msg_var);
}

void Network::SendTCP(MessageVar msg_var)
{
	Visitor visitor =
	{
		[&]<typename Type>(Type& msg)
		{
			shared_ptr buf = make_shared<package_buffer>();
			buf->emplace_back(ChunkType::RawMessage, RawHeader(msg_var.index()), msg, ChunkType::End);

			io.post([=]
			{
				auto handler = [buf](const asio::error_code& er, size_t){ if (er) asio::detail::throw_error(er); };
				for (Peer& peer : peers)
					peer.socket.async_send(buffer(*buf), handler);
			});
		}
	};

	visit(visitor, msg_var);
}

void Network::ProcessMessage(MessageVar msg, Peer& peer)
{
	Visitor visitor
	{
		[&](PingMsg& ping_msg)
		{
			SendRawUDP(PingEchoMsg(ping_msg.time));
			return true;
		},
		[&](PingEchoMsg& ping_msg)
		{
			auto now_ms = time_point_cast<milliseconds>(steady_clock::now());
			int ping = now_ms.time_since_epoch().count() - ping_msg.time;
			logger.log(format("ping: {}ms", ping));

			if (!peer.pings.empty())
			{
				int avg_ping = accumulate(peer.pings.begin(), peer.pings.end(), 0) / peer.pings.size();
				if (ping > avg_ping * 5/3) return true;
			}

			peer.pings.push_back(ping);
			if (peer.pings.size() > 25) peer.pings.pop_front();
			return true;
		},
		[&](ReadyMsg&)
		{
			peer.ready = true;
			auto not_ready = ranges::find_if(peers, [](Peer& peer){ return !peer.ready; });
			if (not_ready == peers.end()) PushMessage(-1, AllReady());
			return true;
		},
		[&](CountdownMsg& countdown_msg)
		{
			int avg_ping = accumulate(peer.pings.begin(), peer.pings.end(), 0) / peer.pings.size();
			countdown_msg.msecs_left -= avg_ping / 2;
			PushMessage(peer.player, countdown_msg);
			return true;
		},
		[](auto&) { return false; }
	};

	if (!visit(visitor, msg)) 
		PushMessage(peer.player, move(msg));
}

void Network::PushMessage(int player, MessageVar msg)
{
	unique_lock lock(message_mtx);
	messages.push(player, move(msg));
	message_cv.notify_one();
}

template<typename Type>
awaitable<Type> Network::ReadTCP(tcp::socket& socket)
{
	Type obj;
	co_await async_read(socket, buffer(&obj, sizeof(obj)), use_awaitable);
	co_return obj;
}

xfuture<unique_ptr<Network>> Network::Connect(vector<Target> targets, int player, Logger logger)
{
	packaged_task task([=](stop_token st)
		{ return make_unique<Network>(targets, player, logger, st); });

	future<unique_ptr<Network>> future = task.get_future();
	jthread thread([task = move(task)](stop_token token) mutable { task(token); });

	return {move(future), move(thread)};
}

Network::~Network() { io.stop(); }
