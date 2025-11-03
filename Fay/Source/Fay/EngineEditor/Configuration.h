#pragma once
#include <Entity/Tile.h>
namespace Fay
{
	class Configuration
	{
	public:
		void addTile(TileInfo tile);
		void removeTile(const TileInfo& tile);

		// Helper functions
		const std::vector<TileInfo>& getTiles() const { return m_tiles; }

		size_t getSize() const { return m_tiles.size(); }
		bool isEmpty() const { return m_tiles.empty(); }
		TileInfo& getBack() { return m_tiles.back(); }
		int getNextId() const;
		
		void clear();

		bool saveConfiguration(const std::string& filepath) const;
		bool loadConfiguration(const std::string& filepath);
		bool deleteConfiguration(const std::string& filepath);
		std::vector<TileInfo> m_tiles;
		std::vector<std::string> listConfigurationsDir(const std::string& dir);
	};
}