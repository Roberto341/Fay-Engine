#pragma once
#include <filesystem>
#include <set>
#include <Renderer/Sprite.h>
#include <Renderer/3DRenderer/Cube.h>
#include <Math/Math.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureManager.h>
#include <Core/Logger.h>
#include <Entity/Entity.h>
#include <Entity/ComponentManager.h>
#include <Entity/Components.h>
namespace Fay
{
	class Scene
	{
	public:
		void addObject(Renderable* object); // Entity is a single standalone entity that takes either a Sprite* or Cube* and has an id and can be multiple
		void removeObject(Renderable* object);
		void clear();

		const std::vector<Renderable*>& getObjects() const { return m_objects; }

		bool saveScene(const std::string& filepath) const;
		bool loadScene(const std::string& filepath, TextureManager tm);
		bool deleteSceneFile(const std::string& filepath);
		std::vector<std::string> listScenesDir(const std::string& dir);
		std::vector<EntityID> getAllEntities() const;

		void setSceneType(SceneType type);
		SceneType getSceneType() { return m_ActiveScene; }
		bool canSwitchScene() const;
	private:
		bool has2DEntities() const;
		bool has3DEntities() const;
		SceneType m_ActiveScene;
		void writeString(std::ofstream& out, const std::string& str) const;
		std::string readString(std::ifstream& in) const;
		std::vector<Renderable*> m_objects;
	};
}