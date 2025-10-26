#include <Core/Core.h>
int main()
{
	Fay::Window window("Fay Editor", 1920, 1080);
	if (!window.getWindow()) return -1;
	Fay::Editor editor(window);

	// Create and load scripting engine
	Fay::ScriptGlue::SetWindow(window);
	Fay::ScriptEngine::Init();
	Fay::ScriptEngine::LoadAssembly("Source/Fay/Scripting/FayRuntime/bin/Debug/FayRuntime.dll");

	Fay::ScriptGlue::RegisterFunctions();
	Fay::ScriptGlue::RegisterComponents();
	Fay::ScriptEngine::InvokeStatic("MyScript", "OnStart");
	Fay::ScriptEngine::InvokeStatic("MyScript", "OnUpdate");
	Fay::ScriptEngine::InvokeStatic("EntityScript", "OnStart");
	//Fay::ScriptEngine::InvokeStatic("PlayerController", "OnStart"); // Still a work in progress
	editor.run();
	return 0;
}