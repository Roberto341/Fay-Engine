#include <Graphics/Layers/TileLayer.h>

namespace Fay
{
	TileLayer::TileLayer(BatchRenderer* renderer)
		: Layer(renderer, m_shader, Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f), (m_ptype == ProjectionType::Cube3D) ? Mat4::identity() : Mat4::translation(Vec3(0, 0, 0)))// for view matrix for cube
	{
		m_shader = nullptr;
		//m_shader = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
	}
	void TileLayer::setProjectionType(ProjectionType type)
	{
		m_ptype = type;
	}
	void TileLayer::setShader(Shader* shader)
	{
		m_shader = shader;
	}
	TileLayer::~TileLayer()
	{
	}
}