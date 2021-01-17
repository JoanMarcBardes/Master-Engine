#pragma once
#include "Libraries/parson/parson.h"
#include "Globals.h"
#include "Libraries/MathGeoLib/MathGeoLib.h"
#include <string>
#include <vector>

class Config_Array;
class Config
{
public:
	Config();						//Contructor used for new files
	Config(const char* buffer);		//Constructor used for data read
	Config(JSON_Object* obj);		//Constructor used for node append
	~Config();						//Free data if initialized

	unsigned Serialize(char** buffer);	//Returns a filled buffer
	bool NodeExists();
	void Release();

	//Set
	void SetNumber(const char* name, double data);
	void SetString(const char* name, const char* data);
	void SetBool(const char* name, bool data);
	Config_Array SetArray(const char* name);
	Config SetNode(const char* name);

	//Get
	double GetNumber(const char* name, double default = 0) const;
	std::string GetString(const char* name, const char* default = "") const;
	bool GetBool(const char* name, bool default = true) const;
	Config_Array GetArray(const char* name) const;
	Config GetNode(const char* name) const;

private:
	JSON_Value* rootValue = nullptr; //Only used for file root
	JSON_Object* node = nullptr;
};

class Config_Array
{
public:
	//Contructor only to be called from Config, it would cause mem leak
	Config_Array();
	Config_Array(JSON_Array* arr);

	//Append attributes ------------
	void AddNumber(double number);
	void AddString(char* string);
	void AddBool(bool boolean);
	void AddFloat3(const float3& data);
	void AddFloat4(const float4& data);

	void AddQuat(const Quat& data);
	Config AddNode();
	//Endof append attributes-------

	//Get attributes ---------------
	double GetNumber(unsigned index, double default = 0) const;
	const char* GetString(unsigned index, const char* default = "") const;
	bool GetBool(unsigned index, bool default = true) const;
	float3 GetFloat3(unsigned index, float3 default = float3::zero) const; //Index is based on float3 not on single data!
	float4 GetFloat4(unsigned index, float4 default = float4::zero) const; //Index is based on float4 not on single data!
	Quat GetQuat(unsigned index, Quat  default = Quat::identity) const;
	void FillVectorNumber(std::vector<double>& vector) const;
	void FillVectorString(std::vector<char*>& vector) const;
	void FillVectorBoool(std::vector<bool>& vector) const;
	Config GetNode(unsigned index) const;
	unsigned GetSize() const;
	//Endof Get attributes----------

private:
	JSON_Array* arr;
	unsigned size = 0;
};