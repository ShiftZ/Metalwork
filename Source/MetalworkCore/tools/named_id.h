#pragma once

#include <shared_mutex>
#include <unordered_map>
#include <ranges>

constexpr size_t encode_name( std::string_view view )
{
	const size_t base = ('z' - 'a' + 3 + 1);

	size_t hash = 1;
	auto it = view.begin();
	for (;it != view.end(); ++it)
	{
		// ' ', '-', '0..9', 'A..Z', '_', 'a..z'

		size_t code;
		if (*it >= 'a')
			code = *it - 'a';
		else if (*it >= 'A')
			code = (*it != '_') ? *it - 'A' : base - 1;
		else if (*it < '0')
			code = (*it == '-') ? base - 2 : base - 3;
		else
			break; // digits

		hash = code + base * hash;
	}

	for (; it != view.end(); ++it)
		hash = (*it - '0') + hash * 10;

	return hash;
}

class named_id
{
	struct hasher : std::hash<std::string_view> { using is_transparent = void; };

	static inline std::atomic<int> count = 0;
	static inline std::unordered_map<std::string, int, hasher, std::equal_to<>> names;
	static inline std::shared_mutex mtx;

private:
	int id = -1;

public:
	named_id() = default;

	named_id( std::string_view name )
	{
		if (name.empty()) return;

		std::shared_lock slock(mtx);
		auto found = names.find(name);
		if (found != names.end())
		{
			id = found->second;
		}
		else
		{
			slock.unlock();
			std::unique_lock lock(mtx);
			names.emplace(name, count);
			id = count++;
		}
	}

	std::string_view to_string() const
	{
		if (id == -1) return {};
		auto& [str, _] = *std::ranges::find_if(names, [&](auto& row){ return row.second == id; });
		return str;
	}

	bool operator==( const named_id& other ) const { return id == other.id; }
};