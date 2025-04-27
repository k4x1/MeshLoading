#pragma once


struct EngineAPI {
	void (*RegisterComponent)(const char* name, CreateFunc, SerializeFunc);
};

extern "C" {
	__declspec(dllexport)
		void GamePlugin_Initialize(EngineAPI* engine);

	__declspec(dllexport)
		void GamePlugin_Update(float deltaTime);

	__declspec(dllexport)
		void GamePlugin_Shutdown();
}
