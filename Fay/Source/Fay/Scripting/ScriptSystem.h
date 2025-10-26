#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <Entity/Entity.h>
#include <Entity/Components.h>
namespace Fay
{
		class ScriptSystem
		{
			void OnStart(EntityID entity, ScriptComponent& script);

			void OnUpdate(EntityID entity, ScriptComponent& script);
		};
	}