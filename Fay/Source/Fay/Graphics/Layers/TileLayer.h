#pragma once

#include <Graphics/Layers/Layer.h>
#include <Renderer/BatchRenderer.h>
#include <Renderer/3DRenderer/BatchRenderer3D.h>
namespace Fay
{
	class TileLayer : public Layer
	{
	public:
		TileLayer(Shader* shader, BatchRenderer* renderer);
		TileLayer(Shader* shader, BatchRenderer3D* renderer);
		//TileLayer(Shader* shader);
		~TileLayer();
	};
}