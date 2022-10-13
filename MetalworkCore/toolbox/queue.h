#pragma once

#include <atomic>
#include <cassert>

template< typename type >
class queue // concurrent mpsc
{
	struct node 
	{ 
		std::atomic<node*> next; 
		type data;
	};

	std::atomic<node*> head; 
	node* tail;
	int size = 0;

public:
	queue()
	{
		node* stub = (node*)operator new(sizeof(node));
		stub->next.store(nullptr, std::memory_order_relaxed);
		head.store(stub, std::memory_order_relaxed);
		tail = stub;
	}

	template< typename... types >
	void push( types&&... args )
	{ 
		node* n = (node*)operator new(sizeof(node));
		n->next.store(nullptr, std::memory_order_relaxed);
		new (&n->data) type(std::forward<types>(args)...);

		node* prev = head.exchange(n, std::memory_order_relaxed);
		prev->next.store(n, std::memory_order_release);
		++size;
	}

	bool empty() const
	{
		return tail->next.load(std::memory_order_relaxed) == nullptr;
	}

	type& get()
	{
		node* next = tail->next.load(std::memory_order_acquire);
		assert(next != nullptr);
		return next->data;
	}

	void pop()
	{ 
		node* next = tail->next.load(std::memory_order_acquire);
		assert(next != nullptr);
		
		operator delete(tail);
		tail = next;
		next->data.~type();
		--size;
	}

	template< typename out_type > requires requires( out_type& v ){ v = std::move(std::declval<type>()); }
	bool pop( out_type& val )
	{ 
		node* next = tail->next.load(std::memory_order_acquire);
		if (next == nullptr) return false;
		
		operator delete(tail);
		tail = next;
		val = std::move(next->data);
		next->data.~type();
		--size;

		return true;
	}

	~queue()
	{
		node* n = tail;
		tail = tail->next.load(std::memory_order_relaxed);
		operator delete(n);

		while (tail != nullptr)
		{
			tail->data.~type();
			n = tail;
			tail = tail->next.load(std::memory_order_relaxed);
			operator delete(n);
		}
	}
};

struct intrusive_queue // lockfree intrusive mpsc
{
	struct node
	{
		std::atomic<node*> next;
		virtual ~node() {}
	};

private:
	std::atomic<node*> head;
    node* tail;
    node stub;
	
public:
	intrusive_queue()
	{
		head.store(&stub, std::memory_order_relaxed);
		tail = &stub;
		stub.next.store(nullptr, std::memory_order_relaxed);
	}

	void push( node* n )
	{
		n->next.store(nullptr, std::memory_order_relaxed);
		node* prev = head.exchange(n, std::memory_order_relaxed);
		prev->next.store(n, std::memory_order_release);
	}

	node* pop()
	{
		node* next = tail->next.load(std::memory_order_acquire);

		if (tail == &stub)
		{
			if (next == nullptr) return nullptr;
			tail = next;
			next = next->next.load(std::memory_order_relaxed);
		}

		if (next)
		{
			node* n = tail;
			tail = next;
			return n;
		}

		if (tail != head.load(std::memory_order_relaxed))
			return nullptr;
		
		push(&stub);
		
		next = tail->next.load(std::memory_order_relaxed);
		
		if (next)
		{
			node* n = tail;
			tail = next;
			return n;
		}

		return nullptr;
	}

	bool empty() const
	{
		return tail == &stub && tail->next.load(std::memory_order_relaxed) == nullptr;
	}

	~intrusive_queue()
	{
		while (tail != nullptr)
		{
			node* n = tail;
			tail = tail->next.load(std::memory_order_relaxed);
			if (n != &stub) delete n;
		}
	}
};

template< typename type, int buffer_size >
class bounded_queue // concurrent mpmc queue
{
	static_assert(buffer_size >= 2 && (buffer_size & (buffer_size - 1)), "buffer_size must be power of 2");

	static const int buffer_mask = buffer_size - 1;
	
	struct cell
	{
		std::atomic<int> sequence;
		type data;
	};

	typedef char cacheline_pad[64];

public:
	struct queue_is_full {};

private:
	cacheline_pad pad0;
	cell* buffer;

	cacheline_pad pad1;
	std::atomic<int> enqueue_pos;

	cacheline_pad pad2;
	std::atomic<int> dequeue_pos;

	cacheline_pad pad3;

private:
	bounded_queue(bounded_queue const&);
	void operator=(bounded_queue const&);

public:
	bounded_queue()
	{
		assert((buffer_size >= 2) && ((buffer_size & (buffer_size - 1)) == 0));

		buffer = (cell*)operator new(sizeof(cell) * buffer_size);

		for (int i = 0; i < buffer_size; i++)
			buffer[i].sequence.store(i, std::memory_order_relaxed);
		
		enqueue_pos.store(0, std::memory_order_relaxed);
		dequeue_pos.store(0, std::memory_order_relaxed);
	}

	template< typename... types >
	type& push( types&&... args )
	{
		cell* cl;
		int pos = enqueue_pos.load(std::memory_order_relaxed);
		for (;;)
		{
			cl = &buffer[pos & buffer_mask];
			int seq = cl->sequence.load(std::memory_order_acquire);
			int diff = seq - pos;
			if (diff == 0)
			{
				if (enqueue_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) break;
			}
			else if (diff < 0)
				throw queue_is_full();
			else
				pos = enqueue_pos.load(std::memory_order_relaxed);
		}
		
		new (&cl->data) type(std::forward<types>(args)...);
		cl->sequence.store(pos + 1, std::memory_order_release);
		return cl->data;
	}

	bool full() const
	{
		int pos = enqueue_pos.load(std::memory_order_relaxed);
		cell* cl = &buffer[pos & buffer_mask];
		int seq = cl->sequence.load(std::memory_order_acquire);
		return (seq - pos) < 0;
	}

	bool pop( type& data )
	{
		cell* cl;
		int pos = dequeue_pos.load(std::memory_order_relaxed);
		for (;;)
		{
			cl = &buffer[pos & buffer_mask];
			int seq = cl->sequence.load(std::memory_order_acquire);
			int diff = seq - (pos + 1);
			if (diff == 0)
			{
				if (dequeue_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) break;
			}
			else if (diff < 0)
				return false;
			else
				pos = dequeue_pos.load(std::memory_order_relaxed);
		}
		data = std::move(cl->data);
		cl->data.~type();
		cl->sequence.store(pos + buffer_mask + 1, std::memory_order_release);
		return true;
	}

	bool empty() const
	{
		int pos = dequeue_pos.load(std::memory_order_relaxed);
		cell* cl = &buffer[pos & buffer_mask];
		int seq = cl->sequence.load(std::memory_order_acquire);
		return (seq - pos) <= 0;
	}

	~bounded_queue()
	{
		int dpos = dequeue_pos.load(std::memory_order_relaxed);
		int epos = enqueue_pos.load(std::memory_order_relaxed);
		while (dpos != epos)
		{
			cell* cl = &buffer[dpos & buffer_mask];
			cl->data.~type();
			dpos++;
		}
		
		operator delete(buffer);
	}
};
