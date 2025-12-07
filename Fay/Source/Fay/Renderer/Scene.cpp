#include "Scene.h"

namespace Fay
{
	void Scene::addObject(Renderable* object)
	{
		m_objects.push_back(object);
	}
	void Scene::removeObject(Renderable* object)
	{
		// Find and remove from render list
		auto it = std::find(m_objects.begin(), m_objects.end(), object);
		if (it != m_objects.end())
		{
			delete* it;
			m_objects.erase(it);
		}
	}
	void Scene::clear()
	{
		ComponentManager<SpriteComponent>::Get().clear();
		ComponentManager<CubeComponent>::Get().clear();
		ComponentManager<CollisionComponent>::Get().clear();
		ComponentManager<TransformComponent>::Get().clear();
		ComponentManager<ScriptComponent>::Get().clear();
		m_objects.clear();
	}
	void Scene::render(TileLayer* renderingLayer) const
	{
		if (!getObjects().empty())
		{
			renderingLayer->clear();
			for (auto* sp : getObjects())
				renderingLayer->add(sp);
			renderingLayer->render();
		}
	}
	bool Scene::saveScene(const std::string& filepath) const
	{
		std::ofstream out(filepath, std::ios::binary);
		if (!out.is_open()) return false;

		auto entities = getAllEntities();

		std::vector<EntityID> filteredEntities;
		for (EntityID entity : entities)
		{
			bool is2D = ComponentManager<SpriteComponent>::Get().hasComponent(entity);
			bool is3D = ComponentManager<CubeComponent>::Get().hasComponent(entity);

			if ((m_ActiveScene == SceneType::Scene2D && is2D) ||
				(m_ActiveScene == SceneType::Scene3D && is3D))
			{
				filteredEntities.push_back(entity);
			}
		}

		uint32_t entityCount = static_cast<uint32_t>(filteredEntities.size());
		out.write(reinterpret_cast<const char*>(&entityCount), sizeof(uint32_t));

		for (EntityID entity : filteredEntities)
		{
			// Write entity ID
			out.write(reinterpret_cast<const char*>(&entity), sizeof(EntityID));

			// Gather components attached to this entity
			std::vector<std::string> componentTypes; // e.g., "SpriteComponent"

			bool hasSprite = ComponentManager<SpriteComponent>::Get().hasComponent(entity);

			bool hasTransform = ComponentManager<TransformComponent>::Get().hasComponent(entity);
			bool hasCamera = ComponentManager<CameraComponent>::Get().hasComponent(entity);
			bool hasHitBox = ComponentManager<CollisionComponent>::Get().hasComponent(entity);
			bool hasCube = ComponentManager<CubeComponent>::Get().hasComponent(entity);
			bool hasScript = ComponentManager<ScriptComponent>::Get().hasComponent(entity);

			uint32_t componentCount = 0;

			if (hasSprite && m_ActiveScene == SceneType::Scene2D) componentCount++;
			if (hasTransform) componentCount++;
			if (hasCamera) componentCount++;
			if (hasHitBox) componentCount++;
			if (hasCube && m_ActiveScene == SceneType::Scene3D) componentCount++;
			if (hasScript) componentCount++;
			// Write component count
			out.write(reinterpret_cast<const char*>(&componentCount), sizeof(uint32_t));

			// Serialize each component
			if (hasSprite && m_ActiveScene == SceneType::Scene2D)
			{
				std::string compName = "SpriteComponent";
				uint32_t nameLen = static_cast<uint32_t>(compName.size());
				out.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
				out.write(compName.c_str(), nameLen);

				// Write SpriteComponent data
				const SpriteComponent* sprite = ComponentManager<SpriteComponent>::Get().getComponent(entity);
				if (sprite)
				{
					Vec3 pos = sprite->getPosition();
					Vec3 size = sprite->getSize();
					Vec4 color = sprite->getColor();

					out.write(reinterpret_cast<const char*>(&pos), sizeof(Vec3));
					out.write(reinterpret_cast<const char*>(&size), sizeof(Vec3));
					out.write(reinterpret_cast<const char*>(&color), sizeof(Vec4));
					std::string texName = sprite->getTexture() ? sprite->getTexture()->getName() : "";
					writeString(out, texName);
				}
			}
			if (hasTransform)
			{
				std::string compName = "TransformComponent";
				uint32_t nameLen = static_cast<uint32_t>(compName.size());
				out.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
				out.write(compName.c_str(), nameLen);

				const TransformComponent* transform = ComponentManager<TransformComponent>::Get().getComponent(entity);

				if (transform)
				{
					out.write(reinterpret_cast<const char*>(&transform->position), sizeof(Vec3));
					out.write(reinterpret_cast<const char*>(&transform->rotation), sizeof(Vec3));
					out.write(reinterpret_cast<const char*>(&transform->scale), sizeof(Vec3));
				}
			}
			if (hasHitBox)
			{
				std::string compName = "CollisionComponent";
				uint32_t nameLen = static_cast<uint32_t>(compName.size());
				out.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
				out.write(compName.c_str(), nameLen);

				const CollisionComponent* hitBox = ComponentManager<CollisionComponent>::Get().getComponent(entity);

				if (hitBox)
				{
					out.write(reinterpret_cast<const char*>(&hitBox->pos), sizeof(Vec3));
					out.write(reinterpret_cast<const char*>(&hitBox->size), sizeof(Vec2));

				}
			}
			if (hasScript)
			{
				std::string compName = "ScriptComponent";
				uint32_t nameLen = static_cast<uint32_t>(compName.size());
				out.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
				out.write(compName.c_str(), nameLen);

				const ScriptComponent* scriptComp = ComponentManager<ScriptComponent>::Get().getComponent(entity);

				if (scriptComp)
				{
					writeString(out, scriptComp->className);
					out.write(reinterpret_cast<const char*>(&scriptComp->entityId), sizeof(uint32_t));
				}
			}
			if (hasCube && m_ActiveScene == SceneType::Scene3D)
			{
				std::string compName = "CubeComponent";
				uint32_t nameLen = static_cast<uint32_t>(compName.size());
				out.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
				out.write(compName.c_str(), nameLen);

				// Write CubeComponent data
				const CubeComponent* cube = ComponentManager<CubeComponent>::Get().getComponent(entity);

				Vec3 pos = cube->getPosition();
				Vec3 size = cube->getSize();
				Vec4 color = cube->getColor();
				bool hasCol = cube->getCollision();
				// Serialize members (pos, size, color, hasCollision)
				out.write(reinterpret_cast<const char*>(&pos), sizeof(Vec3));
				out.write(reinterpret_cast<const char*>(&size), sizeof(Vec3));
				out.write(reinterpret_cast<const char*>(&color), sizeof(Vec4));
			}
			// add camera later
		}
		return true;
	}

