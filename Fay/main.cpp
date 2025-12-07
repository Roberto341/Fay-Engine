#include <Core/Core.h>
int main()
{
	Fay::Window window("Fay Editor", 1920, 1080);
	if (!window.getWindow()) return -1;
	Fay::Editor editor(window);

	// Create and load scripting engine
	Fay::ScriptGlue::SetWindow(window);
	Fay::ScriptEngine::Init();
	Fay::ScriptEngine::LoadAssembly("Source/Fay/Scripting/FayRuntime/bin/Debug/net48/FayRuntime.dll", Fay::ScriptEngine::GetRootDomain());
	Fay::ScriptGlue::RegisterFunctions();
	Fay::ScriptGlue::RegisterComponents();
	Fay::ScriptEngine::InvokeRootStatic("EntityScript", "OnStart");
	
	editor.run();

	return 0;
}