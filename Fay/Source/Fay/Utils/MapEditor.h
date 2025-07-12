#pragma once

#include <vector>
#include <string>
#include <fstream>
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

	class MapEditor
	{
	public:
		MapEditor(TileLayer* layer, int width, int height, float tileSize);
		void renderImGui();
		void render();
		void update();
		void handleInput();
		void clean();
		void setTile(int x, int y, int tileType);
		int getTile(int x, int y) const;

		bool saveToFile(const std::string& filepath) const;
		bool loadFromFile(const std::string& filepath);

		Vec4 getColor(int tileId) const;
	private:
		void openSaveDialog();
		void openLoadDialog();
		bool m_showSaveDialog = false;
		bool m_showLoadDialog = false;
		int width, height;
		float tileSize;
		std::vector<int> m_tiles;
		TileLayer* m_tileLayer;
		std::vector<Sprite*> m_spriteCache;
		int m_selectedTile = 1;
	};
}