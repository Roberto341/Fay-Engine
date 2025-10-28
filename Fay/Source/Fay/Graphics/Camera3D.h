#pragma once
#include <Math/Math.h>

namespace Fay
{
		class Camera3D
		{
		private: 
			Vec3 m_position;
			Vec3 m_forward;
			Vec3 m_up;
			Mat4 m_viewMatrix;
			Mat4 m_projMatrix;
			float m_fov;
			float m_aspect;
			float m_near;
			float m_far;
			bool m_dirty;
		public:
			Camera3D(const Vec3& position, const Vec3& forward, const Vec3& up);

			void setPosition(const Vec3& pos);
			void setPerspective(float fov, float aspect, float near, float far);
			void move(const Vec3& delta);
			void updateViewMatrix();
			Mat4 getViewMatrix();
			Mat4 getProjectionMatrix(float aspect);
			Vec3 getForward() const { return m_forward; }
			Vec3 getPosition() const { return m_position; }

		};
	}