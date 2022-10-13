#pragma once

#include <vector>
#include <thread>
#include <future>
#include <deque>
#include <variant>

#include "NetworkMessages.h"

# undef TEXT

#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ip/udp.hpp"
#include "asio/awaitable.hpp"
#include "asio/steady_timer.hpp"
#include "toolbox/queue.h"

using namespace std;
using namespace std::chrono;
using namespace asio;
using namespace ip;

template<typename> class xfuture;

class Network
{
	struct Peer
	{
		int player;
		tcp::socket socket;
		udp::endpoint udp_endpoint;
		unsigned last_seqid = 0;
		vector<int> missed_ids;
		unsigned last_reported_seqid = 0;
		deque<int> pings;
		bool ready = false;
	};

public:
	struct Logger
	{
		function<void(string)> log, error;
	};

	struct Target
	{
		int player;
		address ip;
		struct { int tcp, udp; } port;
	};

	struct Package
	{
		unsigned seq_id;
		shared_ptr<package_buffer> buffer;
	};

private:
	int player;
	vector<Peer> peers;
	io_context io;
	udp::socket udp;
	unsigned seq_id = 1;
	jthread thread;
	Logger logger;

	queue<pair<int, MessageVar>> messages;
	mutex message_mtx;
	condition_variable_any message_cv;

	vector<Package> packages;

private:
	awaitable<void> ListenUDP();
	awaitable<void> ListenTCP(Peer& peer);
	awaitable<void> Countdown(seconds secs);

	void ProcessMessage(MessageVar msg, Peer& peer);
	void PushMessage(int player, MessageVar msg);

	template<typename Type>
	awaitable<Type> ReadTCP(tcp::socket& socket);

public:
	Network(vector<Target> targets, int player, Logger logger, stop_token st);

	void Start();
	void BeginCountdown(seconds secs);
	void WaitMessage(stop_token st, optional<steady_timer::time_point> until = nullopt);

	bool HasMessages() { return !messages.empty(); }
	pair<int, MessageVar> NextMessage();

	void SendSeqUDP(MessageVar msg);
	void SendRawUDP(MessageVar msg);
	void SendTCP(MessageVar msg);

	int NumPeers() { return peers.size(); }

	~Network();

	static xfuture<unique_ptr<Network>> Connect(vector<Target> targets, int player, Logger logger);
};

template<typename Type>
class xfuture : public future<Type>
{
	jthread thread;

public:
	xfuture() = default;
	xfuture(future<Type>&& f, jthread&& thread) : future<Type>(move(f)), thread(move(thread)) {}
	bool is_ready() const { return future<Type>::wait_for(std::chrono::seconds(0)) == future_status::ready; }
};
