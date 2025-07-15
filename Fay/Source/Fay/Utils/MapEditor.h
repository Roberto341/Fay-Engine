#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <Core/Core.h>
#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_glfw.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"
#include "../ImGui/ImGuiFileDialog.h"
namespace Fay
{
	class TileLayer;
	class Sprite;
	class Texture;
	struct TileTextureInfo
	{
		int id;
		std::string name;
		std::string path;
		Texture* texture = nullptr;
	};
	struct TileColorInfo
	{
		int id;
		std::string name;
		Fay::Vec4 color;
	};
	struct Tile {
		int id;
		bool isTexture;
		// Constructor for simplness 
		Tile() : id(0), isTexture(false) {}
		Tile(int id_, bool isTexture_) : id(id_), isTexture(isTexture_) {}

		bool operator==(const Tile& other) const {
			return id == other.id && isTexture == other.isTexture;
		}

		bool operator!=(const Tile& other) const {
			return !(*this == other);
		}
	};
	class MapEditor
	{
	public:
		MapEditor(TileLayer* layer, int width, int height, float tileSize);
		void renderImGui();
		void render();
		void update();
		void handleInput();
		void clean();
		void setTile(int x, int y, const Tile& tile);
		Tile getTile(int x, int y) const;


		// files
		bool loadConfigFile(const std::string& filepath);
		bool saveConfigFile(const std::string& filepath);
		bool saveToFile(const std::string& filepath) const;
		bool loadFromFile(const std::string& filepath);
		
		Vec4 getColor(int tileId) const;
		Texture* getTexture(int tileId) const;
	private:
		int parseTileId(const std::string& token, bool& isTexture);
		void openSaveDialog();
		void openLoadDialog();
		void openConfigLoadDialog();
		void openConfigSaveDialog();
		bool m_showSaveDialog = false;
		bool m_showLoadDialog = false;
		bool m_showLoadConfig = false;
		bool m_showSaveConfig = false;
		int width, height;
		float tileSize;
		//std::vector<int> m_tiles;
		TileLayer* m_tileLayer;
		std::vector<Sprite*> m_spriteCache;
		Tile m_selectedTile;
		std::vector<TileTextureInfo> m_tileTexturePalette;
		std::vector<TileColorInfo> m_tileColorPalette;
		std::vector<Tile> m_tiles;
		// Add Tiles
		int newColorTileId = 0;
		char newColorTileName[64] = "";
		Fay::Vec4 newColor = Fay::Vec4(1.0f, 1.0f, 1.0f, 1.0f); // Default white

		int newTextureTileId = 0;
		char newTextureTileName[64] = "";
		char newTextureTilePath[256] = "";

	};
}