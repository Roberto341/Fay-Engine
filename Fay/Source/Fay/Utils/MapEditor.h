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
	// Normal tile
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
	struct Tile {
		int id;
		bool isTexture;
		// Constructor for simplness 
		Tile() : id(0), isTexture(false) {}
		Tile(int id_, bool isTexture_) : id(id_), isTexture(isTexture_) {}
	};
	// Collision Tile
	enum class CollisionType
	{
		None, // 0 == 0
		Person, // 1 == c1
		Tile // 2 == t2
	};
	struct TileCollisionInfo
	{
		int id;
		CollisionType type;
		Vec4 color;
	};
	struct CollisionTile
	{
		int id;
		bool isCollision;
		CollisionType type;
		CollisionTile() : id(0), isCollision(false), type(CollisionType::None) {}
		CollisionTile(int id_, bool isCollision_, CollisionType type_) : id(id_), isCollision(isCollision_), type(type_) {}
	};
	// Object tile
	struct TileObjectInfo
	{
		int id;
		std::string name;
		std::string path;
		Texture* texture = nullptr;
	};
	struct ObjectTile
	{
		int id;
		bool isTexture;
		ObjectTile() : id(0), isTexture(false) {}
		ObjectTile(int id_, bool isTexture_) : id(id_), isTexture(isTexture_) {}
	};
	// Spawn tile
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
		//WorldBounds or World //4
	};
	// Dialog 
	enum class DialogType {
		Save,
		Load,
		LoadConfig,
		SaveConfig,
		Texture
	};
	struct DialogState
	{
		bool showSaveDialog = false;
		bool showLoadDialog = false;
		bool showLoadConfig = false;
		bool showSaveConfig = false;
		bool showTextureDialog = false;
		bool newTextureAdded = false;

		void resetAll()
		{
			showSaveDialog = false;
			showLoadDialog = false;
			showLoadConfig = false;
			showSaveConfig = false;
			showTextureDialog = false;
			newTextureAdded = false;
		}
	};
	/*
	*
	* tile layer 1 will be backgroun
	* tile layer 2 will be spawns
	* tile layer 3 will be objects
	* tile layer 4 will be collision boxes
	*/
	class MapEditor
	{
	public:
		//MapEditor(TileLayer* layer,TileLayer* layer2, int width, int height, float tileSize); // Just gonna comment this out in case
		// if anything blows up :)
		MapEditor(const std::vector<TileLayer*> layers, int width, int height, float tileSize);
		void renderImGui();
		void render();
		void update();
		void handleInput();


		// Normal
		void setTile(int x, int y, const Tile& tile);
		Tile getTile(int x, int y) const;

		// Spawn
		void setSpawnTile(int x, int y, const SpawnTile& spawnTile);
		SpawnTile getSpawnTile(int x, int y) const;

		// Object
		void setObjectTile(int x, int y, const ObjectTile& objectTile);
		ObjectTile getObjectTile(int x, int y) const;

		// Collision
		void setColTile(int x, int y, const CollisionTile& colTile);
		CollisionTile getColTile(int x, int y) const;
		// files
		bool loadConfigFile(const std::string& filepath);
		bool saveConfigFile(const std::string& filepath);
		bool saveToFile(const std::string& filepath) const;
		bool loadFromFile(const std::string& filepath);
		
		Vec4 getColor(int tileId) const;
		Vec4 getSpawnColor(int tileId) const;
		Vec4 getColColor(int tileId) const;

		Texture* getTexture(int tileId) const;
		Texture* getObjectTexture(int tileId) const;
	private:
		int parseTileId(const std::string& token, bool& isTexture);
		int parseSpawnTileId(const std::string& token, SpawnType& type, bool& isSpawn);
		int parseObjectTileId(const std::string& token, bool& isTexture);
		int parseCollisionTileId(const std::string& token, CollisionType& type, bool& isCollision);

		int getColorId(int current);
		int getTextureId(int current);
		
		// File dialog 
		void openDialog(DialogState& dialog, DialogType type);

		DialogState dialog;

		int width, height;
		float tileSize;

		std::vector<TileLayer*> m_layers;

		Tile m_selectedTile;
		SpawnTile m_selectedSpawnTile;
		CollisionTile m_selectedColTile;
		ObjectTile m_selectedObjectTile;

		std::vector<TileTextureInfo> m_tileTexturePalette;
		std::vector<TileColorInfo> m_tileColorPalette;
		std::vector<TileSpawnInfo> m_tileSpawnPalette;
		std::vector<TileCollisionInfo> m_tileCollisionPalette;
		std::vector<TileObjectInfo> m_tileObjectsPalette;

		std::vector<Tile> m_tiles;
		std::vector<SpawnTile> m_spawnTiles;
		std::vector<CollisionTile> m_collisionTiles;
		std::vector<ObjectTile> m_objectTiles;
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
		CollisionType m_currentCollision;

		const Tile EMPTY_TILE = Tile(0, false);
		const SpawnTile EMPTY_SPAWN_TILE = SpawnTile(0, SpawnType::None, false);
		const CollisionTile EMPTY_COLLISION_TILE = CollisionTile(0, false, CollisionType::None);
		const ObjectTile EMPTY_OBJECT_TILE = ObjectTile(0, false);

		// Spawn parser
		std::string spawnTypeToString(SpawnType type);
		SpawnType parseSpawnType(const std::string& str);

		// Collision parser
		std::string collisionTypeToString(CollisionType type);
		CollisionType parseCollisionType(const std::string& str);
	};
}