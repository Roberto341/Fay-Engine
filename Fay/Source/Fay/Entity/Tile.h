#pragma once
#include <fstream>
#include <filesystem>
#include <set>
#include <string>
#include <Core/Logger.h>
#include <Math/Math.h>           // for Vec4
#include <Graphics/TextureManager.h> 
namespace Fay
{
	class Texture;
	class Sprite;
	using EntityID = uint32_t;
	struct Tile
	{
		int id;
		bool isTexture;
		Tile() : id(0), isTexture(false) {}
		Tile(int id_, bool isTexture_) : id(id_), isTexture(isTexture_) {}
	};
	struct TileInfo
	{
		int id;
		std::string name;
		bool isTexture;
		std::string texturePath;
		Texture* texture = nullptr;
		Vec4 color;
		Vec3 size;
		Vec3 position;

		TileInfo() : id(0), name(""), isTexture(false), texturePath(""), texture(nullptr), color(0.0f, 0.0f, 0.0f, 0.0f), size(1, 1, 1), position(0, 0, 0){}
		TileInfo(int id, const std::string& name, const Vec4& color)
			: id(id), name(name), isTexture(false), texturePath(""), texture(nullptr), color(color) {}

		TileInfo(int id, const std::string& name, const std::string& texPath)
			: id(id), name(name), isTexture(true), texturePath(texPath), texture(nullptr), color(0.0f, 0.0f, 0.0f, 0.0f) {
		}
		void setSize(const Vec3& size)
		{
			this->size = size;
		}
		const Vec3& getSize() const
		{
			return this->size;
		}

	};
}