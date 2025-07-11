#include <Graphics/Layers/TileLayer.h>

namespace Fay
{
	TileLayer::TileLayer(Shader* shader)
		: Layer(new BatchRenderer(), shader, Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f))
	{

	}
	TileLayer::~TileLayer()
	{
	}
}