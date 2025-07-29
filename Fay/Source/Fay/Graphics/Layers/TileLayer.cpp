#include <Graphics/Layers/TileLayer.h>

namespace Fay
{
	TileLayer::TileLayer(Shader* shader, BatchRenderer* renderer)
		: Layer(renderer, shader, Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f))
	{

	}
	TileLayer::TileLayer(Shader* shader, BatchRenderer3D* renderer)
		: Layer(renderer, shader, Mat4::perspective(70.0f, 1920.0f / 1080.0f, 0.1f, 100.0f), Mat4::identity())  // fix this to use the camera
	{

	}
	TileLayer::~TileLayer()
	{
	}
}