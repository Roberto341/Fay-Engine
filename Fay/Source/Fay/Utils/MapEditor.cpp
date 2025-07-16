#include <Utils/MapEditor.h>

namespace Fay
{
	MapEditor::MapEditor(TileLayer* layer, TileLayer* layer2, int width, int height, float tileSize)
		: width(width), height(height), tileSize(tileSize), m_tileLayer(layer), m_spawnLayer(layer2)
	{
		m_tiles.resize(width * height, Tile(0, false));
		m_spawnTiles.resize(width * height, SpawnTile(0, SpawnType::None, false));
		// load default config file
		loadConfigFile("Res/MapEditor/MapEditor.config"); // Change to default.config
		m_selectedTile = Tile(0, false);

		// Get latest id in the list
		getColorId(m_tileColorPalette.back().id);
		// Get latest id for texture tile
		getTextureId(m_tileTexturePalette.back().id);
	}

	void MapEditor::renderImGui()
	{
		ImGui::Begin("Map Editor");
		// Layer switcher
		const char* layerNames[] = { "Tile Editor", "Spawns", "Objects", "Collision" };
		static int currentLayer = 0; // Tile editor

		ImGui::Combo("Active Layer", &currentLayer, layerNames, IM_ARRAYSIZE(layerNames));
		m_activeLayer = static_cast<MapEditorLayer>(currentLayer);

		// Tile selection UI
		float thumbnailSize = 48.0f;
		float padding = 4.0f;
		int tilesPerRow = 4;
		int i = 0;
		ImGui::Text("Tile Texture Pallet");
		//int i = 0;
		for (const auto& tile : m_tileTexturePalette)
		{
			ImGui::PushID(tile.id);

			if (tile.texture && tile.texture->getId() != 0)
			{
				ImGui::Image((void*)(intptr_t)tile.texture->getId(), ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 1), ImVec2(1, 0));
			}
			else
			{
				ImGui::Dummy(ImVec2(thumbnailSize, thumbnailSize));
				ImGui::GetWindowDrawList()->AddRectFilled(
					ImGui::GetItemRectMin(),
					ImGui::GetItemRectMax(),
					IM_COL32(50, 50, 50, 255));
			}

			if (ImGui::IsItemClicked())
				m_selectedTile = Tile(tile.id, true);

			if (m_selectedTile.id == tile.id && m_selectedTile.isTexture)
			{
				ImVec2 min = ImGui::GetItemRectMin();
				ImVec2 max = ImGui::GetItemRectMax();
				ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(255, 255, 0, 255), 0.0f, 0, 3.0f);
			}

			ImGui::PopID();

			// Only call SameLine if not the last tile, so all are on the same line
			if (i < m_tileTexturePalette.size() - 1)
				ImGui::SameLine(0, padding);

