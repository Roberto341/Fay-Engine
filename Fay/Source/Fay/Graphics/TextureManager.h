#pragma once
#include <vector>
#include <Graphics/Texture.h>
namespace Fay
{
	class TextureManager
	{
	public:
		TextureManager();
		~TextureManager();
	
		//static void add(Texture* texture);
		static Texture* add(Texture* texture);
		static Texture* getTexture(const std::string& name);
		static void clean();
		static const std::vector<Texture*>& getTextures() { return m_textures; }
	private:
		static std::vector<Texture*> m_textures;
	};
}