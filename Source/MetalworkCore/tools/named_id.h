#pragma once

#include <shared_mutex>
#include <unordered_map>
#include <ranges>

class named_id
{
	struct hasher : std::hash<std::string_view> { using is_transparent = void; };

	static inline int count = 0;
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