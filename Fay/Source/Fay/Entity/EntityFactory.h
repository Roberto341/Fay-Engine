#pragma once

#include <Entity/Components.h>
#include <Renderer/Scene.h>
#include <Renderer/BatchRenderer.h>
#include <EngineEditor/EditorUtils.h>

namespace Fay
{
	class EntityFactory
	{
	public:
		EntityFactory(Scene* scene, TileLayer* layer, EditorUtils* utils);

		EntityID CreateSprite();
		EntityID CreateCube();
		EntityID CreateEntity(RenderMode mode);
	private:
		Scene* m_scene = nullptr;
		TileLayer* m_layer = nullptr;
		EditorUtils* m_utils = nullptr;
	};
}