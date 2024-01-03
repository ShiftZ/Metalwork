#include "CoreInterface.h"
#include "Network.h"
#include "CoreEngine.h"

METALWORKCORE_API path content_dir;
METALWORKCORE_API bool reload_modified;

unique_ptr<IFuture<INetwork>> INetwork::Connect(span<RemotePlayer> targets, int player)
{
	return make_unique<Task<INetwork>>(Network::Connect(targets, player));
}

void SetContentPath(path dir, bool reload_modifed)
{
	::content_dir = dir;
	::reload_modified = reload_modifed;
}

Json::Value& GetJson(Name name)
{
	static unordered_map<Name, pair<Json::Value, file_time_type>> jvalues;

	auto [it, ins] = jvalues.try_emplace(name);
	auto& [jval, time] = it->second;

	if (!ins && !reload_modified) return jval;

	path json_path = content_dir / *name += ".json";

	if (reload_modified)
	{
		file_time_type mod_time = last_write_time(json_path);
		ins = (time != mod_time);
		time = mod_time;
	}

	if (ins)
	{
		ifstream file(json_path);
		string json_string;
		json_string.resize(file_size(json_path));
		file.read(json_string.data(), json_string.size());

		Json::Reader reader;
		reader.parse(json_string.data(), json_string.data() + json_string.size(), jval);
	}

	return jval;
}