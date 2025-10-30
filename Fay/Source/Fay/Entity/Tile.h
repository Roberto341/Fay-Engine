#pragma once

namespace Fay
{
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

		TileInfo() : id(0), name(""), isTexture(false), texturePath(""), texture(nullptr), color(0.0f, 0.0f, 0.0f, 0.0f) {}
		TileInfo(int id, const std::string& name, const Vec4& color)
			: id(id), name(name), isTexture(false), texturePath(""), texture(nullptr), color(color) {}

		TileInfo(int id, const std::string& name, const std::string& texPath)
			: id(id), name(name), isTexture(true), texturePath(texPath), texture(nullptr), color(0.0f, 0.0f, 0.0f, 0.0f) {}
	};
}