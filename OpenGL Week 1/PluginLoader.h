#pragma once
#include <string>
#include "Debug.h"
#include "EnginePluginAPI.h"
#include <Windows.h>

class ENGINE_API PluginLoader
{
public:
	PluginLoader(const std::string& dllpath);
	~PluginLoader(){
		Unload();
	}

	bool Load();
	void Unload();
private:
	std::string _dllPath;
	HMODULE     _hModule;
};

