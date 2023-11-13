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

#if defined(LOGGER_EXPORT)
#	define LOGGER_API inline DLLEXPORT
#elif defined(LOGGER_IMPORT)
#	define LOGGER_API DLLIMPORT
#else
#	define LOGGER_API inline
#endif

LOGGER_API std::unordered_map<size_t, std::any> loggers;
LOGGER_API std::function<void(std::string)> logger;
inline std::shared_mutex log_mutex;
inline std::unordered_map<std::string_view, std::ofstream> log_files;

template< typename... types >
struct format_with_location
{
	std::format_string<types...> str;
	std::source_location loc;
	format_with_location( std::format_string<types...> str, std::source_location loc = std::source_location::current() )
		: str(std::move(str)), loc(std::move(loc)) {}
};

struct log_category
{
	operator size_t(this auto&& self) { return std::type_index(typeid(self)).hash_code(); }
};

inline void add_logger( std::function<void(std::string)> func )
{
	logger = func;
}

template< typename log_t, typename func_t > requires
	requires (log_t t, func_t&& f) { size_t(t); std::function<void(std::string)>(std::forward<func_t>(f)); }
void add_logger( const log_t& type, std::function<void(std::string)>&& func )
{
	loggers[size_t(type)].emplace<std::function<void(std::string)>>(std::forward<func_t>(func));
}

template< typename log_t, typename func_t > requires
	requires (log_t t, func_t&& f) { size_t(t); std::function<void(std::string, const log_t&)>(std::forward<func_t>(f)); }
void add_logger( const log_t& type, func_t&& func )
{
	loggers[size_t(type)].emplace<std::function<void(std::string, const log_t&)>>(std::forward<func_t>(func));
}

template< typename... types >
void log( format_with_location<types...> fmt, types... args )
{
	using namespace std;
	string msg = format(fmt.str, args...);

	size_t pos = msg.find("$fn");
	if (pos != string::npos)
		fmt.str.replace(pos, 4, fmt.loc.function_name());

	logger(move(msg));
}

template< typename log_t, typename... types > requires std::is_convertible_v<log_t, size_t>
void log( const log_t& id, format_with_location<types> fmt, const types&... args )
{
	using namespace std;
	using ext_signature = void(string, const log_t&);

	auto logger_it = loggers.find(id);
	if (logger_it == loggers.end())
		throw logic_error(format("No logger found for id#{}", size_t(id)));

	auto& [_, logr] = *loggers.find(size_t(id));

	string msg = format(fmt.str, args...);

	size_t pos = msg.find("$fn");
	if (pos != string::npos)
		fmt.str.replace(pos, 4, fmt.loc.function_name());
	
	if (logr.type() == typeid(function<void(string)>))
		any_cast<function<void(string)>>(logr)(move(msg));
	else if (logr.type() == typeid(function<ext_signature>))
		any_cast<function<void(string, const log_t&)>>(logr)(move(msg), id);
	else
		throw logic_error(format("Invalid log id type for id#{} ({})", size_t(id), typeid(log_t).name()));
}

template< typename... types >
void flog( std::string_view file_name, format_with_location<types> fmt, const types&... args )
{
	using namespace std;

	string msg = format(fmt.str, args...) + '\n';

	size_t pos = msg.find("$fn");
	if (pos != string::npos)
		fmt.str.replace(pos, 4, fmt.loc.function_name());

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

inline void flog_path( std::string_view name, std::filesystem::path path, std::optional<std::string_view> header = {})
{
	using namespace std;

	shared_lock slock(log_mutex);
	ofstream file(path, ios_base::out | ios_base::trunc);
	if (header) file.write(header->data(), header->size()).put('\n').flush();
	log_files[name] = move(file);
}