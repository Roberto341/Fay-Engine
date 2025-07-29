#pragma once
#include <Renderer/Sprite.h> // replace with object and mesh
#include <Graphics/Texture.h>
#include <Graphics/TextureManager.h>
#include <Math/Math.h>
#include <filesystem>
#include <Renderer/Renderer2D.h>

namespace Fay
{
	class Scene2D
	{
	public: 
		void addObject(Sprite* sprite);
		//void addCube(Cube* cube);
		void clear();
		
		const std::vector<Sprite*>& getObjects() const { return m_objects; }
		//const std::vector<Cube*>& getCubes() const { return m_cubes; } // change to load and get objects

		bool saveToFile(const std::string& filepath) const;
		bool loadFromFile(const std::string& filepath, TextureManager& texManager);
		bool deleteSceneFile(const std::string& filepath);
		std::vector<std::string> listScenesInDirectory(const std::string& dir);
	private:
		void writeString(std::ofstream& out, const std::string& str) const;
		std::string readString(std::ifstream& in) const;
		std::vector<Sprite*> m_objects;
		//std::vector<Cube*> m_cubes;
	};
}