#pragma once

class Name
{
	//using string = basic_string<TCHAR>;
	//using string_view = basic_string_view<TCHAR>;

	struct Hasher
	{
		using is_transparent = void;
		size_t operator()(string s) const { return hash<string>()(s); }
		size_t operator()(string_view sv) const { return hash<string_view>()(sv); }
	};

	METALWORKCORE_API static inline sol_hash_set<string, Hasher, equal_to<>> names;

private:
	string* name = nullptr;
	template<typename> friend struct std::hash;

public:
	Name() = default;
	Name(const Name&) = default;
	Name(const wchar_t* wstr) { name = &*names.insert(string(wstr, wstr + wcslen(wstr))).first; }

	template< typename Type > requires is_convertible_v<Type, string_view>
	Name(const Type& str) { name = &*names.insert(string_view(str)).first; }

	Name& operator=(const Name&) = default;
	Name& operator=(const wchar_t* wstr) { name = &*names.insert(string(wstr, wstr + wcslen(wstr))).first; return *this; }

	template< typename Type > requires is_convertible_v<Type, string_view>
	Name& operator=(const Type& str) { name = &*names.insert(string_view(str)).first; return *this; }

	bool operator==(const Name& other) const { return name == other.name; }
	bool operator!=(const Name& other) const { return name != other.name; }

	template< typename StrType > requires requires (StrType str) { *name == str; }
	bool operator==(const StrType& str) const { return name ? *name == str : ""sv == str; }

	template< typename StrType > requires requires (StrType str) { *name != str; }
	bool operator!=(const StrType& str) const { return name ? *name != str : ""sv != str; }

	const char* operator*() const { return name ? name->c_str() : ""; }

	operator const wchar_t*() const
	{
		static wstring wstr;
		return name ? wstr.assign(name->begin(), name->end()).c_str() : L"";
	}
};

template<>
struct std::hash<Name>
{
	size_t operator()(const Name& n) const noexcept { return n.name ? std::hash<void*>()(n.name) : 0; }
};

inline Name operator ""_n(const char* str, size_t len) { return basic_string_view(str, len); }