#pragma once

#include <Graphics/Layers/Layer.h>
#include <Renderer/BatchRenderer.h>
namespace Fay
{
	enum class ProjectionType { Quad2D, Cube3D };
	class TileLayer : public Layer
	{
	private:
		ProjectionType m_ptype = ProjectionType::Quad2D; // defaults to 2d
	public:
		TileLayer(BatchRenderer* renderer);
		void setProjectionType(ProjectionType type);
		void setShader(Shader* shader);
		~TileLayer();
	};
}