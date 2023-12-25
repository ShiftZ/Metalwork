#pragma once

class Name
{
	//using string = basic_string<TCHAR>;
	//using string_view = basic_string_view<TCHAR>;

	struct Hasher
	{
		using is_transparent = void;
		size_t operator()(const string& s) const { return hash<string>()(s); }
		size_t operator()(const string_view& sv) const { return hash<string_view>()(sv); }
	};

	METALWORKCORE_API inline static constinit sol_hash_set<string, Hasher, equal_to<>> names;

private:
	string* str = nullptr;

	friend struct std::hash<Name>;
	friend struct std::formatter<Name>;

public:
	Name() = default;
	Name(const Name&) = default;
	Name(const wchar_t* wstr) { str = &*names.insert(string(wstr, wstr + wcslen(wstr))).first; }

	template< typename Type > requires is_convertible_v<Type, string_view>
	Name(const Type& src) { str = &*names.insert(string_view(src)).first; }

	Name& operator=(const Name&) = default;
	Name& operator=(const wchar_t* wstr) { str = &*names.insert(string(wstr, wstr + wcslen(wstr))).first; return *this; }

	template< typename Type > requires is_convertible_v<Type, string_view>
	Name& operator=(const Type& src) { str = &*names.insert(string_view(src)).first; return *this; }

	bool operator==(const Name& other) const { return str == other.str; }

	template< typename StrType > requires requires (StrType str) { *str == str; }
	bool operator==(const StrType& rh) const { return str ? *str == rh : ""sv == rh; }

	const char* operator*() const { return str ? str->c_str() : ""; }

	operator const wchar_t*() const
	{
		static wstring wstr;
		return str ? wstr.assign(str->begin(), str->end()).c_str() : L"";
	}

	operator bool() const { return str != nullptr; }
};

template<>
struct std::hash<Name>
{
	size_t operator()(const Name& name) const noexcept
		{ return name.str ? hash<void*>()(name.str) : 0; }
};

template<>
struct std::formatter<Name> : formatter<string>
{
    auto format(const Name& name, auto& ctx) const
		{ return formatter<string>::format(name.str ? *name.str : string(), ctx); }
};

inline Name operator ""_n(const char* str, size_t len) { return basic_string_view(str, len); }