#pragma once
#include <Scripting/ScriptEngine.h>
#include <Renderer/Sprite.h>
#include <Renderer/Cube.h>
#include <Renderer/MeshObject.h>
#include <Renderer/ControlNode.h>
#include <Entity/ComponentManager.h>
#include <functional>
#include <Core/Logger.h>
#include <unordered_set>
//#include <Math/Math.h>
// declare all components here
namespace Fay
{
	enum class SceneType
	{
		None,
		Scene2D,
		Scene3D
	};
	struct TransformComponent
	{
		Vec3 position = Vec3(0, 0, 0);
		Vec3 translation = Vec3(0, 0, 0);
		Vec3 rotation = Vec3(0, 0, 0);
		Vec3 scale = Vec3(1, 1, 1);

		TransformComponent() = default;
		TransformComponent(const Vec3& pos, const Vec3& rot, const Vec3& translation, const Vec3& scale) : position(pos),
			rotation(rot), translation(translation), scale(scale) {
		}
	};

	struct CollisionComponent
	{
		Vec3 pos;
		Vec3 size;
		
		CollisionComponent() = default;
		CollisionComponent(const Vec3& pos, const Vec3& size) : pos(pos), size(size) {}
	};
	template<typename... Components>
	struct ComponentGroup
	{

	};
	struct CameraComponent
	{

	};
	struct SpriteComponent
	{
		// Can take in CameraComponent, TransformComponent and ScriptComponent
		Renderable* sprite = nullptr;
		std::unordered_set<std::string> tags;

		SpriteComponent() = default;
		SpriteComponent(Renderable* s) : sprite(s) {}

		void addTag(std::string& tag)
		{
			tags.insert(tag);
		}

		bool hasTag(const std::string& tag) const
		{
			return tags.contains(tag);
		}
	
		std::string getAllTagsAsString() const
		{
			/* Prints out each and every single tag*/
			std::string result;

			for (const auto& tag : tags)
			{
				if (!result.empty())
					result += ",";
				result += tag;
			}

			return result;
		}
		const std::string* getTag(const std::string& tagName) const
		{
			auto it = tags.find(tagName);

			if (it == tags.end())
				return nullptr;

			return &(*it);
		}

		bool removeTagBool(const std::string& tag)
		{
			return tags.erase(tag) > 0;
		}

		void removeTag(const std::string& tag)
		{
			tags.erase(tag);
		}

		void setCollision(bool cond)
		{
			//sprite->setColision(cond);
			sprite->setCollision(cond);
		}
		void setColor(Vec4 color)
		{
			sprite->setColor(color);
		}
		void setTexture(Texture* texture)
		{
			sprite->setTexture(texture);
		}
		void setPosition(Vec3 position)
		{
			sprite->setPosition(position);
		}
		void setSize(Vec3 size)
		{
			sprite->setSize(size);
		}

		Texture* getTexture() const
		{
			return sprite->getTexture();
		}

		bool getCollision() const
		{
			return sprite->getCollision();
		}
		Vec3 getPosition() const
		{
			return sprite->getPosition();
		}
		Vec3 getSize() const {
			return sprite->getSize();
		}
		Vec4 getColor() const {
			return sprite->getColor();
		}
		uint32_t getId() const{
			return sprite->getId();
		}
	};

	struct CubeComponent
	{
		Renderable* cube = nullptr; 
		std::unordered_set<std::string> tags;

		//Mat4 modelMatrix;
		CubeComponent() = default;
		CubeComponent(Renderable* c) : cube(c) {}
		void addTag(std::string& tag)
		{
			tags.insert(tag);
		}

		bool hasTag(const std::string& tag) const
		{
			return tags.contains(tag);
		}
		std::string getAllTagsAsString() const
		{
			/* Prints out each and every single tag*/
			std::string result;

			for (const auto& tag : tags)
			{
				if (!result.empty())
					result += ",";
				result += tag;
			}

			return result;
		}
		const std::string* getTag(const std::string& tagName) const
		{
			auto it = tags.find(tagName);

			if (it == tags.end())
				return nullptr;

			return &(*it);
		}
		bool removeTagBool(const std::string& tag)
		{
			return tags.erase(tag) > 0;
		}

		void removeTag(const std::string& tag)
		{
			tags.erase(tag);
		}
		void setCollision(bool cond)
		{
			//sprite->setColision(cond);
			cube->setCollision(cond);
		}
		void setColor(Vec4 color)
		{
			cube->setColor(color);
		}
		void setPosition(Vec3 position)
		{
			cube->setPosition(position);
		}
		void setSize(Vec3 size)
		{
			cube->setSize(size);
		}
		bool getCollision() const
		{
			return cube->getCollision();
		}
		Vec3 getPosition() const
		{
			return cube->getPosition();
		}
		Vec3 getSize() const {
			return cube->getSize();
		}
		Vec4 getColor() const {
			return cube->getColor();
		}
		uint32_t getId() const {
			return cube->getId();
		}
	};
	// this will be what carries the c# script
	struct ScriptInstance
	{
		std::string className;
		bool hasStarted = false;

		ScriptInstance() = default;
		ScriptInstance(const std::string& name) : className(name) {}
	};
	struct ScriptComponent
	{
		// New method
		uint32_t id = 0;

		std::vector<ScriptInstance> scripts;
		std::string tag;

		ScriptComponent() = default;
		ScriptComponent(uint32_t _Id) : id(_Id) {}

		void setTag(const std::string& _tag)
		{
			tag = _tag;
		}
		std::string& getTag()
		{
			return tag;
		}
		void addScript(const std::string& className)
		{
			scripts.emplace_back(className);
		}
		void removeScript(size_t index)
		{
			if (index < scripts.size())
				scripts.erase(scripts.begin() + index);
		}
	};
	struct ControllerComponent
	{
		ControlNode* node = nullptr;
		ControllerComponent() = default;
		ControllerComponent(ControlNode* n) : node(n) {}

		std::vector<EntityID> entities;
		uint32_t getNodeId() 
		{
			return node->getId();
		}
		size_t getNodeChildrenCount()
		{
			return entities.size();
		}
		const std::vector<EntityID>& getEntities() const
		{
			return entities;
		}
		bool hasEntity(EntityID id) const
		{
			return std::find(entities.begin(), entities.end(), id) != entities.end();
		}
		void addEntity(EntityID id)
		{
			if(!hasEntity(id))
				entities.emplace_back(id);
		}
		void removeEntity(EntityID id)
		{
			auto it = std::find(entities.begin(), entities.end(), id);
			if (it != entities.end())
			{
				entities.erase(it);
			}
		}
	};
	struct MeshComponent
	{
		MeshObject* mesh = nullptr;

		MeshComponent() = default;

		MeshComponent(MeshObject* object) : mesh(object) {}

		MeshObject* getMesh() { return mesh; }
		const MeshObject* getMesh() const { return mesh; }
		
	};
	using AllComponents = ComponentGroup<
		TransformComponent, 
		CameraComponent, 
		SpriteComponent, 
		CubeComponent, 
		ScriptComponent,
		CollisionComponent,
		ControllerComponent,
		MeshComponent
	>;

}