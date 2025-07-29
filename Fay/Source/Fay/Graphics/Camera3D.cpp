#include <Graphics/Camera3D.h>

namespace Fay
{
		Camera3D::Camera3D(const Vec3& position, const Vec3& forward, const Vec3& up)
			: m_position(position), m_forward(forward), m_up(up), m_dirty(true)
		{
		}

		void Camera3D::setPosition(const Vec3& pos)
		{
			m_position = pos;
		}
		Mat4 Camera3D::getViewMatrix()
		{
			if (m_dirty)
				updateViewMatrix();
			return m_viewMatrix;
		}

		void Camera3D::updateViewMatrix()
		{
			m_viewMatrix = Mat4::lookAt(m_position, m_position + m_forward, m_up);
			m_dirty = false;
		}
		void Camera3D::move(const Vec3& delta)
		{	
			m_position += delta;
			//m_target += delta;
			updateViewMatrix();
		}
}