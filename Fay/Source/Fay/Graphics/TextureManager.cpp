#include <Graphics/TextureManager.h>

namespace Fay
{
	std::vector<Texture*> TextureManager::m_textures;

	TextureManager::TextureManager()
	{
	}
	TextureManager::~TextureManager()
	{
	}
	
	Texture* TextureManager::add(Texture* texture)
	{
		m_textures.push_back(texture);
		return texture;
	}
	void TextureManager::clean()
	{
		for (int i = 0; i < m_textures.size(); i++)
		{
			delete m_textures[i];
		}
	}
	Texture* TextureManager::getTexture(const std::string& name)
	{
		//return nullptr;
		for (Texture* texture : m_textures)
		{
			if (texture->getName() == name)
			{
				return texture;
			}
		}
		return nullptr;
	}
	
}