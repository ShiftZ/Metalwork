#pragma once

#include "Mirror/MirrorExpress.h"

#undef CLASS
#undef ENUM
#undef PROPERTY
#undef METHOD

#define CLASS(Type, ...) MIRROR_CLASS(Type, MetalProperty, Mirror::Method, METALWORKCORE_API, __VA_ARGS__)
#define ENUM(Type, ...) MIRROR_ENUM(Type, METALWORKCORE_API, __VA_ARGS__)
#define PROPERTY(Property, ...) MIRROR_PROPERTY(Property, METALWORKCORE_API, __VA_ARGS__)
#define METHOD(Name) MIRROR_METHOD(Name, METALWORKCORE_API)

class MetalProperty : public Mirror::Property
{
	function<void(void*, string_view)> read_string;

public:
	template< typename MetaType >
	MetalProperty( MetaType* meta ) : Property(meta)
	{
		if (is_enum_v<typename MetaType::Type>)
			read_string = [this](void* obj, string_view str){ Property::SetValue(obj, Mirror::Enum<typename MetaType::Type>::GetValue(str)); };
		else if (is_same_v<typename MetaType::Type, Name>)
			read_string = [this](void* obj, string_view str){ Property::SetValue(obj, Name(str)); };
	}

	using Property::SetValue;
	void SetValue(auto& obj, string& str) const { read_string ? read_string(&obj, str) : Property::SetValue(obj, str); }
};