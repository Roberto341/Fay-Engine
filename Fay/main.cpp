#include <Core/Core.h>
int main(int argc, char** argv)
{
	Fay::Editor editor;

	// Create and load scripting engine
	Fay::ScriptGlue::SetWindow(editor.getWindow());
	Fay::ScriptEngine::Init(argv[0]);
	Fay::ScriptEngine::LoadAssembly(Fay::ScriptEngine::GetRuntimeDll(), Fay::ScriptEngine::GetRootDomain());
	Fay::ScriptGlue::SetEditorUtils(editor.getUtils());
	Fay::ScriptGlue::RegisterFunctions();
	Fay::ScriptGlue::RegisterComponents();
	
	editor.runEditor();
	return 0;
}