			++i;
		}
		ImGui::NewLine();

		ImGui::Text("Tile Color Palette");

		for (size_t i = 0; i < m_tileColorPalette.size(); ++i)
		{
			const auto& tile = m_tileColorPalette[i];
			ImGui::PushID(static_cast<int>(tile.id));

			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 size = ImVec2(thumbnailSize, thumbnailSize);

			ImGui::BeginGroup();
			ImU32 col = IM_COL32(
				(int)(tile.color.x * 255),
				(int)(tile.color.y * 255),
				(int)(tile.color.z * 255),
				(int)(tile.color.w * 255)
			);

			ImGui::InvisibleButton("paletteBtn", size);

			ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), col);

			if (m_selectedTile.id == tile.id && !m_selectedTile.isTexture)
			{
				ImGui::GetWindowDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y),
					IM_COL32(255, 255, 0, 255), 0.0f, 0, 3.0f);
			}

			if (ImGui::IsItemClicked())
			{
					m_selectedTile = Tile(tile.id, false);
			}

			// Draw tile name below the square
			ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + size.y + 2));
			ImGui::TextUnformatted(tile.name.c_str());

			ImGui::EndGroup();
			ImGui::PopID();

			if (i < m_tileColorPalette.size() - 1)
				ImGui::SameLine(0, padding);  // Keep on same line with padding
		}
		ImGui::NewLine();

		ImGui::Text("Tile Spawn Palette");

		for (size_t i = 0; i < m_tileSpawnPalette.size(); ++i)
		{
			const auto& tile = m_tileSpawnPalette[i];
			ImGui::PushID(static_cast<int>(tile.id));

			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 size = ImVec2(thumbnailSize, thumbnailSize);

			ImGui::BeginGroup();
			ImU32 col = IM_COL32(
				(int)(tile.color.x * 255),
				(int)(tile.color.y * 255),
				(int)(tile.color.z * 255),
				(int)(tile.color.w * 255)
			);

			ImGui::InvisibleButton("spawnPaletteBtn", size);

			ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), col);

			if (m_selectedSpawnTile.id == tile.id)
			{
				ImGui::GetWindowDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y),
					IM_COL32(255, 255, 0, 255), 0.0f, 0, 3.0f);
			}

			if (ImGui::IsItemClicked())
			{
				m_currentSpawn = tile.type;
				m_selectedSpawnTile = SpawnTile(tile.id, m_currentSpawn, true);
			}

			// Draw tile name below the square
			ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + size.y + 2));
			ImGui::TextUnformatted(spawnTypeToString(tile.type).c_str());

			ImGui::EndGroup();
			ImGui::PopID();

			if (i < m_tileSpawnPalette.size() - 1)
				ImGui::SameLine(0, padding);  // Keep on same line with padding
		}
		ImGui::NewLine();
		// Spawn palette
		// Add texture tiles or color tiles directly
		ImGui::Separator();

		ImGui::Text("Add New Color Tile");

		ImGui::InputInt("Color Tile Id", &newColorTileId);
		ImGui::InputText("Color Tile name", newColorTileName, IM_ARRAYSIZE(newColorTileName));
		ImGui::ColorEdit4("Tile color", (float*)&newColor);

		if (ImGui::Button("Add Color Tile"))
		{
			m_tileColorPalette.push_back({ newColorTileId, std::string(newColorTileName), newColor });
			getColorId(m_tileColorPalette.back().id);
		}

		// Add texture tile
	
		ImGui::Text("Add New Texture Tile");

		ImGui::InputInt("Texture Tile Id", &newTextureTileId);
		ImGui::InputText("Texture Tile name", newTextureTileName, IM_ARRAYSIZE(newTextureTileName));
		//ImGui::ColorEdit4("Tile color", (float*)&newColor);

		if (ImGui::Button("Select Texture"))
		{
			openTexture();
		}
		if (m_showTextureDialog)
		{
			if (ImGuiFileDialog::Instance()->Display("ChooseTex"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					newTextureAdded = false;

					newTextureTilePath = ImGuiFileDialog::Instance()->GetFilePathName();

					std::replace(newTextureTilePath.begin(), newTextureTilePath.end(), '\\', '/');

					size_t pos = newTextureTilePath.find("Res/");

					relPath = (pos != std::string::npos) ? newTextureTilePath.substr(pos) : newTextureTilePath;
					
					std::string texName = std::string(newTextureTileName);
					Texture* newTexture = TextureManager::add(new Texture(newTextureTileName, relPath));
					//ImGui::Text("Texture Preview:");
					//ImGui::Image((void*)(intptr_t)newTexture->getId(), ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
				}
				ImGuiFileDialog::Instance()->Close();
			}
		}
		// Show preview if available
		//auto newTexture = std::make_shared<Texture>(newTextureTileName, relPath
		Texture* newTex = TextureManager::getTexture(newTextureTileName);
		if (!newTextureAdded && newTex && newTex->getId() != 0)
		{
			ImGui::Text("Texture Preview:");
			ImGui::Image((void*)(intptr_t)newTex->getId(), ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
		}
		// Add button to push tile

		if (ImGui::Button("Add Texture Tile"))
		{
			if (newTex && newTex->getId() != 0)
			{
				auto exists = std::any_of(m_tileTexturePalette.begin(), m_tileTexturePalette.end(),
					[&](const TileTextureInfo& t) { return t.id == newTextureTileId; });
				if (!exists)
				{
					m_tileTexturePalette.push_back({ newTextureTileId, std::string(newTextureTileName), relPath, newTex });
					getTextureId(m_tileTexturePalette.back().id);
					newTextureAdded = true;
					//saveConfigFile(configFile);
				}
				else {
					std::cout << "Texture ID already exists in the palette. \n";
				}
			}
			else
			{
				std::cout << "Failed to add texture. Id is 0 or texture not created.\n";
			}
		}
		// Canvas setup
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();// Top-left corner of canvas
		//ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Size of canvas

		// Fix canvas by Robert
		float canWidth = width * tileSize;
		float canHeight = height * tileSize;
		ImVec2 canvas_size = ImVec2(canWidth, canHeight);
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y);

		// Reserve canvas space and enable mouse interaction
		ImGui::InvisibleButton("canvas", canvas_size, ImGuiButtonFlags_MouseButtonLeft);
		bool is_hovered = ImGui::IsItemHovered();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// Fill background
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(30, 30, 30, 255));

		// Draw grid lines
		for (int x = 0; x <= width; ++x)
		{
			float x_pos = canvas_p0.x + x * tileSize;
			draw_list->AddLine(ImVec2(x_pos, canvas_p0.y), ImVec2(x_pos, canvas_p0.y + height * tileSize), IM_COL32(100, 100, 100, 255));
		}
		for (int y = 0; y <= height; ++y)
		{
			float y_pos = canvas_p0.y + y * tileSize;
			draw_list->AddLine(ImVec2(canvas_p0.x, y_pos), ImVec2(canvas_p0.x + width * tileSize, y_pos), IM_COL32(100, 100, 100, 255));
		}

		// Draw existing tiles (optional, depends on your tile data structure)
		// for example:
		if (m_activeLayer == MapEditorLayer::Tile)
		{
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					Tile tileID = getTile(x, y);
					if (tileID.id == 0)
						continue;

					ImVec2 min = ImVec2(canvas_p0.x + x * tileSize, canvas_p0.y + (height - 1 - y) * tileSize);
					ImVec2 max = ImVec2(min.x + tileSize, min.y + tileSize);

					if (tileID.isTexture)
					{
						auto texIt = std::find_if(m_tileTexturePalette.begin(), m_tileTexturePalette.end(),
							[tileID](const auto& tile) { return tile.id == tileID.id; });

						if (texIt != m_tileTexturePalette.end() && texIt->texture)
						{
							draw_list->AddImage(
								(void*)(intptr_t)texIt->texture->getId(),
								min, max,
								ImVec2(0, 1), ImVec2(1, 0));
						}
						else
						{
							draw_list->AddRectFilled(min, max, IM_COL32(255, 0, 0, 255)); // fallback red
						}
					}
					else
					{
						auto colorIt = std::find_if(m_tileColorPalette.begin(), m_tileColorPalette.end(),
							[tileID](const auto& tile) { return tile.id == tileID.id; });

						if (colorIt != m_tileColorPalette.end())
						{
							const auto& color = colorIt->color;
							ImU32 col = IM_COL32(
								(int)(color.x * 255),
								(int)(color.y * 255),
								(int)(color.z * 255),
								(int)(color.w * 255));
							draw_list->AddRectFilled(min, max, col);
						}
						else
						{
							draw_list->AddRectFilled(min, max, IM_COL32(255, 0, 255, 255)); // fallback magenta
						}
					}

					draw_list->AddText(min, IM_COL32(0, 0, 0, 255), std::to_string(tileID.id).c_str());
				}
			}
		}
		else if (m_activeLayer == MapEditorLayer::SpawnPoints)
		{
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					const SpawnTile& spawnTile = getSpawnTile(x, y);
					if (!spawnTile.isSpawn || spawnTile.id == 0)
						continue;

					ImVec2 min = ImVec2(canvas_p0.x + x * tileSize, canvas_p0.y + (height - 1 - y) * tileSize);
					ImVec2 max = ImVec2(min.x + tileSize, min.y + tileSize);

					auto spawnIt = std::find_if(m_tileSpawnPalette.begin(), m_tileSpawnPalette.end(),
						[&](const auto& tile) { return tile.id == spawnTile.id; });

					if (spawnIt != m_tileSpawnPalette.end())
					{
						ImU32 color = IM_COL32(
							(int)(spawnIt->color.x * 255),
							(int)(spawnIt->color.y * 255),
							(int)(spawnIt->color.z * 255),
							(int)(spawnIt->color.w * 255));
						draw_list->AddRectFilled(min, max, color);
						draw_list->AddRect(min, max, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);
					}
				}
			}
		}
		// MapEditorLayer

		// Handle painting tiles on click
		ImVec2 mouse_pos = ImGui::GetMousePos(); // screen coords
		//ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
		//ImVec2 local_pos = ImVec2(mouse_pos.x - canvas_pos.x, mouse_pos.y - canvas_pos.y);
		int tileX = (int)((mouse_pos.x - canvas_p0.x) / tileSize);
		int tileY = height - 1 - (int)((mouse_pos.y - canvas_p0.y) / tileSize);
			if (tileX >= 0 && tileX < width && tileY >= 0 && tileY < height)
			{
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				{
					
					switch (m_activeLayer)
					{
					case MapEditorLayer::Tile:
							setTile(tileX, tileY, m_selectedTile);
						break;
					case MapEditorLayer::SpawnPoints:
						setSpawnTile(tileX, tileY, m_selectedSpawnTile);
					break;
					// so on and so fifth
					// case MapEditorLayer::Objects
					// case MapEditorLayer::Collision;
					}
					
				}
				else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
				{
					// Switch m_activeLayer | MapEditorLayer
					switch (m_activeLayer)
					{
					case MapEditorLayer::Tile:
						setTile(tileX, tileY, EMPTY_TILE);
						break;
					case MapEditorLayer::SpawnPoints:
					{
						setSpawnTile(tileX, tileY, EMPTY_SPAWN_TILE);
						break;
					}
						// so on and so fifth
						// case MapEditorLayer::Objects
						// case MapEditorLayer::Collision;
					}
				}
				//setTile(tileX, tileY, m_selectedTile);
			}

		if (ImGui::Button("Save Map"))
		{
			openSaveDialog();
		}

		if (m_showSaveDialog)
		{
			if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
					saveToFile(filePathName.c_str());
				}
				ImGuiFileDialog::Instance()->Close();
				m_showSaveDialog = false;
			}
		}
		if (ImGui::Button("Load Map"))
		{
			openLoadDialog();
		}

		if (m_showLoadDialog)
		{
			if (ImGuiFileDialog::Instance()->Display("LoadFileDlgKey"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
					loadFromFile(filePath.c_str());
				}
				ImGuiFileDialog::Instance()->Close();
				m_showLoadDialog = false;
			}
		}

		if (ImGui::Button("Load Config"))
		{
			openConfigLoadDialog();
		}

		if (m_showLoadConfig)
			if (ImGuiFileDialog::Instance()->Display("LoadConfigDlgKey"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
					loadConfigFile(filePath.c_str());
				}
				ImGuiFileDialog::Instance()->Close();
				m_showLoadConfig = false;
			}

		if (ImGui::Button("Save Config"))
		{
			openConfigSaveDialog();
		}

		if (m_showSaveConfig)
		{
			if (ImGuiFileDialog::Instance()->Display("SaveConfigDlgKey"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
					saveConfigFile(filePath.c_str());
				}
				ImGuiFileDialog::Instance()->Close();
				m_showSaveConfig = false;
			}
		}
		ImGui::End();

		//std::cout << spawnTypeToString(m_currentSpawn) << std::endl;
	}

	void MapEditor::update()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureMouse && io.MouseDown[0])
		{
			float mx = io.MousePos.x;
			float my = io.MousePos.y;
			int gridX = (int)(mx / 32.0f);
			int gridY = (int)(my / 32.0f);
			if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height)
			{
				m_tiles[gridY * width + gridX] = m_selectedTile;
			}
		}
	}

	void MapEditor::clean()
	{
		for (auto s : m_spriteCache)
			delete s;
		m_spriteCache.clear();
	}

	void MapEditor::render()
	{
		/*
		m_tileLayer->clear();
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				Tile tileId = getTile(x, y);

				Vec4 color = Vec4(1, 0, 1, 1); // default magenta
				Texture* tex = nullptr;
				if (m_activeLayer == MapEditorLayer::Tile)
				{
					if (tileId.isTexture)
					{
						tex = getTexture(tileId.id);
					}
					else
					{
						// Look up color in color tile palette
						auto it = std::find_if(m_tileColorPalette.begin(), m_tileColorPalette.end(),
							[&](const TileColorInfo& t) { return t.id == tileId.id; });
						if (it != m_tileColorPalette.end())
							color = it->color;
					}
				}
				else if (m_activeLayer == MapEditorLayer::SpawnPoints)
				{
					if (tileId.isSpawnPoint)
					{
						// Look up color in spawn tile palette
						auto it = std::find_if(m_tileSpawnPalette.begin(), m_tileSpawnPalette.end(),
							[&](const TileSpawnInfo& t) { return t.id == tileId.id; });
						if (it != m_tileSpawnPalette.end())
							color = it->color;
					}
					else continue;
				}
				// Render either texture or color
				Sprite* sprite = nullptr;
				if (tileId.isTexture && tex)
				{
					sprite = new Sprite((float)x, (float)y, 1.0f, 1.0f, tex);
				}
				else
				{
					sprite = new Sprite((float)x, (float)y, 1.0f, 1.0f, color);
				}

				m_tileLayer->add(sprite);
				m_spriteCache.push_back(sprite);
			}
		}
		*/
		m_tileLayer->clear();
		m_spawnLayer->clear();
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (m_activeLayer == MapEditorLayer::Tile)
				{
						Tile tileId = getTile(x, y);
						Vec4 color = getColor(tileId.id);
						Texture* tex = getTexture(tileId.id);
						if (tileId.isTexture)
						{
							auto sprite = new Sprite((float)x, (float)y, 1.0f, 1.0f, tex);
							m_tileLayer->add(sprite);
							m_spriteCache.push_back(sprite);
						}
						else
						{
							auto sprite = new Sprite((float)x, (float)y, 1.0f, 1.0f, color);
							m_tileLayer->add(sprite);
							m_spriteCache.push_back(sprite);
						}
				}
				else if (m_activeLayer == MapEditorLayer::SpawnPoints)
				{
					SpawnTile tileId = getSpawnTile(x, y);
					Vec4 color = getSpawnColor(tileId.id);
					auto sprite = new Sprite((float)x, (float)y, 1.0f, 1.0f, color);
					m_spawnLayer->add(sprite);
					m_spriteCache.push_back(sprite);
				}
			}
		}
	}

	void MapEditor::handleInput()
	{
		if (!ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			ImVec2 mouse = ImGui::GetMousePos();
			int x = static_cast<int>(mouse.x / tileSize);
			int y = static_cast<int>(mouse.y / tileSize);
			if (x >= 0 && x < width && y >= 0 && y < height)
			{
				setTile(x, y, m_selectedTile);
				setSpawnTile(x, y, m_selectedSpawnTile);
			}
		}
	}

	void MapEditor::setTile(int x, int y, const Tile& tile)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			m_tiles[y * width + x] = tile;
	}
	void MapEditor::setSpawnTile(int x, int y, const SpawnTile& spawnTile)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			m_spawnTiles[y * width + x] = spawnTile;
	}
	Tile MapEditor::getTile(int x, int y) const
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return Tile(-1, false);
		return m_tiles[y * width + x];
	}

	SpawnTile MapEditor::getSpawnTile(int x, int y) const
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return SpawnTile(-1, SpawnType::None, true);
		return m_spawnTiles[y * width + x];
	}

	bool MapEditor::saveToFile(const std::string& filepath) const
	{
		std::ofstream file(filepath);
		if (!file.is_open())
			return false;

		file << "width=" << width << "\n";
		file << "height=" << height << "\n";
		file << "map=\n";

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				Tile tile = getTile(x, y);
				if (tile.isTexture)
					file << "t" << tile.id << " ";
				else
					file << tile.id << " ";
			}
			file << "\n";
		}
		file << "spawn_points=\n";
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				SpawnTile tile = getSpawnTile(x, y);
				if (tile.isSpawn)
				{
					switch (tile.type)
					{
					case SpawnType::Player:
						file << "sp" << tile.id << " ";
						break;
					case SpawnType::Npc:
						file << "np" << tile.id << " ";
						break;
					case SpawnType::Enemey:
						file << "ep" << tile.id << " ";
						break;
					}
				}
				else
				{
					file << "0 "; // Or some default value indicating no spawn
				}
			}
			file << "\n"; // Correct: new line after each row
		}
		return true;
	}

	bool MapEditor::loadFromFile(const std::string& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open()) return false;

		std::string line;
		bool readingMap = false;
		bool readingSpawns = false;
		int rowIndex = 0;

		m_tiles.clear();
		m_spawnPoints.clear();
		
		auto trim = [](std::string& s) {
			s.erase(0, s.find_first_not_of(" \t\r\n"));
			s.erase(s.find_last_not_of(" \t\r\n") + 1);
			};

		while (std::getline(file, line)) 
		{
			trim(line);
			if (line.empty()) continue;

			if (line.find("width=") == 0) {
				width = std::stoi(line.substr(6));
				continue;
			}
			if (line.find("height=") == 0) {
				height = std::stoi(line.substr(7));
				continue;
			}
			if (line == "map=") {
				readingMap = true;
				readingSpawns = false;
				m_tiles.resize(width * height, Tile(-1, false));
				rowIndex = 0;
				continue;
			}
			if (line == "spawn_points=") {
				readingMap = false;
				readingSpawns = true;
				rowIndex = 0;
				continue;
			}

			if (readingMap && rowIndex < height) {
				std::istringstream ss(line);
				for (int x = 0; x < width; ++x) {
					std::string token;
					if (!(ss >> token)) break;

					bool isSpawn = false;
					SpawnType type = SpawnType::None;

					bool isTexture = false;
					int id = parseTileId(token, isTexture);


					setTile(x, rowIndex, Tile(id, isTexture));
				}
				rowIndex++;
			}
			else if (readingSpawns && rowIndex < height) {
				std::istringstream ss(line);
				for (int x = 0; x < width; ++x) {
					std::string token;
					if (!(ss >> token)) break;

					if (token == "0") continue;

					bool isSpawn = false;
					SpawnType type = SpawnType::None;
					int id = parseSpawnTileId(token, type, isSpawn);

					if (isSpawn) {
						setSpawnTile(x, rowIndex, SpawnTile(id, type, true));

						// Get color from palette
						Vec4 color = Vec4(1, 0, 1, 1); // fallback magenta
						auto it = std::find_if(m_tileSpawnPalette.begin(), m_tileSpawnPalette.end(),
							[id](const auto& t) { return t.id == id; });
						if (it != m_tileSpawnPalette.end()) {
							color = it->color;
						}
						m_spawnPoints.push_back({ x, rowIndex, type, color });
					}
				}
				rowIndex++;
			}
		}

		return true;
	}

	int MapEditor::parseTileId(const std::string& token, bool& isTexture)
	{
		isTexture = false;
		if (token.empty()) return -1;
		if (token[0] == 't' || token[0] == 'T') {
			isTexture = true;
			try { return std::stoi(token.substr(1)); }
			catch (...) { return -1; }
		}
		else {
			try { return std::stoi(token); }
			catch (...) { return -1; }
		}
	}

	int MapEditor::parseSpawnTileId(const std::string& token, SpawnType& type, bool& isSpawn)
	{
		isSpawn = false;
		type = SpawnType::None;

		if (token.empty())
			return -1;

		std::string lowerToken = token;
		std::transform(lowerToken.begin(), lowerToken.end(), lowerToken.begin(), ::tolower);

		if (lowerToken.rfind("sp", 0) == 0) // starts with sp
		{
			isSpawn = true;
			type = SpawnType::Player;
			try { return std::stoi(lowerToken.substr(2)); }
			catch (...) { return -1; }
		}
		else if (lowerToken.rfind("np", 0) == 0) // starts with sp
		{
			isSpawn = true;
			type = SpawnType::Npc;
			try { return std::stoi(lowerToken.substr(2)); }
			catch (...) { return -1; }
		}
		else if (lowerToken.rfind("ep", 0) == 0) // starts with sp
		{
			isSpawn = true;
			type = SpawnType::Enemey;
			try { return std::stoi(lowerToken.substr(2)); }
			catch (...) { return -1; }
		}
		// not a spawn tile 
		return -1;
	}

	int MapEditor::getColorId(int current)
	{
		// takes m_tileColorPalette.back().id for current
		if (!m_tileColorPalette.empty())
		{
			newColorTileId = current + 1;
		}
		else {
			newColorTileId = 1; // Default starting id
		}
		return newColorTileId;
	}

	int MapEditor::getTextureId(int current)
	{
		// takes m_tileTexturePalette.back().id for current
		if (!m_tileTexturePalette.empty())
		{
			newTextureTileId = current + 1;
		}
		else {
			newTextureTileId = 1; // Default starting id
		}
		return newTextureTileId;
	}

	Vec4 MapEditor::getColor(int tileId) const
	{
		for (const auto& tile : m_tileColorPalette)
			if (tile.id == tileId)
				return tile.color;

		return Vec4(1.0f, 0.0f, 1.0f, 1.0f); // fallback magenta
	}

	Vec4 MapEditor::getSpawnColor(int tileId) const
	{
			for (const auto& stile : m_tileSpawnPalette)
				if (stile.id == tileId)
					return stile.color;
		return Vec4(1.0f, 0.0f, 1.0f, 1.0f); // fallback
	}

	Texture* MapEditor::getTexture(int tileId) const
	{
		for (const auto& tile : m_tileTexturePalette)
			if (tile.id == tileId)
				return tile.texture;
		return nullptr;
	}
	void MapEditor::openSaveDialog()
	{
		m_showSaveDialog = true;
		ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Save World File", ".world");
	}
	void MapEditor::openLoadDialog()
	{
		m_showLoadDialog = true;
		ImGuiFileDialog::Instance()->OpenDialog("LoadFileDlgKey", "Load World File", ".world");
	}
	void MapEditor::openConfigSaveDialog()
	{
		m_showSaveConfig = true;
		ImGuiFileDialog::Instance()->OpenDialog("SaveConfigDlgKey", "Save Config File", ".config");
	}
	void MapEditor::openTexture()
	{
		m_showTextureDialog = true;
		ImGuiFileDialog::Instance()->OpenDialog("ChooseTex", "Select Texture", ".bmp"); // change to .png, .jpg, .dds

	}
	void MapEditor::openConfigLoadDialog()
	{
		m_showLoadConfig = true;
		ImGuiFileDialog::Instance()->OpenDialog("LoadConfigDlgKey", "Load Config File", ".config");
	}

	bool MapEditor::loadConfigFile(const std::string& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open())
			return false;

		std::string line;
		bool inTilesBlock = false;

		m_tileColorPalette.clear();
		m_tileTexturePalette.clear();
		//m_spawnPalette.clear();

		auto trim = [](std::string& s) {
			s.erase(0, s.find_first_not_of(" \t\r\n"));
			s.erase(s.find_last_not_of(" \t\r\n") + 1);
			};

		while (std::getline(file, line))
		{
			trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			if (line == "[tiles]") {
				inTilesBlock = true;
				continue;
			}
			if (line == "{")
				continue;
			if (line == "}") {
				inTilesBlock = false;
				continue;
			}

			if (!inTilesBlock)
				continue;

			// Handle ColorTile and TextureTile headers
			if (line.find("[ColorTile]") == 0 || line.find("[TextureTile]") == 0 || line.find("[SpawnTile]") == 0)
			{
				std::string entryType = line;

				if (!std::getline(file, line)) break;
				trim(line);

				if (entryType == "[ColorTile]")
				{
					// Format: id, name, vec4(...)
					size_t vecStart = line.find("vec4(");
					size_t vecEnd = line.find(")", vecStart);
					if (vecStart == std::string::npos || vecEnd == std::string::npos)
						continue;

					std::string header = line.substr(0, vecStart);
					std::string vecContent = line.substr(vecStart + 5, vecEnd - vecStart - 5); // inside vec4(...)
					std::replace(vecContent.begin(), vecContent.end(), ',', ' ');

					// Remove 'f' suffixes from floats
					vecContent.erase(std::remove(vecContent.begin(), vecContent.end(), 'f'), vecContent.end());

					int id = 0;
					std::string name;
					std::istringstream headerSS(header);
					std::string idStr, nameStr;
					std::getline(headerSS, idStr, ',');
					std::getline(headerSS, nameStr, ',');

					trim(idStr);
					trim(nameStr);
					try {
						id = std::stoi(idStr);
					}
					catch (...) {
						continue;
					}

					float r = 0, g = 0, b = 0, a = 0;
					std::istringstream vecSS(vecContent);
					vecSS >> r >> g >> b >> a;

					m_tileColorPalette.push_back({ id, nameStr, Vec4(r, g, b, a) });
				}
				else if (entryType == "[TextureTile]")
				{
					// Format: id, name, path
					std::istringstream ss(line);
					std::string idStr, nameStr, pathStr;
					std::getline(ss, idStr, ',');
					std::getline(ss, nameStr, ',');
					std::getline(ss, pathStr);

					trim(idStr);
					trim(nameStr);
					trim(pathStr);

					int id = 0;
					try {
						id = std::stoi(idStr);
					}
					catch (...) {
						continue;
					}

					//Texture* tex = TextureManager::add(new Texture(nameStr, pathStr));
					Texture* tex = TextureManager::add(new Texture(nameStr, pathStr));
					m_tileTexturePalette.push_back({ id, nameStr, pathStr, tex});
				}
				// SpawnTiles
				else if (entryType == "[SpawnTile]")
				{
					// Format: id, name, vec4(...)
					size_t vecStart = line.find("vec4(");
					size_t vecEnd = line.find(")", vecStart);
					if (vecStart == std::string::npos || vecEnd == std::string::npos)
						continue;

					std::string header = line.substr(0, vecStart);
					std::string vecContent = line.substr(vecStart + 5, vecEnd - vecStart - 5); // inside vec4(...)
					std::replace(vecContent.begin(), vecContent.end(), ',', ' ');

					// Remove 'f' suffixes from floats
					vecContent.erase(std::remove(vecContent.begin(), vecContent.end(), 'f'), vecContent.end());

					int id = 0;
					std::string name;
					std::istringstream headerSS(header);
					std::string idStr, nameStr;
					std::getline(headerSS, idStr, ',');
					std::getline(headerSS, nameStr, ',');

					trim(idStr);
					trim(nameStr);
					try {
						id = std::stoi(idStr);
					}
					catch (...) {
						continue;
					}

					float r = 0, g = 0, b = 0, a = 0;
					std::istringstream vecSS(vecContent);
					vecSS >> r >> g >> b >> a;

					SpawnType type = parseSpawnType(nameStr);
					// OR if you refactor TileSpawnInfo to store SpawnType:
					m_tileSpawnPalette.push_back({ id, type, Vec4(r, g, b, a) });
				}
			}
		}
		return true;
	}

	bool MapEditor::saveConfigFile(const std::string& filepath)
	{
		std::ofstream file(filepath);
		if (!file.is_open())
			return false;

		file << "[tiles]\n{\n";

		// Save ColorTile entities
		for (const auto& colorTile : m_tileColorPalette)
		{
			file << "[ColorTile]\n";
			file << colorTile.id << ", " << colorTile.name << ", "
				<< "vec4(" << colorTile.color.x << "f, "
				<< colorTile.color.y << "f, "
				<< colorTile.color.z << "f, "
				<< colorTile.color.w << "f)\n";
		}
		// Save TextureTile entities
		for (const auto& textureTile : m_tileTexturePalette)
		{
			file << "[TextureTile]\n";
			file << textureTile.id << ", " << textureTile.name << ", " << textureTile.path << "\n";
		}
		for (const auto& spawnTile : m_tileSpawnPalette)
		{
			file << "[SpawnTile]\n";
			file << spawnTile.id << ", " << spawnTypeToString(spawnTile.type) << ", "
				<< "vec4(" << spawnTile.color.x << "f, "
				<< spawnTile.color.y << "f, "
				<< spawnTile.color.z << "f, "
				<< spawnTile.color.w << "f)\n";
		}

		file << "}\n";
		return true;
	}

	std::string MapEditor::spawnTypeToString(SpawnType type)
	{
		switch (type)
		{
		case SpawnType::Player: return "Player";
		case SpawnType::Npc: return "Npc";
		case SpawnType::Enemey: return "Enemey";
		default: return "None";
		}
	}

	SpawnType MapEditor::parseSpawnType(const std::string& str)
	{
		if (str == "Player") return SpawnType::Player;
		if (str == "Npc") return SpawnType::Npc;
		if (str == "Enemey") return SpawnType::Enemey;
		return SpawnType::None;
	}
}