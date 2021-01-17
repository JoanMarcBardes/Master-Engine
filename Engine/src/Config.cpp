#include "Config.h"

Config::Config()
{
	rootValue = json_value_init_object();
	node = json_value_get_object(rootValue);
}


Config::Config(const char* buffer)
{
	rootValue = json_parse_string(buffer);
	if (rootValue)
	{
		node = json_value_get_object(rootValue);
	}
}

Config::Config(JSON_Object* obj) : node(obj)
{

}

Config::~Config()
{
	Release();
}

unsigned Config::Serialize(char** buffer)
{
	size_t size = json_serialization_size_pretty(rootValue);
	*buffer = new char[size];
	json_serialize_to_buffer_pretty(rootValue, *buffer, size);
	return size;
}

bool Config::NodeExists()
{
	return rootValue != nullptr;
}

void Config::Release()
{
	if (rootValue)
	{
		json_value_free(rootValue);
	}
}

void Config::SetNumber(const char* name, double data)
{
	json_object_set_number(node, name, data);
}

void Config::SetString(const char* name, const char* data)
{
	json_object_set_string(node, name, data);
}

void Config::SetBool(const char* name, bool data)
{
	json_object_set_boolean(node, name, data);
}

Config_Array Config::SetArray(const char* name)
{
	json_object_set_value(node, name, json_value_init_array());
	return Config_Array(json_object_get_array(node, name));
}

Config Config::SetNode(const char* name)
{
	json_object_set_value(node, name, json_value_init_object());
	return Config(json_object_get_object(node, name));
}

//Get
double Config::GetNumber(const char* name, double default) const
{
	if (json_object_has_value_of_type(node, name, JSONNumber))
		return json_object_get_number(node, name);
	return default;
}

std::string Config::GetString(const char* name, const char* default) const
{
	if (json_object_has_value_of_type(node, name, JSONString))
		return json_object_get_string(node, name);
	return default;
}

bool Config::GetBool(const char* name, bool default) const
{
	if (json_object_has_value_of_type(node, name, JSONBoolean))
		return json_object_get_boolean(node, name);
	return default;
}

Config_Array Config::GetArray(const char* name) const
{
	if (json_object_has_value_of_type(node, name, JSONArray))
		return Config_Array(json_object_get_array(node, name));
	else
	{
		LOG("[error] Array '%s' not found when loading scene", name);
		return Config_Array();
	}
}

Config Config::GetNode(const char* name) const
{
	return Config(json_object_get_object(node, name));
}

//// Config_Array ////

Config_Array::Config_Array()
{
	arr = json_value_get_array(json_value_init_array());
}

Config_Array::Config_Array(JSON_Array* arr) : arr(arr)
{
	size = json_array_get_count(arr);
}

//Set
void Config_Array::AddNumber(double number)
{
	json_array_append_number(arr, number);
	size++;
}

void Config_Array::AddString(char* string)
{
	json_array_append_string(arr, string);
	size++;
}

void Config_Array::AddBool(bool boolean)
{
	json_array_append_boolean(arr, boolean);
	size++;
}

void Config_Array::AddFloat3(const float3& data)
{
	json_array_append_number(arr, data.x);
	json_array_append_number(arr, data.y);
	json_array_append_number(arr, data.z);
	size += 3;
}

void Config_Array::AddFloat4(const float4& data)
{
	json_array_append_number(arr, data.x);
	json_array_append_number(arr, data.y);
	json_array_append_number(arr, data.z);
	json_array_append_number(arr, data.w);
	size += 4;
}

void Config_Array::AddQuat(const Quat& data)
{
	json_array_append_number(arr, data.x);
	json_array_append_number(arr, data.y);
	json_array_append_number(arr, data.z);
	json_array_append_number(arr, data.w);
	size += 4;
}

Config Config_Array::AddNode()
{
	json_array_append_value(arr, json_value_init_object());
	size++;
	return Config(json_array_get_object(arr, size - 1));
}

//Get
double Config_Array::GetNumber(unsigned index, double default) const
{
	if (index < size)
		return json_array_get_number(arr, index);
	else
	{
		LOG("[Warning] JSON Array: Index out of size");
		return 0;
	}
}

const char* Config_Array::GetString(unsigned index, const char* default) const
{
	if (index < size)
		return json_array_get_string(arr, index);
	else
	{
		LOG("[Warning] JSON Array: Index out of size");
		return default;
	}
}

float3 Config_Array::GetFloat3(unsigned index, float3 default) const
{
	index *= 3;
	float3 ret = default;

	ret.x = GetNumber(index + 0, ret.x);
	ret.y = GetNumber(index + 1, ret.y);
	ret.z = GetNumber(index + 2, ret.z);

	return ret;
}

float4 Config_Array::GetFloat4(unsigned index, float4 default) const
{
	index *= 4;
	float4 ret = default;

	ret.x = GetNumber(index + 0, ret.x);
	ret.y = GetNumber(index + 1, ret.y);
	ret.z = GetNumber(index + 2, ret.z);
	ret.w = GetNumber(index + 3, ret.z);

	return ret;
}

Quat Config_Array::GetQuat(unsigned index, Quat  default) const
{
	index *= 4;
	Quat ret = default;

	ret.x = GetNumber(index + 0, ret.x);
	ret.y = GetNumber(index + 1, ret.y);
	ret.z = GetNumber(index + 2, ret.z);
	ret.w = GetNumber(index + 3, ret.w);

	return ret;
}

bool Config_Array::GetBool(unsigned index, bool default) const
{
	if (index < size)
		return json_array_get_boolean(arr, index);
	else
	{
		LOG("[Warning] JSON Array: Index out of size");
		return default;
	}
}

void Config_Array::FillVectorNumber(std::vector<double> & vector) const
{
	for (unsigned i = 0; i < size; i++)
	{
		vector.push_back(GetNumber(i));
	}
}

void Config_Array::FillVectorString(std::vector<char*> & vector) const
{
	for (unsigned i = 0; i < size; i++)
	{
		vector.push_back((char*)GetString(i));
	}
}

void Config_Array::FillVectorBoool(std::vector<bool> & vector) const
{
	for (unsigned i = 0; i < size; i++)
	{
		vector.push_back(GetBool(i));
	}
}

Config Config_Array::GetNode(unsigned index) const
{
	return Config(json_array_get_object(arr, index));
}

unsigned Config_Array::GetSize() const
{
	return size;
}