#include <Renderer/Scene2D.h>

namespace Fay
{
	void Scene2D::addObject(Sprite* sprite)
	{
		m_objects.push_back(sprite);
	}
	void Scene2D::clear()
	{
		for (auto s : m_objects)
			delete s;
		m_objects.clear();
	}
	bool Scene2D::saveToFile(const std::string& filepath) const
	{
		std::ofstream out(filepath, std::ios::binary);
		if (!out.is_open()) return false;

		uint32_t count = m_objects.size();
		out.write(reinterpret_cast<char*>(&count), sizeof(uint32_t));

		for (const auto& sprite : m_objects)
		{
			// Position and size;
			out.write(reinterpret_cast<const char*>(&sprite->pos), sizeof(Vec3));
			out.write(reinterpret_cast<const char*>(&sprite->size), sizeof(Vec2));
			out.write(reinterpret_cast<const char*>(&sprite->color), sizeof(Vec4));
			// Texture
			std::string texName = sprite->getTexture() ? sprite->getTexture()->getName() : "";
			writeString(out, texName);

			// Collision
			out.write(reinterpret_cast<const char*>(&sprite->hasCollision), sizeof(bool));
		}
		return true;
	}
	bool Scene2D::loadFromFile(const std::string& filepath, TextureManager& texManager)
	{
		std::ifstream in(filepath, std::ios::binary);
		if (!in.is_open()) return false;

		clear();

		uint32_t count;
		in.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));

		for (uint32_t i = 0; i < count; i++)
		{
			Vec3 pos;
			Vec2 size;
			Vec4 color;
			bool hasCol;
			std::string texName;

			in.read(reinterpret_cast<char*>(&pos), sizeof(Vec3));
			in.read(reinterpret_cast<char*>(&size), sizeof(Vec2));
			in.read(reinterpret_cast<char*>(&color), sizeof(Vec4));
			texName = readString(in);
			in.read(reinterpret_cast<char*>(&hasCol), sizeof(bool));

			Sprite* sprite = nullptr;

			if (!texName.empty())
			{
				Texture* tex = texManager.getTexture(texName);
				sprite = new Sprite(pos.x, pos.y, size.x, size.y, tex);
			}
			else
			{
				sprite = new Sprite(pos.x, pos.y, size.x, size.y, color); // Re assign the color
			}
			sprite->setCollision(hasCol);
			m_objects.push_back(sprite);
		}
		return true;
	}
	bool Scene2D::deleteSceneFile(const std::string& filepath)
	{
		try {
			return std::filesystem::remove(filepath);
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::cerr << "Error deleting file: " << e.what() << std::endl;
			return false;
		}
	}
	std::vector<std::string> Scene2D::listScenesInDirectory(const std::string& dir)
	{
		std::vector<std::string> files;
		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (entry.path().extension() == ".scene_2d")
				files.push_back(entry.path().filename().string());
		}
		return files;
	}
	void Scene2D::writeString(std::ofstream& out, const std::string& str) const
	{
		uint32_t len = str.size();
		out.write(reinterpret_cast<const char*>(&len), sizeof(uint32_t));
		out.write(str.data(), len);
	}
	std::string Scene2D::readString(std::ifstream& in) const
	{
		uint32_t len = 0;
		in.read(reinterpret_cast<char*>(&len), sizeof(uint32_t));
		std::string str(len, '\0');
		in.read(&str[0], len);
		return str;
	}
}