#pragma once

class Name
{
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
	Name(string_view sv) { AcquireName(sv); }
	Name(const string& s) { AcquireName(s); }

	Name& operator=(const Name&) = default;
	Name& operator=(string_view sv) { return AcquireName(sv); }
	Name& operator=(const string& s) { return AcquireName(s); }
	Name& operator=(const char* cstr) { return AcquireName(string_view(cstr)); }

	bool operator==(const Name& other) const { return name == other.name; }
	bool operator!=(const Name& other) const { return name != other.name; }

	template< typename StrType > requires requires (StrType str) { *name == str; }
	bool operator==(const StrType& str) const { return name ? *name == str : ""sv == str; }

	template< typename StrType > requires requires (StrType str) { *name != str; }
	bool operator!=(const StrType& str) const { return name ? *name != str : ""sv != str; }

	const char* operator*() const { return name ? name->c_str() : ""; }
	string_view ToString() const { return name ? string_view(*name) : ""sv; }

private:
	template<typename Type>
	Name& AcquireName(const Type& str)
	{
		auto [it, _] = names.insert(str);
		name = &*it;
		return *this;
	}
};

template<>
struct std::hash<Name>
{
	size_t operator()(const Name& n) const noexcept { return n.name ? std::hash<void*>()(n.name) : 0; }
};