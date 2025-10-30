#include <Graphics/Layers/TileLayer.h>

namespace Fay
{
	TileLayer::TileLayer(BatchRenderer* renderer)
		: Layer(renderer, m_shader, Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f), (m_ptype == ProjectionType::Cube3D) ? Mat4::identity() : Mat4::translation(Vec3(0, 0, 0)))// for view matrix for cube
	{
	}
	TileLayer::~TileLayer()
	{
	}
}