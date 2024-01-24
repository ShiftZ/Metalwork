#pragma once

#include <coroutine>
#include <exception>

template< typename type >
class generator
{
public:
	struct promise_type;
	using handle_type = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		type value;
		std::exception_ptr exception;

		promise_type() = default;
		~promise_type() = default;

		auto get_return_object() { return generator{handle_type::from_promise(*this)}; }
		auto initial_suspend() { return std::suspend_always{}; }
		auto final_suspend() noexcept { return std::suspend_always{}; }
		void unhandled_exception() { exception = std::current_exception(); }
		auto yield_value(type some_value)
		{
			value = some_value;
			return std::suspend_always{};
		}

		void return_void() {}

		static generator get_return_object_on_allocation_failure() { return generator{nullptr}; }
	};

	generator() : coro(nullptr) {}
	generator(const generator&) = delete;
	generator(generator&& other) noexcept : coro(other.coro) { other.coro = nullptr; }

	~generator() { if (coro) coro.destroy(); }

	generator& operator=(const generator&) = delete;

	generator& operator=(generator&& other) noexcept
	{
		if (this != &other)
		{
			if (coro) coro.destroy();
			coro = other.coro;
			other.coro = nullptr;
		}
		return *this;
	}

	type next()
	{
		if (!coro) throw std::out_of_range("Generator is exhausted");
		coro.resume();
		if (coro.done())
		{
			std::exception_ptr exception = coro.promise().exception;
			if (exception) std::rethrow_exception(exception);
		}
		return coro.promise().value;
	}

	struct iterator
	{
		handle_type coro;

		iterator(handle_type coro) : coro(coro) {}

		iterator& operator++()
		{
			coro.resume();
			if (coro.done()) coro = nullptr;
			return *this;
		}

		bool operator!=(const iterator& other) const { return coro != other.coro; }
		const type& operator*() const { return coro.promise().value; }
		const type* operator->() const { return &operator*(); }
	};

	iterator begin()
	{
		if (coro)
		{
			coro.resume();
			if (coro.done()) return end();
		}
		return iterator{coro};
	}

	iterator end() { return iterator{nullptr}; }

private:
	generator( handle_type h ) : coro(h) {}
	handle_type coro;
};