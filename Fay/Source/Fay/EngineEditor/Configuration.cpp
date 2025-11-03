#include <EngineEditor/Configuration.h>
namespace Fay
{
	void Configuration::addTile(TileInfo tile)
	{
		m_tiles.push_back(tile);
	}
	int Configuration::getNextId() const
	{
		if (m_tiles.empty())
			return 1;

		int maxId = 1;
		std::set<int> usedIds;
		for (const auto& tile : m_tiles)
		{
			usedIds.insert(tile.id);
			if (tile.id > maxId)
				maxId = tile.id;
		}

		// Look for the first gap
		for (int i = 1; i <= maxId; ++i)
		{
			if (usedIds.find(i) == usedIds.end())
				return i; // reuse deleted ID
		}

		return maxId + 1; // no gaps, assign next ID
	}
	void Configuration::clear()
	{
		m_tiles.clear();
	}
	void Configuration::removeTile(const TileInfo& tile)
	{
		auto it = std::find_if(m_tiles.begin(), m_tiles.end(),
			[&](const TileInfo& t) {
				return t.id == tile.id; // Compare by unique ID
			});

		if (it != m_tiles.end())
		{
			m_tiles.erase(it);
			FAY_LOG_INFO("Removed tile: " << tile.name << " (ID: " << tile.id << ")");
		}
		else
		{
			FAY_LOG_WARN("Tile not found: " << tile.name << " (ID: " << tile.id << ")");
		}
	}
	bool Configuration::saveConfiguration(const std::string& filepath) const
	{
		std::ofstream file(filepath);
		if (!file.is_open()) return false;

		file << "[Tile Palette]\n{\n";
		for (const auto& tile : m_tiles)
		{
			file << "    [Tile]\n    " << tile.id << ", " << tile.name << ", ";
			if (tile.isTexture)
				file << tile.texturePath << "\n";
			else
				file << "vec4(" << tile.color.x << "f, " << tile.color.y << "f, "
				<< tile.color.z << "f, " << tile.color.w << "f)\n";
		}
		file << "}\n";
		return true;
	}
	bool Configuration::loadConfiguration(const std::string& filepath)
	{
		// Move to binary eventually
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			FAY_LOG_ERROR("Failed to open tile palette");
			return false;
		}

		clear();

		auto trim = [](std::string& s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
			};
		std::string line;
		while (std::getline(file, line))
		{
			trim(line);
			if (line.empty() || line[0] == '#') continue;

			if (line.rfind("[Tile]", 0) == 0)
			{
				// Read next line with tile data
				if (!std::getline(file, line)) break;
				trim(line);
				if (line.empty()) continue;

				std::stringstream ss(line);
				std::string idStr, name, value;

				if (!std::getline(ss, idStr, ',')) continue;
				if (!std::getline(ss, name, ',')) continue;
				if (!std::getline(ss, value)) value = ""; // read the rest of the line as value

				trim(idStr); trim(name); trim(value);

				int id = std::stoi(idStr);

				if (value.find("vec4(") == 0)
				{
					std::string inside = value.substr(5, value.size() - 6); // remove "vec4(" and ")"
					inside.erase(std::remove(inside.begin(), inside.end(), 'f'), inside.end()); // remove 'f'

					std::stringstream ssVec(inside);
					std::string component;
					float r = 0, g = 0, b = 0, a = 1;

					if (std::getline(ssVec, component, ',')) { trim(component); r = std::stof(component); }
					if (std::getline(ssVec, component, ',')) { trim(component); g = std::stof(component); }
					if (std::getline(ssVec, component, ',')) { trim(component); b = std::stof(component); }
					if (std::getline(ssVec, component, ',')) { trim(component); a = std::stof(component); }

					TileInfo tile = TileInfo(id, name, Vec4(r, g, b, a));
					m_tiles.emplace_back(tile);
				}
				else
				{
					// treat as texture path
					TileInfo tile = TileInfo(id, name, value);
					m_tiles.emplace_back(tile);
					TextureManager::add(new Texture(tile.name, tile.texturePath));
				}
			}
		}
		for (auto& tile : m_tiles)
		{
			if (tile.isTexture)
				tile.texture = TextureManager::getTexture(tile.name);
		}
		return true;
	}
	bool Configuration::deleteConfiguration(const std::string& filepath)
	{
		try {
			return std::filesystem::remove(filepath);
		}
		catch (const std::filesystem::filesystem_error& e) {
			FAY_LOG_ERROR("Error deleting file: " << e.what());
			return false;
		}
	}
	std::vector<std::string> Configuration::listConfigurationsDir(const std::string& dir)
	{
		std::vector<std::string> files;
		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (entry.path().extension() == ".config")
				files.push_back(entry.path().filename().string());
		}
		return files;
	}
}