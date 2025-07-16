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
		Vec4 color;
	};
	enum class SpawnType
	{
		None, // 0
		Player, // 1
		Npc, // 2
		Enemey // 3
	};
	struct TileSpawnInfo
	{
		int id;
		SpawnType type;
		Vec4 color;
	};
	struct Tile {
		int id;
		bool isTexture;
		// Constructor for simplness 
		Tile() : id(0), isTexture(false){}
		Tile(int id_, bool isTexture_) : id(id_), isTexture(isTexture_){}
	};
	struct SpawnTile
	{
		int id;
		SpawnType type;
		bool isSpawn;
		// Constructor for simplness 
		SpawnTile() : id(0), type(SpawnType::None), isSpawn(false) {}
		SpawnTile(int id_, SpawnType spawnType_, bool isSpawn_) : id(id_), type(spawnType_), isSpawn(isSpawn_) {}

	};
	enum class MapEditorLayer
	{
		Tile, // 0
		SpawnPoints, // 1
		Objects, // 2
		Collision // 3
	};
	struct SpawnPoint
	{
		int x, y;
		SpawnType type; // e.g. "Player, Enemey, Or Npc
		Vec4 color;
	};
	/*
	*
	* tile layer 1 will be backgroun
	* tile layer 2 will be spawns
	*/
	class MapEditor
	{
	public:
		MapEditor(TileLayer* layer,TileLayer* layer2, int width, int height, float tileSize);
		void renderImGui();
		void render();
		void update();
		void handleInput();
		void clean();
		void setTile(int x, int y, const Tile& tile);
		Tile getTile(int x, int y) const;
		void setSpawnTile(int x, int y, const SpawnTile& spawnTile);
		SpawnTile getSpawnTile(int x, int y) const;
		// files
		bool loadConfigFile(const std::string& filepath);
		bool saveConfigFile(const std::string& filepath);
		bool saveToFile(const std::string& filepath) const;
		bool loadFromFile(const std::string& filepath);
		
		Vec4 getColor(int tileId) const;
		Vec4 getSpawnColor(int tileId) const;

		Texture* getTexture(int tileId) const;

	private:
		int parseTileId(const std::string& token, bool& isTexture);
		int parseSpawnTileId(const std::string& token, SpawnType& type, bool& isSpawn);
		int getColorId(int current);
		int getTextureId(int current);
		void openSaveDialog();
		void openLoadDialog();
		void openConfigLoadDialog();
		void openConfigSaveDialog();
		void openTexture();
		bool m_showSaveDialog = false;
		bool m_showLoadDialog = false;
		bool m_showLoadConfig = false;
		bool m_showSaveConfig = false;
		bool m_showTextureDialog = false;
		bool newTextureAdded = false;
		int width, height;
		float tileSize;
		//std::vector<int> m_tiles;
		TileLayer* m_tileLayer;
		TileLayer* m_spawnLayer;
		std::vector<Sprite*> m_spriteCache;
		Tile m_selectedTile;
		SpawnTile m_selectedSpawnTile;
		std::vector<TileTextureInfo> m_tileTexturePalette;
		std::vector<TileColorInfo> m_tileColorPalette;
		std::vector<TileSpawnInfo> m_tileSpawnPalette;

		std::vector<Tile> m_tiles;
		std::vector<SpawnTile> m_spawnTiles;

		// Add Tiles
		int newColorTileId = 0;
		char newColorTileName[64] = "";
		Fay::Vec4 newColor = Fay::Vec4(1.0f, 1.0f, 1.0f, 1.0f); // Default white

		int newTextureTileId = 0;
		char newTextureTileName[64] = "";
		std::string newTextureTilePath;
		std::string relPath;

		// Spawn stuff
		MapEditorLayer m_activeLayer = MapEditorLayer::Tile;
		SpawnType m_currentSpawn;
		std::vector<SpawnPoint> m_spawnPoints;
		const Tile EMPTY_TILE = Tile(0, false);
		const SpawnTile EMPTY_SPAWN_TILE = SpawnTile(0, SpawnType::None, false);

		std::string spawnTypeToString(SpawnType type);
		SpawnType parseSpawnType(const std::string& str);
	};
}