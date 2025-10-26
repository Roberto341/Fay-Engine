#include <Scripting/ScriptSystem.h>

namespace Fay
{
		void ScriptSystem::OnStart(EntityID entity, ScriptComponent& script)
		{
			if (script.onStartMethod && script.instance)
				mono_runtime_invoke(script.onStartMethod, nullptr, nullptr, nullptr);
		}

		void ScriptSystem::OnUpdate(EntityID entity, ScriptComponent& script)
		{
			if (script.onUpdateMethod && script.instance)
				mono_runtime_invoke(script.onUpdateMethod, nullptr, nullptr, nullptr);
		}
	}