#include "CoreInterface.h"
#include "Network.h"
#include "CoreEngine.h"

static path content_dir;

unique_ptr<IFuture<INetwork>> INetwork::Connect(span<RemotePlayer> targets, int player)
{
	return make_unique<Task<INetwork>>(Network::Connect(targets, player));
}

void SetContentPath(path dir)
{
	content_dir = dir;
}

Json::Value& GetJson(Name name)
{
	static unordered_map<Name, Json::Value> jvalues;

	auto [it, ins] = jvalues.try_emplace(name);
	Json::Value& jval = it->second;

	if (ins)
	{
		path json_path = content_dir / *name += ".json";
		ifstream file(json_path);
		string json_string;
		json_string.resize(file_size(json_path));
		file.read(json_string.data(), json_string.size());

		Json::Reader reader;
		reader.parse(json_string.data(), json_string.data() + json_string.size(), jval);
	}

	return jval;
}

Json::Value& MakeJson(string_view json_str)
{
	static Json::Value jval;
	Json::Reader reader;
	reader.parse(json_str.data(), json_str.data() + json_str.size(), jval);
	return jval;
}
