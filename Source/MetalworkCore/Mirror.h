#pragma once

#include "Mirror/MirrorExpress.h"

#undef CLASS
#undef ENUM

#define CLASS(_Type_, ...) MIRROR_CLASS(_Type_, MetalProperty, Mirror::Method, METALWORKCORE_API, __VA_ARGS__)
#define ENUM(_Type_, ...) MIRROR_ENUM(_Type_, METALWORKCORE_API, __VA_ARGS__)

class MetalProperty : public Mirror::Property
{
	function<void(MetalProperty*, void*, any)> reader;

public:
	template<typename MetaType>
	MetalProperty(MetaType* meta) : Property(meta)
	{
		if constexpr (is_enum_v<typename MetaType::Type>)
			reader = [](MetalProperty* p, void* obj, any value)
			{
				auto enum_value = Mirror::Enum<typename MetaType::Type>::ToValue(any_cast<string>(value));
				p->SetValue(obj, enum_value);
			};
		else
			reader = [](MetalProperty* p, void* obj, any value){ p->SetAny(obj, value); };
	}
};

namespace Json { class Value; }

