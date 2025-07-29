#pragma once
#include <Renderer/Sprite.h>
#include <Renderer/3DRenderer/Cube.h> // replace with object and mesh
#include <Math/Math.h>
#include <filesystem>
#include <Renderer/3DRenderer/Renderer3d.h>

namespace Fay
{
	class Scene3D
	{
	public:
		void addObject(Cube* cube);
		void clear();

		const std::vector<Cube*>& getObjects() const { return m_objects; }

		bool saveToFile(const std::string& filepath) const;
		bool loadFromFile(const std::string& filepath);
		bool deleteSceneFile(const std::string& filepath);
		std::vector<std::string> listScenesInDirectory(const std::string& dir);
	private:
		void writeString(std::ofstream& out, const std::string& str) const;
		std::string readString(std::ifstream& in) const;
		std::vector<Cube*> m_objects;
	};
}