	bool Scene::loadScene(const std::string& filepath, TextureManager tm)
	{
		std::ifstream in(filepath, std::ios::binary);
		if (!in.is_open()) return false;

		clear();

		uint32_t entityCount;
		in.read(reinterpret_cast<char*>(&entityCount), sizeof(uint32_t));

		for (uint32_t i = 0; i < entityCount; i++)
		{
			EntityID entity;
			in.read(reinterpret_cast<char*>(&entity), sizeof(EntityID));

			uint32_t componentCount;
			in.read(reinterpret_cast<char*>(&componentCount), sizeof(uint32_t));

			for (uint32_t c = 0; c < componentCount; c++)
			{
				uint32_t nameLen;
				in.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));
				std::string compName(nameLen, '\0');
				in.read(&compName[0], nameLen);

				if (compName == "SpriteComponent")
				{
					Vec3 pos;
					Vec3 size;
					Vec4 color;
					std::string texName;

					in.read(reinterpret_cast<char*>(&pos), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&size), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&color), sizeof(Vec4));

					std::cout << "EntityID: " << entity << std::endl;
					std::cout << "Color: " << color.x << "," << color.y << "," << color.z << "," << color.w << std::endl;
					std::cout << "Position: "  << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
					texName = readString(in);

					Sprite* sprite = nullptr;

					if (!texName.empty())
					{
						Texture* tex = TextureManager::getTexture(texName);
						sprite = new Sprite(entity, pos.x, pos.y, pos.z, size.x, size.y, size.z, tex);
					}
					else
					{
						sprite = new Sprite(entity, pos.x, pos.y, pos.z, size.x, size.y, size.z, color);
					}
					m_objects.push_back(sprite);
					std::cout << "Sprite ID: " << entity << ", Sprite color " << sprite->getColor() << std::endl;
					SpriteComponent spriteComp(sprite);

