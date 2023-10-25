#pragma once

#include "CoreInterface.h"
#include "NetworkMessages.h"

template<typename> class Task;

class Network : public INetwork
{
	struct Peer
	{
		int player;
		tcp::socket socket;
		udp::endpoint udp_endpoint;
		unsigned last_seqid = 0;
		vector<unsigned> missed_ids;
		unsigned last_reported_seqid = 0;
		deque<int> pings;
		bool ready = false;
	};

public:
	struct Logger
	{
		function<void(string)> log, error;
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

	cc_queue<pair<int, MessageVar>> messages;
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
	Network(span<RemotePlayer> targets, int player, stop_token st);

	void Start();
	void BeginCountdown(seconds secs);
	void WaitMessage(stop_token st, optional<steady_timer::time_point> until = nullopt);

	bool HasMessages() { return !messages.empty(); }
	pair<int, MessageVar> NextMessage();

	void SendSeqUDP(MessageVar msg);
	void SendRawUDP(MessageVar msg);
	void SendTCP(MessageVar msg);

	size_t NumPeers() { return peers.size(); }

	~Network();

	static Task<INetwork> Connect(span<RemotePlayer> targets, int player);
};

template<typename Type>
class Task : public IFuture<Type>
{
	future<unique_ptr<Type>> fu;
	jthread th;

public:
	Task(future<unique_ptr<Type>>&& fu, jthread&& th) : fu(move(fu)), th(move(th)) {}
	bool IsRead() const override { return fu.wait_for(0s) == future_status::ready; }
	unique_ptr<Type> Get() override { return fu.get(); }
};