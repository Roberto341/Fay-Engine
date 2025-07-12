#include <Graphics/Camera.h>

namespace Fay
{
	Camera::Camera(float screenWidth, float screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight), position(0.0f, 0.0f, 0.0f), zoom(1.0f)
	{
		projection = Mat4::orthographic(0.0f, screenWidth, 0.0f, screenHeight, -1.0f, 1.0f);
		update();
	}

	void Camera::setPosition(const Vec3& pos)
	{
		position = pos;
		update();
	}

	void Camera::move(const Vec3& delta)
	{
		position += delta;
		update();
	}

	void Camera::setZoom(float zoomLevel)
	{
		zoom = zoomLevel;
		projection = Mat4::orthographic(0.0f, screenWidth * (1.0f / zoom), 0.0f, screenHeight * (1.0f / zoom), -1.0f, 1.0f);
		update();
	}

	void Camera::update()
	{
		viewMatrix = Mat4::translation(Vec3(-position.x, -position.y, 0.0f));
	}

	const Mat4 Camera::getViewMatrix()
	{
		return viewMatrix;
	}
	const Mat4 Camera::getProjection()
	{
		return projection;
	}
}