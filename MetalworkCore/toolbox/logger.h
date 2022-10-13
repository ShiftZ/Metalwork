#pragma once

#include <string>
#include <format>
#include <fstream>
#include <source_location>
#include <shared_mutex>
#include <unordered_map>
#include <functional>

inline std::shared_mutex log_mutex;
inline std::unordered_map<std::string_view, std::ofstream> log_files;
inline std::unordered_map<int, std::function<void(std::string)>> loggers;
inline std::function<void(std::string)> logger;

struct format_with_location
{
	std::string_view str;
	std::source_location loc;
	template< typename string_type > requires is_convertible_v<string_type, std::string_view>
	format_with_location( string_type str, std::source_location loc = std::source_location::current() ) : str(str), loc(loc) {}
};

inline void reg_logger( std::function<void(std::string)> func )
{
	logger = func;
}

template< typename log_type > requires std::is_convertible_v<log_type, int>
void reg_logger( const log_type& type, std::function<void(std::string)> func )
{
	loggers[int(type)] = std::move(func);
}

template< typename... types >
void log( format_with_location fmt, types... args )
{
	using namespace std;
	string msg = format("{}: ", fmt.loc.function_name()) + vformat(fmt.str, make_format_args(args...));
	logger(move(msg));
}

template< typename log_type, typename... types > requires std::is_convertible_v<log_type, int>
void log( const log_type& type, format_with_location fmt, const types&... args )
{
	using namespace std;
	auto& [ignore, logger] = *loggers.find(int(type));
	string msg = format("{}: ", fmt.loc.function_name()) + vformat(fmt.str, make_format_args(args...)) + '\n';
	logger(move(msg));
}

template< typename... types >
void log( std::string_view file_name, format_with_location fmt, const types&... args )
{
	using namespace std;

	string msg = format(fmt.str, args...) + '\n';

	shared_lock slock(log_mutex);
	auto file_it = log_files.find(file_name);
	if (file_it != log_files.end())
	{
		auto& [name, file] = *file_it;
		file.write(msg.data(), msg.size()).flush();
	}
	else
	{
		slock.unlock();

		ofstream file(string(file_name) + ".log", ios_base::out | ios_base::trunc);
		file.write(msg.data(), msg.size()).flush();

		unique_lock lock(log_mutex);
		log_files.try_emplace(file_name, move(file));
	}
}