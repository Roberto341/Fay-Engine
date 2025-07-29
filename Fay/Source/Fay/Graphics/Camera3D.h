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
			bool m_dirty;
		public:
			Camera3D(const Vec3& position, const Vec3& forward, const Vec3& up);

			void setPosition(const Vec3& pos);
			void move(const Vec3& delta);
			void updateViewMatrix();
			Mat4 getViewMatrix();

			Vec3 getForward() const { return m_forward; }
			Vec3 getPosition() const { return m_position; }

		};
	}