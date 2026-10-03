#pragma once
#include <filesystem>
#include <set>
#include <Renderer/Sprite.h>
#include <Renderer/Cube.h>
#include <Renderer/ControlNode.h>
#include <Math/Math.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureManager.h>
#include <Core/Logger.h>
#include <Entity/Entity.h>
#include <Entity/ComponentManager.h>
#include <Entity/Components.h>
#include <Graphics/Layers/TileLayer.h>
namespace Fay
{
	class Scene
	{
	public:
		// Nodes
		void addNode(ControlNode* node);
		void removeNode(ControlNode* node);
		void destroyNode(NodeID id);
		// Objects
		void addObject(Renderable* object);
		void removeObject(Renderable* object);
		void destroyEntity(EntityID id);
		void clear();
		void render(TileLayer* renderingLayer) const;

		const std::vector<Renderable*>& getObjects() const { return m_objects; }
		const std::vector<ControlNode*>& getNodes() const { return m_nodes; }
		const size_t getObjectCount() const { return m_objects.size(); }
		const size_t getNodeCount() const { return m_nodes.size(); }
		ControlNode* getNodeByIndex(NodeID index) const;
		ControlNode* getNodeById(NodeID id) const;

		bool saveScene(const std::string& filepath) const;
		bool saveSceneAs(const std::string& filepath) const;
		bool loadScene(const std::string& filepath, TextureManager tm);
		bool deleteSceneFile(const std::string& filepath);

		std::vector<std::string> listScenesDir(const std::string& dir);
		std::vector<EntityID> getAllEntities() const;

		void setSceneType(SceneType type);
		SceneType getSceneType() { return m_ActiveScene; }
		bool canSwitchScene() const;
		EntityID getNextId();
		NodeID getNextNodeId();
		bool has2DEntities() const;
		bool has3DEntities() const;

	private:
		SceneType m_ActiveScene;
		void writeString(std::ofstream& out, const std::string& str) const;
		std::string readString(std::ifstream& in) const;
		std::vector<Renderable*> m_objects;
		std::vector<ControlNode*> m_nodes;
	};
}