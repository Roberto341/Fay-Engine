#pragma once

#include <Math/Vec2.h>
#include <Math/Vec3.h>
#include <Math/Vec4.h>
#include <Math/Mat4.h>
#include <math.h>
namespace Fay
{
	struct Ray
	{
		Vec3 origin;
		Vec3 dir;
	};
	inline float clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}
	inline Ray getRayFromMouse(const Vec2& mousePos, const Vec2& viewportPos, const Vec2& viewportSize, const Mat4& proj, const Mat4& view)
	{
		// 1. Convert mouse position to normalized device coordinates (-1 to 1)
		float relX = mousePos.x - viewportPos.x;
		float relY = mousePos.y - viewportPos.y;

		float ndcX = (2.0f * relX) / viewportSize.x - 1.0f;
		float ndcY = 1.0f - (2.0f * relY) / viewportSize.y;

		// 2. Define clip-space positions for near and far planes
		Vec4 rayStartClip(ndcX, ndcY, -1.0f, 1.0f);
		Vec4 rayEndClip(ndcX, ndcY, 1.0f, 1.0f);

		// 3. Invert the combined view-projection matrix
		Mat4 invViewProj = (proj * view).inverse();

		// 4. Unproject to world space
		Vec4 rayStartWorld = invViewProj * rayStartClip;
		rayStartWorld /= rayStartWorld.w;

		Vec4 rayEndWorld = invViewProj * rayEndClip;
		rayEndWorld /= rayEndWorld.w;

		// 5. Build ray
		Vec3 origin(rayStartWorld.x, rayStartWorld.y, rayStartWorld.z);
		Vec3 direction = Vec3(
			rayEndWorld.x - rayStartWorld.x,
			rayEndWorld.y - rayStartWorld.y,
			rayEndWorld.z - rayStartWorld.z
		).normalized();

		return { origin, direction };
	}
	inline bool intersectRayAABB(const Vec3& rayOrigin, const Vec3& rayDir, const Vec3& aabbMin, const Vec3& aabbMax, float& t)
	{
		float t1 = (aabbMin.x - rayOrigin.x) / rayDir.x;
		float t2 = (aabbMax.x - rayOrigin.x) / rayDir.x;
		float t3 = (aabbMin.y - rayOrigin.y) / rayDir.y;
		float t4 = (aabbMax.y - rayOrigin.y) / rayDir.y;
		float t5 = (aabbMin.z - rayOrigin.z) / rayDir.z;
		float t6 = (aabbMax.z - rayOrigin.z) / rayDir.z;

		float tmin = std::max({ std::min(t1,t2), std::min(t3,t4), std::min(t5,t6) });
		float tmax = std::min({ std::max(t1,t2), std::max(t3,t4), std::max(t5,t6) });

		if (tmax < 0 || tmin > tmax)
			return false;

		t = (tmin < 0.0f) ? tmax : tmin;
		return true;
	}
}