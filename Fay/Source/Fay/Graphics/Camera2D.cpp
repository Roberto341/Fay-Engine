#include <Graphics/Camera2D.h>
namespace Fay
{
	Camera2D::Camera2D(float screenWidth, float screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight), position(0.0f, 0.0f, 0.0f), zoom(1.0f)
		{
			m_width = screenWidth;
			m_height = screenHeight;
			m_zoom = 0.0f;
			projection = Mat4::orthographic(0.0f, screenWidth, 0.0f, screenHeight, -1.0f, 1.0f);
			update();
		}

		void Camera2D::setPosition(const Vec3& pos)
		{
			position = pos;
			update();
		}

		void Camera2D::move(const Vec3& delta)
		{
			position += delta;
			update();
		}

		void Camera2D::setZoom(float zoomLevel)
		{
			zoom = zoomLevel > 0.0f ? zoomLevel : 1.0f; // clamp zoom so no divide by zero

			float halfWidth = screenWidth / 2.0f;
			float halfHeight = screenHeight / 2.0f;

			// Zoom *scales* the view size — higher zoom means you see *less* world (zoomed in)
			float viewHalfWidth = halfWidth / zoom;
			float viewHalfHeight = halfHeight / zoom;

		//	// Left, Right, Bottom, Top
			projection = Mat4::orthographic(
				-viewHalfWidth, viewHalfWidth,
				-viewHalfHeight, viewHalfHeight,
				-1.0f, 1.0f
			);
			m_zoom = zoom;
			updateProjection();
		}

		void Camera2D::resize(int width, int height)
		{
			m_width = (float)width;
			m_height = (float)height;
			updateProjection();
		}

		void Camera2D::updateProjection()
		{
			float halfWidth = m_width / 2.0f / m_zoom;
			float halfHeight = m_height / 2.0f / m_zoom;
			projection = Mat4::orthographic(
				-halfWidth, halfWidth,
				-halfHeight, halfHeight,
				-1.0f, 1.0f
			);
			update();
		}


		void Camera2D::update()
		{
			viewMatrix = Mat4::translation(Vec3(-position.x, -position.y, 0.0f));
		}

		const Mat4 Camera2D::getViewMatrix()
		{
			return viewMatrix;
		}
		const Mat4 Camera2D::getProjection()
		{
			return projection;
		}
}