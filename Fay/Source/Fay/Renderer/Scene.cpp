#include <Renderer/Scene.h>
namespace Fay
{
	void Scene::addNode(ControlNode* node)
	{
		m_nodes.push_back(node);
	}
	void Scene::removeNode(ControlNode* node)
	{
		auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
		if (it != m_nodes.end())
		{
			delete* it;
			m_nodes.erase(it);
		}
	}
	void Scene::destroyNode(NodeID id)
	{
		ComponentManager<ControllerComponent>::Get().removeNodeComponent(id);
		// Remove node reference
		auto it = std::find_if(m_nodes.begin(), m_nodes.end(), 
			[&](Node* node) {return node->getId() == id; });

		if (it != m_nodes.end())
		{
			removeNode(*it);
		}
	}
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
			delete *it;
			m_objects.erase(it);
		}
	}
	void Scene::destroyEntity(EntityID id)
	{
		ComponentManager<SpriteComponent>::Get().removeComponent(id);
		ComponentManager<CubeComponent>::Get().removeComponent(id);
		ComponentManager<ScriptComponent>::Get().removeComponent(id);
		// remove renderable reference
		auto it = std::find_if(m_objects.begin(), m_objects.end(), 
			[&](Renderable* obj) {return obj->getId() == id; });
		
		if (it != m_objects.end())
		{
			removeObject(*it);
		}
	}
	void Scene::clear()
	{
		ComponentManager<SpriteComponent>::Get().clear();
		ComponentManager<CubeComponent>::Get().clear();
		ComponentManager<CollisionComponent>::Get().clear();
		ComponentManager<TransformComponent>::Get().clear();
		ComponentManager<ScriptComponent>::Get().clear();
		ComponentManager<ControllerComponent>::Get().clear();

		// Node
		ComponentManager<ScriptComponent>::Get().clearNodeComponents();
		ComponentManager<ControllerComponent>::Get().clearNodeComponents();
		m_objects.clear();
		m_nodes.clear();
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
	ControlNode* Scene::getNodeByIndex(NodeID index) const
	{
		if (index >= m_nodes.size())
			return nullptr;
		return m_nodes[index];
	}
	ControlNode* Scene::getNodeById(NodeID id) const
	{
		for (auto* node : m_nodes)
		{
			if (node && node->getId() == id)
				return node;
		}
		return nullptr;
	}
	bool Scene::saveScene(const std::string& filepath) const
	{
		std::ofstream out(filepath, std::ios::binary);
		if (!out.is_open()) return false;

		auto entities = getAllEntities();

		std::vector<EntityID> filteredNodeEntities;

		// =======================================
		// Save Nodes
		// =======================================

		std::vector<NodeID> nodes;

		for (int i = 0; i < getNodeCount(); i++)
		{
			ControlNode* node = getNodeByIndex(i);

			if (node)
			{
				nodes.push_back(node->getId());
			}
		}

		uint32_t nodeCount = static_cast<uint32_t>(nodes.size());
		out.write(reinterpret_cast<const char*>(&nodeCount), sizeof(uint32_t));
			
		for (NodeID nodeID : nodes)
		{
			ControlNode* node = getNodeById(nodeID);

			if (!node)
			{
				FAY_LOG_WARN("Invalid NodeID found during save: " << nodeID);
				continue;
			}

			out.write(reinterpret_cast<const char*>(&nodeID), sizeof(NodeID));

			writeString(out, node->getName());

			uint32_t childCount = 0;

			auto* controller = ComponentManager<ControllerComponent>::Get().getNodeComponent(nodeID);

			if (controller)
			{
				const auto& children = controller->getEntities();
				childCount = static_cast<uint32_t>(children.size());

				out.write(reinterpret_cast<const char*>(&childCount), sizeof(uint32_t));

				for (EntityID entity : children)
				{
					out.write(reinterpret_cast<const char*>(&entity), sizeof(EntityID));
				}
			}
			else
			{
				out.write(reinterpret_cast<const char*>(&childCount), sizeof(uint32_t));
			}

			bool nodeHasScript = ComponentManager<ScriptComponent>::Get().getNodeComponent(nodeID);

			FAY_LOG_DEBUG("Node ID: " << nodeID << " has script: " << (nodeHasScript ? "Yes" : "No"));

			uint32_t nodeComponentCount = nodeHasScript ? 1 : 0;

			out.write(reinterpret_cast<const char*>(&nodeComponentCount), sizeof(uint32_t));

			if (nodeHasScript)
			{

				const ScriptComponent* scriptComp =
					ComponentManager<ScriptComponent>::Get().getNodeComponent(nodeID);

				if (!scriptComp)
					FAY_LOG_ERROR("Node has ScriptComponent according to manager, "
						"but getNodeComponent returned nullptr: " << nodeID);

				std::string compName = "NodeScript";

				uint32_t nameLen = static_cast<uint32_t>(compName.size());

				out.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));

				out.write(compName.data(), nameLen);

				uint32_t scriptCount = static_cast<uint32_t>(scriptComp->scripts.size());

				out.write(reinterpret_cast<const char*>(&scriptCount), sizeof(uint32_t));

				for (const auto& script : scriptComp->scripts)
				{
					writeString(out, script.className);
				}
			}
		}

		// =============================
		// Save Entities
		// =============================
		std::vector<EntityID> filteredEntities;

		for (EntityID entity : entities)
		{
			bool is2D = ComponentManager<SpriteComponent>::Get().hasComponent(entity);
			bool is3D = ComponentManager<CubeComponent>::Get().hasComponent(entity);
			
			bool isChildEntity = false;

			// Check if this entity belongs to a node

			for (NodeID nodeID : nodes)
			{
				ControlNode* node = getNodeById(nodeID);

				if (!node)
					continue;

				if (ComponentManager<ControllerComponent>::Get().getNodeComponent(nodeID)->hasEntity(entity))
				{
					isChildEntity = true;
					break;
				}
			}

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
					std::string tags = sprite->getAllTagsAsString();
					
					out.write(reinterpret_cast<const char*>(&pos), sizeof(Vec3));
					out.write(reinterpret_cast<const char*>(&size), sizeof(Vec3));
					out.write(reinterpret_cast<const char*>(&color), sizeof(Vec4));
					std::string texName = sprite->getTexture() ? sprite->getTexture()->getName() : "";
					writeString(out, texName);
					writeString(out, tags);
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
					uint32_t scriptCount = static_cast<uint32_t>(scriptComp->scripts.size());
					out.write(reinterpret_cast<const char*>(&scriptCount), sizeof(uint32_t));
					for (auto& s : scriptComp->scripts)
						writeString(out, s.className);
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
				std::string tags = cube->getAllTagsAsString();

				// Serialize members (pos, size, color, hasCollision)
				out.write(reinterpret_cast<const char*>(&pos), sizeof(Vec3));
				out.write(reinterpret_cast<const char*>(&size), sizeof(Vec3));
				out.write(reinterpret_cast<const char*>(&color), sizeof(Vec4));
				writeString(out, tags);

			}
			// add camera later
		}
		return true;
	}

	bool Scene::saveSceneAs(const std::string& filepath) const
	{
		if (!saveScene(filepath))
			return false;
		return true;
	}

	bool Scene::loadScene(const std::string& filepath, TextureManager tm)
	{
		std::ifstream in(filepath, std::ios::binary);
		if (!in.is_open()) return false;

		clear();

		// ===============================
		// Load Nodes
		// ===============================

		uint32_t nodeCount = 0;

		in.read(reinterpret_cast<char*>(&nodeCount), sizeof(uint32_t));

		struct PendingNode
		{
			NodeID id{};
			std::string name{};
			std::vector<EntityID> entities {};

			std::vector<std::string> scripts{};
		};

		std::vector<PendingNode> pendingNodes;

		for (uint32_t i = 0; i < nodeCount; i++)
		{
			NodeID nodeID;

			in.read(reinterpret_cast<char*>(&nodeID), sizeof(NodeID));
			
			std::string nodeName = readString(in);

			uint32_t childCount = 0;
			
			in.read(reinterpret_cast<char*>(&childCount), sizeof(uint32_t));

			PendingNode node;
			node.id = nodeID;
			node.name = nodeName;
			for (uint32_t j = 0; j < childCount; j++)
			{
				EntityID entity;

				in.read(reinterpret_cast<char*>(&entity), sizeof(EntityID));

				node.entities.push_back(entity);
			}
			
			// ====================================
			// Load Node Components
			// ====================================

			uint32_t nodeComponentCount = 0;

			in.read(reinterpret_cast<char*>(&nodeComponentCount), sizeof(uint32_t));

			for (uint32_t c = 0; c < nodeComponentCount; c++)
			{
				uint32_t nameLen = 0;

				in.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));

				std::string compName(nameLen, '\0');

				in.read(compName.data(), nameLen);

				if (compName == "NodeScript")
				{
					uint32_t scriptCount = 0;

					in.read(reinterpret_cast<char*>(&scriptCount), sizeof(uint32_t));

					for (uint32_t s = 0; s < scriptCount; s++)
					{
						std::string className = readString(in);
					
						node.scripts.push_back(className);
					}
				}
			}

			pendingNodes.push_back(node);

		}


		// ====================================
		// Load Entities
		// ====================================

		uint32_t entityCount = 0;
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
					std::string tagsString;

					in.read(reinterpret_cast<char*>(&pos), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&size), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&color), sizeof(Vec4));

					texName = readString(in);

					// Read saved tags
					tagsString = readString(in);

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
					SpriteComponent spriteComp(sprite);

					// Add each saved string
					std::stringstream ss(tagsString);
					std::string tag;

					while (std::getline(ss, tag, ','))
					{
						if (!tag.empty())
						{
							spriteComp.addTag(tag);
						}
					}

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
					ScriptComponent sc(entity);
					uint32_t scriptCount;
					in.read(reinterpret_cast<char*>(&scriptCount), sizeof(uint32_t));

					for (uint32_t i = 0; i < scriptCount; i++)
					{
						std::string className = readString(in);
						sc.scripts.emplace_back(className);
					}
					ComponentManager<ScriptComponent>::Get().addComponent(entity, sc);
				}
				else if (compName == "CubeComponent")
				{
					Vec3 pos;
					Vec3 size;
					Vec4 color;
					std::string tagsString;

					in.read(reinterpret_cast<char*>(&pos), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&size), sizeof(Vec3));
					in.read(reinterpret_cast<char*>(&color), sizeof(Vec4));

					tagsString = readString(in);

					Cube* cube = nullptr;

					cube = new Cube(entity, pos.x, pos.y, pos.z, size.x, size.y, size.z, color);

					m_objects.push_back(cube);

					CubeComponent cubeComp(cube);

					// Add each saved string
					std::stringstream ss(tagsString);
					std::string tag;

					while (std::getline(ss, tag, ','))
					{
						if (!tag.empty())
						{
							cubeComp.addTag(tag);
						}
					}

					ComponentManager<CubeComponent>::Get().addComponent(entity, cubeComp);
				}
			}
		}
		// ===========================
		// Restore Node Relationships
		// ===========================

		for (auto& pending : pendingNodes)
		{
			auto* node = new ControlNode(
				pending.id,
				pending.name
			);

			ComponentManager<ControllerComponent>::Get().addNodeComponent(pending.id, ControllerComponent());

			// Restore node scripts
			if (!pending.scripts.empty())
			{
				ScriptComponent scriptComp(pending.id);

				for (const auto& className : pending.scripts)
				{
					scriptComp.scripts.emplace_back(className);
				}

				ComponentManager<ScriptComponent>::Get().addNodeComponent(pending.id, scriptComp);
			}

			addNode(node);

			auto* controller =
				ComponentManager<ControllerComponent>::Get().getNodeComponent(pending.id);

			if (controller)
			{
				for (EntityID entity : pending.entities)
				{
					controller->addEntity(entity);
				}
			}
		}

		return true;
	}
	bool Scene::deleteSceneFile(const std::string& filepath)
	{
		clear();
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
	NodeID Scene::getNextNodeId()
	{
		if (m_nodes.empty())
			return 1;
		uint32_t maxId = 1;
		std::set<uint32_t> usedIds;
		for (const auto& nd : m_nodes)
		{
			usedIds.insert(nd->getId());
			if (nd->getId() > maxId)
				maxId = nd->getId();
		}

		// Look for the first gap

		for (int i = 1; i <= maxId; ++i)
		{
			if (usedIds.find(i) == usedIds.end())
				return i;
		}
		return maxId + 1;
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