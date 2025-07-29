#pragma once

#include <iostream>

namespace Fay
{
	struct Vec3
	{
		float x, y, z;

		Vec3();
		Vec3(const float& x, const float& y, const float& z);
		
		Vec3 normalized() const;
		float dot(const Vec3& other) const;
		float vecLength() const;
		Vec3 cross(const Vec3& other) const;

		Vec3& add(const Vec3& other);
		Vec3& sub(const Vec3& other);
		Vec3& mul(const Vec3& other);
		Vec3& div(const Vec3& other);
		Vec3 operator*(float scalar) const;
		friend Vec3 operator+(Vec3 left, const Vec3& right);
		friend Vec3 operator-(Vec3 left, const Vec3& right);
		friend Vec3 operator*(float scalar, const Vec3& vec);
		friend Vec3 operator*(Vec3 left, const Vec3& right);
		friend Vec3 operator/(Vec3 left, const Vec3& right);
		
		bool operator==(const Vec3& other);
		bool operator!=(const Vec3& other);

		Vec3& operator+=(const Vec3& other);
		Vec3& operator-=(const Vec3& other);
		Vec3& operator*=(const Vec3& other);
		Vec3& operator/=(const Vec3& other);
		friend std::ostream& operator<<(std::ostream& stream, const Vec3& vector);
	};
}