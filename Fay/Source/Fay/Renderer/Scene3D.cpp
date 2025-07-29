#include <Renderer/Scene3D.h>

namespace Fay
{
	void Scene3D::addObject(Cube* cube)
	{
		m_objects.push_back(cube);
	}
	void Scene3D::clear()
	{
		for (auto s : m_objects)
			delete s;
		m_objects.clear();
	}
	bool Scene3D::saveToFile(const std::string& filepath) const
	{
		std::ofstream out(filepath, std::ios::binary);
		if (!out.is_open()) return false;

		uint32_t count = m_objects.size();
		out.write(reinterpret_cast<char*>(&count), sizeof(uint32_t));

		for (const auto& sprite : m_objects)
		{
			// Position and size;
			out.write(reinterpret_cast<const char*>(&sprite->pos), sizeof(Vec3));
			out.write(reinterpret_cast<const char*>(&sprite->size), sizeof(Vec3));
			out.write(reinterpret_cast<const char*>(&sprite->color), sizeof(Vec4));
		}
		return true;
	}
	bool Scene3D::loadFromFile(const std::string& filepath)
	{
		std::ifstream in(filepath, std::ios::binary);
		if (!in.is_open()) return false;

		clear();

		uint32_t count;
		in.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));

		for (uint32_t i = 0; i < count; i++)
		{
			Vec3 pos;
			Vec3 size;
			Vec4 color;

			in.read(reinterpret_cast<char*>(&pos), sizeof(Vec3));
			in.read(reinterpret_cast<char*>(&size), sizeof(Vec3));
			in.read(reinterpret_cast<char*>(&color), sizeof(Vec4));

			Cube* cube = nullptr;

			cube = new Cube(pos.x, pos.y, pos.z, size.x, size.y, size.z, color); // Re assign the color
			m_objects.push_back(cube);
		}
		return true;
	}
	bool Scene3D::deleteSceneFile(const std::string& filepath)
	{
		try {
			return std::filesystem::remove(filepath);
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::cerr << "Error deleting file: " << e.what() << std::endl;
			return false;
		}
	}
	std::vector<std::string> Scene3D::listScenesInDirectory(const std::string& dir)
	{
		std::vector<std::string> files;
		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (entry.path().extension() == ".scene_3d")
				files.push_back(entry.path().filename().string());
		}
		return files;
	}
	void Scene3D::writeString(std::ofstream& out, const std::string& str) const
	{
		uint32_t len = str.size();
		out.write(reinterpret_cast<const char*>(&len), sizeof(uint32_t));
		out.write(str.data(), len);
	}
	std::string Scene3D::readString(std::ifstream& in) const
	{
		uint32_t len = 0;
		in.read(reinterpret_cast<char*>(&len), sizeof(uint32_t));
		std::string str(len, '\0');
		in.read(&str[0], len);
		return str;
	}
}