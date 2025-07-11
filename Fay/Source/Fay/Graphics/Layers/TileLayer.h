#pragma once

#include <Graphics/Layers/Layer.h>
#include <Renderer/BatchRenderer.h>
namespace Fay
{
	class TileLayer : public Layer
	{
	public:
		TileLayer(Shader* shader);
		~TileLayer();
	};
}