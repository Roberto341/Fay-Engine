#pragma once

#include <Graphics/Layers/Layer.h>
#include <Renderer/BatchRenderer.h>
namespace Fay
{
	class TileLayer : public Layer
	{
	public:
		TileLayer(BatchRenderer* renderer);
		~TileLayer();
	};
}