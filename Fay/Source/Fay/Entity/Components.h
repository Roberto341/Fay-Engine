#pragma once
#include <Scripting/ScriptEngine.h>
#include <Renderer/Sprite.h>
#include <Renderer/Cube.h>
#include <Entity/ComponentManager.h>
#include <functional>
#include <Core/Logger.h>
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

		SpriteComponent() = default;
		SpriteComponent(Renderable* s) : sprite(s) {}
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
		//Mat4 modelMatrix;
		CubeComponent() = default;
		CubeComponent(Renderable* c) : cube(c) {}
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
		uint32_t entityId = 0;
		std::vector<ScriptInstance> scripts;
		std::string tag;

		ScriptComponent() = default;
		ScriptComponent(uint32_t _entityId) : entityId(_entityId) {}
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
	using AllComponents = ComponentGroup<
		TransformComponent, 
		CameraComponent, 
		SpriteComponent, 
		CubeComponent, 
		ScriptComponent,
		CollisionComponent
	>;

}