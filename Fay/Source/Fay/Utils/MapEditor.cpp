#include <Utils/MapEditor.h>

namespace Fay
{
	MapEditor::MapEditor(TileLayer* layer, int width, int height, float tileSize)
		: width(width), height(height), tileSize(tileSize), m_tileLayer(layer)
	{
		m_tiles.resize(width * height, Tile(0, false));

		// load default config file
		loadConfigFile("Res/MapEditor/default.config");

		m_selectedTile = Tile(1, false);
	}

	void MapEditor::renderImGui()
	{
		ImGui::Begin("Map Editor");

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

			if (tile.texture)
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

		float squareSize = 48.0f;
		float pad = 4.0f;

		for (size_t i = 0; i < m_tileColorPalette.size(); ++i)
		{
			const auto& tile = m_tileColorPalette[i];
			ImGui::PushID(static_cast<int>(tile.id));

			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 size = ImVec2(squareSize, squareSize);

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
				ImGui::SameLine(0, pad);  // Keep on same line with padding
		}
		ImGui::NewLine();
		// Add texture tiles or color tiles directly
		ImGui::Separator();
		ImGui::Text("Add New Color Tile");

		ImGui::InputInt("Color Tile Id", &newColorTileId);
		ImGui::InputText("Color Tile name", newColorTileName, IM_ARRAYSIZE(newColorTileName));
		ImGui::ColorEdit4("Tile color", (float*)&newColor);

		if (ImGui::Button("Add Color Tile"))
		{
			m_tileColorPalette.push_back({ newColorTileId, std::string(newColorTileName), newColor });
		}
		// Canvas setup
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();             // Top-left corner of canvas
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
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				Tile tileID = getTile(x, y);

				//int tileID = getTile(x, y);
				if (tileID.id != 0)
				{
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
								ImVec2(0, 1), ImVec2(1, 0)
							);
						}
						else
						{
							draw_list->AddRectFilled(min, max, IM_COL32(255, 0, 0, 255)); // red fallback
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
								static_cast<int>(color.x * 255),
								static_cast<int>(color.y * 255),
								static_cast<int>(color.z * 255),
								static_cast<int>(color.w * 255)
							);
							draw_list->AddRectFilled(min, max, col);
						}
						else
						{
							draw_list->AddRectFilled(min, max, IM_COL32(255, 0, 255, 255)); // magenta fallback
						}
					}
					// Change this to display actual color or texture to corospond
					//draw_list->AddRectFilled(min, max, IM_COL32(200, 200, 100, 255));
					// Optionally add tile ID text
					// draw_list->AddText(min, IM_COL32(0,0,0,255), std::to_string(tileID).c_str());
				}
			}
		}

		// Handle painting tiles on click
		if (is_hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			ImVec2 mouse_pos = ImGui::GetMousePos(); // screen coords
			//ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
			//ImVec2 local_pos = ImVec2(mouse_pos.x - canvas_pos.x, mouse_pos.y - canvas_pos.y);
			int tileX = (int)((mouse_pos.x - canvas_p0.x) / tileSize);
			int tileY = height - 1 - (int)((mouse_pos.y - canvas_p0.y) / tileSize);

			if (tileX >= 0 && tileX < width && tileY >= 0 && tileY < height)
			{
				setTile(tileX, tileY, m_selectedTile);
			}
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
					;
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
		m_tileLayer->clear();
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
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
			}
		}
	}

	void MapEditor::setTile(int x, int y, const Tile& tile)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			m_tiles[y * width + x] = tile;
	}

	Tile MapEditor::getTile(int x, int y) const
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return Tile(-1, false);
		return m_tiles[y * width + x];
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
		return true;
	}

	bool MapEditor::loadFromFile(const std::string& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open()) return false;

		std::string line;
		bool readingMap = false;
		int rowIndex = 0;

		while (std::getline(file, line)) {
			if (line.find("width=") == 0) {
				width = std::stoi(line.substr(6));
				continue;
			}
			if (line.find("height=") == 0) {
				height = std::stoi(line.substr(7));
				continue;
			}
			if (line.find("map=") == 0) {
				readingMap = true;
				m_tiles.resize(width * height, Tile(-1, false));
				continue;
			}

			if (readingMap && rowIndex < height) {
				std::istringstream ss(line);
				for (int x = 0; x < width; ++x) {
					std::string token;
					if (!(ss >> token)) break;
					bool isTexture = false;
					int id = parseTileId(token, isTexture);
					setTile(x, rowIndex, Tile(id, isTexture));
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

	Vec4 MapEditor::getColor(int tileId) const
	{
		for (const auto& tile : m_tileColorPalette)
			if (tile.id == tileId)
				return tile.color;
		return Vec4(1.0f, 0.0f, 1.0f, 1.0f); // fallback magenta
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
			if (line.find("[ColorTile]") == 0 || line.find("[TextureTile]") == 0)
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

					Texture* tex = TextureManager::add(new Texture(nameStr, pathStr));
					m_tileTexturePalette.push_back({ id, nameStr, pathStr, tex });
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

		file << "[tiles]\n{";

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

		file << "}\n";
		return true;
	}
}