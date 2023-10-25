#pragma once

#include <string>
#include <format>
#include <fstream>
#include <source_location>
#include <shared_mutex>
#include <unordered_map>
#include <functional>
#include <any>
#include <stdexcept>

inline std::shared_mutex log_mutex;
inline std::unordered_map<std::string_view, std::ofstream> log_files;
inline std::unordered_map<int, std::any> loggers;
inline std::function<void(std::string)> logger;

struct format_with_location
{
	std::string_view str;
	std::source_location loc;
	template< typename string_type > requires std::is_convertible_v<string_type, std::string_view>
	format_with_location( string_type str, std::source_location loc = std::source_location::current() ) : str(str), loc(loc) {}
};

inline void add_logger( std::function<void(std::string)> func )
{
	logger = func;
}

template< typename log_t, typename func_t > requires
	requires (log_t t, func_t&& f) { int(t); std::function<void(std::string)>(std::forward<func_t>(f)); }
void add_logger( const log_t& type, std::function<void(std::string)>&& func )
{
	loggers[int(type)].emplace<std::function<void(std::string)>>(std::forward<func_t>(func));
}

template< typename log_t, typename func_t > requires
	requires (log_t t, func_t&& f) { int(t); std::function<void(std::string, const log_t&)>(std::forward<func_t>(f)); }
void add_logger( const log_t& type, func_t&& func )
{
	loggers[int(type)].emplace<std::function<void(std::string, const log_t&)>>(std::forward<func_t>(func));
}

template< typename... types >
void log( format_with_location fmt, types... args )
{
	using namespace std;
	string msg = format("{}: ", fmt.loc.function_name()) + vformat(fmt.str, make_format_args(args...));
	logger(move(msg));
}

template< typename log_t, typename... types > requires std::is_convertible_v<log_t, int>
void log( const log_t& id, format_with_location fmt, const types&... args )
{
	using namespace std;
	using ext_signature = void(string, const log_t&);

	auto logger_it = loggers.find(int(id));
	if (logger_it == loggers.end())
		throw logic_error(format("No logger found for id#{}", int(id)));

	auto& [_, logr] = *loggers.find(int(id));

	string msg = format("{}: ", fmt.loc.function_name()) + vformat(fmt.str, make_format_args(args...)) + '\n';
	if (logr.type() == typeid(function<void(string)>))
		any_cast<function<void(string)>>(logr)(move(msg));
	else if (logr.type() == typeid(function<ext_signature>))
		any_cast<function<void(string, const log_t&)>>(logr)(move(msg), id);
	else
		throw logic_error(format("Invalid log id type for id#{}", int(id)));
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