					ComponentManager<SpriteComponent>::Get().addComponent(entity, spriteComp);
				}
				else if (compName == "TransformComponent")
				{
					Vec3 pos, rot, scale;
					in.read(reinterpret_cast<char*>(&pos), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&rot), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&scale), sizeof(Vec3));

					ComponentManager<TransformComponent>::Get().addComponent(entity, TransformComponent(pos, rot, Vec3(0, 0, 0), scale));
				}
				else if (compName == "CollisionComponent")
				{
					Vec3 pos;
					Vec3 size;
					in.read(reinterpret_cast<char*>(&pos), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&size), sizeof(Vec2));

					ComponentManager<CollisionComponent>::Get().addComponent(entity, CollisionComponent(pos, size));
				}
				else if (compName == "ScriptComponent")
				{
					std::string className;
					uint32_t entityID;
					className = readString(in);
					in.read(reinterpret_cast<char*>(&entityID), sizeof(uint32_t));

					ComponentManager<ScriptComponent>::Get().addComponent(entity, ScriptComponent(className, entityID));
				}
				else if (compName == "CubeComponent")
				{
					Vec3 pos;
					Vec3 size;
					Vec4 color;

					in.read(reinterpret_cast<char*>(&pos), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&size), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&color), sizeof(Vec4));

					Cube* cube = nullptr;

					cube = new Cube(entity, pos.x, pos.y, pos.z, size.x, size.y, size.z, color);

					m_objects.push_back(cube);

					CubeComponent cubeComp(cube);

					ComponentManager<CubeComponent>::Get().addComponent(entity, cubeComp);
				}
			}
		}
		return true;
	}
	bool Scene::deleteSceneFile(const std::string& filepath)
	{
		try {
			return std::filesystem::remove(filepath);
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::cerr << "Error deleting file: " << e.what() << std::endl;
			return false;
		}
	}
	std::vector<std::string> Scene::listScenesDir(const std::string& dir)
	{
		std::vector<std::string> files;
		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (entry.path().extension() == ".fayScene")
				files.push_back(entry.path().filename().string());
		}
		return files;
	}
	std::vector<EntityID> Scene::getAllEntities() const
	{
		std::set<EntityID> allEntities;

		auto& spriteEntities = ComponentManager<SpriteComponent>::Get().getEntities();
		allEntities.insert(spriteEntities.begin(), spriteEntities.end());

		auto& cameraEntities = ComponentManager<CameraComponent>::Get().getEntities();
		allEntities.insert(cameraEntities.begin(), cameraEntities.end());

		auto& transformEntities = ComponentManager<TransformComponent>::Get().getEntities();
		allEntities.insert(transformEntities.begin(), transformEntities.end());

		auto& cubeEntities = ComponentManager<CubeComponent>::Get().getEntities();
		allEntities.insert(cubeEntities.begin(), cubeEntities.end());

		return std::vector<EntityID>(allEntities.begin(), allEntities.end());
	}
	void Scene::setSceneType(SceneType type)
	{
		if (m_ActiveScene == type)
			return;

		if ((m_ActiveScene == SceneType::Scene2D && has2DEntities()) ||
			(m_ActiveScene == SceneType::Scene3D && has3DEntities()))
		{
			std::cout << "Cannot switch scene mode - scene already contains "
				<< (m_ActiveScene == SceneType::Scene2D ? "2D" : "3D")
				<< " entities.\n";
			return;
		}
		m_ActiveScene = type;
	}
	bool Scene::canSwitchScene() const
	{
		if (m_ActiveScene == SceneType::Scene2D && has2DEntities())
			return false;
		if (m_ActiveScene == SceneType::Scene3D && has3DEntities())
			return false;
		return true;
	}
	EntityID Scene::getNextId()
	{
		if (m_objects.empty())
			return 1;
		uint32_t maxId = 1;
		std::set<uint32_t> usedIds;
		for (const auto& obj : m_objects)
		{
			usedIds.insert(obj->getId());
			if (obj->getId() > maxId)
				maxId = obj->getId();
		}

		// Look for the first gap
		for (int i = 1; i <= maxId; ++i)
		{
			if (usedIds.find(i) == usedIds.end())
				return i; // reuse id
		}
		return maxId + 1; // no gaps, assign next id
	}
	bool Scene::has2DEntities() const
	{
		for (auto e : getAllEntities())
			if (ComponentManager<SpriteComponent>::Get().hasComponent(e))
				return true;
		return false;

	}

	bool Scene::has3DEntities() const
	{
		for (auto e : getAllEntities())
			if (ComponentManager<CubeComponent>::Get().hasComponent(e))
				return true;
		return false;
	}

	void Scene::writeString(std::ofstream& out, const std::string& str) const
	{
		uint32_t len = str.size();
		out.write(reinterpret_cast<const char*>(&len), sizeof(uint32_t));
		out.write(str.data(), len);
	}
	std::string Scene::readString(std::ifstream& in) const
	{
		uint32_t len = 0;
		in.read(reinterpret_cast<char*>(&len), sizeof(uint32_t));
		std::string str(len, '\0');
		in.read(&str[0], len);
		return str;
	}
}