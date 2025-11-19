#pragma once
#include <Scripting/ScriptEngine.h>
#include <Renderer/Sprite.h>
#include <Renderer/3DRenderer/Cube.h>
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

		//Vec3 position = sprite->getPosition;
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
	};
	/*
	struct MeshComponent
	{
		Mesh* mesh;
		Material* material;
	}
	*/
	// this will be what carries the c# script
	struct ScriptComponent
	{
		std::string className;
		MonoObject* instance = nullptr;
		MonoMethod* onUpdateMethod = nullptr;
		MonoMethod* onStartMethod = nullptr;

		void Bind()
		{
			auto* klass = mono_class_from_name(ScriptEngine::GetImage(), "FayRuntime", className.c_str());
			if (!klass)
			{
				FAY_LOG_ERROR("[ScriptComponent] Class not found: " << className);
				return;
			}

			instance = mono_object_new(ScriptEngine::GetDomain(), klass);
			if (!instance)
			{
				FAY_LOG_ERROR("[ScriptComponent] Failed to create instance of: " << className);
				return;
			}
			mono_runtime_object_init(instance);

			onStartMethod = mono_class_get_method_from_name(klass, "OnStart", 0);
			onUpdateMethod = mono_class_get_method_from_name(klass, "OnUpdate", 0);
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