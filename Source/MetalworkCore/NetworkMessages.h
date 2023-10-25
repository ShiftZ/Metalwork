#pragma once

enum class ChunkType : int32_t { SeqMessage = 1097116219, RawMessage, Report, End };

template<typename... Types>
struct Visitor : Types... { using Types::operator()...; };

#pragma pack(push, 1)

struct PingMsg { int64_t time; };
struct PingEchoMsg { int64_t time; };
struct ReadyMsg {};
struct AllReady {};
struct CountdownMsg { int id, num, msecs_left; };

struct InputMsg
{
	int seq_id, step;
	vec2i pointer;
};

struct RawHeader
{
	uint8_t type;
};

struct SeqHeader
{
	uint8_t type;
	unsigned seq_id;
};

struct ReportMsg
{
	unsigned last_seqid;
	int16_t missing_count;
};

#pragma pack(pop)

using MessageVar = variant<PingMsg, PingEchoMsg, ReadyMsg, AllReady, CountdownMsg, InputMsg>;

class package_buffer : public vector<byte>
{
	int top = 0;

public:
	using vector::vector;

	template<typename type, typename... arguments> requires is_trivially_copy_assignable_v<type>
	type& emplace_back(arguments&&... args)
	{
		resize(size() + sizeof(type));
		return *new (data() + size() - sizeof(type)) decay_t<type>{forward<arguments>(args)...};
	}

	template<typename type>
	type& emplace_back(type&& val)
	{
		resize(size() + sizeof(type));
		return  *new (data() + size() - sizeof(type)) decay_t<type>(forward<type>(val));
	}

	template<typename... types>
	void emplace_back(types&&... values)
	{
		(emplace_back(forward<types>(values)), ...);
	}

	template<typename type>
	type pop_front()
	{
		if (size() - top < sizeof(type))
			throw runtime_error("Invalid package size");
		type val;
		memcpy(&val, data() + top, sizeof(val));
		top += sizeof(val);
		return val;
	}
};

template<typename Type, typename... Types>
constexpr uint8_t MessageIndex(variant<Types...>*)
{
	bool result[sizeof...(Types)] = {is_same_v<Type, Types>...};
	for (uint8_t i = 0; i < sizeof...(Types); i++)
		if (result[i]) return i;
	return -1;
}

template<typename Type>
constexpr uint8_t MessageIndex() { return MessageIndex<Type>((MessageVar*)nullptr); }