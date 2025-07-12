#include <Utils/MapEditor.h>

namespace Fay
{
	MapEditor::MapEditor(TileLayer* layer, int width, int height, float tileSize)
		: width(width), height(height), tileSize(tileSize), m_tileLayer(layer)
	{
		m_tiles.resize(width * height, 0);
	}

	void MapEditor::renderImGui()
	{
		int maxTileId = 3;
		ImGui::Begin("Map Editor");

		// Tile selection UI
		ImGui::Text("Selected Tile ID: %d", m_selectedTile);
		ImGui::SliderInt("Tile ID", &m_selectedTile, 0, maxTileId);

		// Canvas setup
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();             // Top-left corner of canvas
		ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Size of canvas
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
		         int tileID = getTile(x, y);
		         if (tileID != 0)
		         {
		             ImVec2 min = ImVec2(canvas_p0.x + x * tileSize, canvas_p0.y + y * tileSize);
		             ImVec2 max = ImVec2(min.x + tileSize, min.y + tileSize);
		             draw_list->AddRectFilled(min, max, IM_COL32(200, 200, 100, 255));
		             // Optionally add tile ID text
		             // draw_list->AddText(min, IM_COL32(0,0,0,255), std::to_string(tileID).c_str());
		         }
		     }
		 }

		// Handle painting tiles on click
		if (is_hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			ImVec2 mouse_pos = ImGui::GetMousePos();
			int tileX = (int)((mouse_pos.x - canvas_p0.x) / tileSize);
			int tileY = (int)((mouse_pos.y - canvas_p0.y) / tileSize);

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
;				}
				ImGuiFileDialog::Instance()->Close();
				m_showLoadDialog = false;
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
				int tileId = getTile(x, y);
				Vec4 color = getColor(tileId);
				auto sprite = new Sprite((float)x, (float)y, 1.0f, 1.0f, color);
				m_tileLayer->add(sprite);
				m_spriteCache.push_back(sprite);
			}
		}
		
	}
	void MapEditor::handleInput()
	{
		// Only paint when mouse is NOT over ImGui windows
		if (!ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			ImVec2 mouse = ImGui::GetMousePos();

			// Optional: convert screen coordinates to world if using a camera
			int x = static_cast<int>(mouse.x / tileSize);
			int y = static_cast<int>(mouse.y / tileSize);

			// Defensive bounds check
			if (x >= 0 && x < width && y >= 0 && y < height)
			{
				setTile(x, y, m_selectedTile);
			}
		}
	}

	void MapEditor::setTile(int x, int y, int tileType)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			m_tiles[y * width + x] = tileType;
		}
	}
	int MapEditor::getTile(int x, int y) const
	{
		if (x < 0 || x >= width || y < 0 || y >= height) return -1;
		return m_tiles[y * width + x];
	}

	bool MapEditor::saveToFile(const std::string& filepath) const {
		std::ofstream file(filepath);
		if (!file.is_open())
			return false;

		// Write metadata
		file << "width=" << width << "\n";
		file << "height=" << height << "\n";
		file << "map=\n";

		// Write map data line-by-line
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				file << getTile(x, y) << "";
			}
			file << "\n";
		}

		file.close();
		return true;
	}
	bool MapEditor::loadFromFile(const std::string& filepath)
	{
		std::ifstream file(filepath);
		std::string line;
		bool readingMap = false;
		int rowIndex = 0;

		while (std::getline(file, line)) {
			if (line.find("map=") == 0) {
				readingMap = true;
				continue;
			}

			if (readingMap) {
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < std::min((int)line.length(), width); x++) {
						int id;
						file >> id;
						setTile(x, y, id);
					}
					rowIndex++;
				}
				continue;
			}
		}
		return true;
	}
	Vec4 MapEditor::getColor(int tileId) const
	{
		switch (tileId)
		{
		case 0: return Vec4(0.2f, 0.2f, 0.2f, 1.0f); // Empty
		case 1: return Vec4(0.0f, 0.8f, 0.0f, 1.0f); // Grass
		default: return Vec4(1.0f, 0.0f, 1.0f, 1.0f); // Unkown = magenta
		}
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
}