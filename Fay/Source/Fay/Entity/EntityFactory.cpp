#include <Entity/EntityFactory.h>

namespace Fay
{
	EntityFactory::EntityFactory(Scene* scene, TileLayer* layer, EditorUtils* utils)
		: m_scene(scene), m_layer(layer), m_utils(utils)
	{
	}

	EntityID EntityFactory::CreateSprite()
	{
		EntityID entity = m_scene->getNextId();

		auto* sprite = new Sprite(
			entity, 
			0, 0, 0, 
			100, 100, 0, 
			Vec4(1, 1, 1, 1)
		);


		ComponentManager<SpriteComponent>::Get().addComponent(entity, SpriteComponent(sprite));

		m_scene->addObject(sprite);

		return entity;
	}
	EntityID EntityFactory::CreateCube()
	{
		EntityID entity = m_scene->getNextId();

		auto* cube = new Cube(
			entity,
			0, 0, 0,
			1, 1, 1,
			Vec4(1, 1, 1, 1)
		);

		ComponentManager<CubeComponent>::Get().addComponent(entity, CubeComponent(cube));

		m_scene->addObject(cube);

		return entity;
	}
	NodeID EntityFactory::CreateControllNode(const std::string& name)
	{
		NodeID nId = m_scene->getNextNodeId();

		auto* node = new ControlNode(
			nId, 
			name
		);

		ComponentManager<ControllerComponent>::Get().addNodeComponent(nId, ControllerComponent());

		m_scene->addNode(node);

		return nId;
	}
	EntityID EntityFactory::CreateEntity(RenderMode mode)
	{
		if (mode == RenderMode::MODE_2D)
			return CreateSprite();

		return CreateCube();
	}
}