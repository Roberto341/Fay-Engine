#pragma once

#include <Math/Math.h>

namespace Fay
{
	class Camera
	{
	public:
		Camera(float screenWidth, float screenHeight);

		void setPosition(const Vec3& pos);
		void move(const Vec3& delta);
		void setZoom(float zoomLevel);
		void resize(int width, int height);
		float getZoom() const { return zoom; }

		void updateProjection();
		void update();

		const Mat4 getViewMatrix();
		const Mat4 getProjection();
	private:
		Vec3 position;
		float m_width, m_height;
		float m_zoom;
		float zoom;
		float screenWidth, screenHeight;
		Mat4 projection;
		Mat4 viewMatrix;
	};
}