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
		void recalculateViewMatrix();
		void update();

		const Mat4 getViewMatrix();
		const Mat4 getProjection();

	private:
		Vec3 position;
		float zoom;
		float screenWidth, screenHeight;
		Mat4 projection;
		Mat4 viewMatrix;
	};
}