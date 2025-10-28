#include <Graphics/Camera3D.h>

namespace Fay
{
		Camera3D::Camera3D(const Vec3& position, const Vec3& forward, const Vec3& up)
			: m_position(position), m_forward(forward), m_up(up), m_dirty(true), 
			m_fov(70.0f), m_aspect(16.0f/9.0f), m_near(0.1f), m_far(1000.0f)
		{
		}

		void Camera3D::setPosition(const Vec3& pos)
		{
			m_position = pos;
		}
		void Camera3D::setPerspective(float fov, float aspect, float near, float far)
		{
			m_fov = fov;
			m_aspect = aspect;
			m_near = near;
			m_far = far;
		}
		Mat4 Camera3D::getViewMatrix()
		{
			if (m_dirty)
				updateViewMatrix();
			return m_viewMatrix;
		}

		Mat4 Camera3D::getProjectionMatrix(float aspect)
		{
			return Mat4::perspective(m_fov, aspect, m_near, m_far);
		}

		void Camera3D::updateViewMatrix()
		{
			m_viewMatrix = Mat4::lookAt(m_position, m_position + m_forward, m_up);
			m_dirty = false;
		}
		void Camera3D::move(const Vec3& delta)
		{	
			m_position += delta;
			updateViewMatrix();
		}
}