#pragma once

#define _USE_MATH_DEFINES
#include <Math/Math.h>
#include <sstream>
#include <iomanip>

namespace Fay
{
	float toRadians(float degres);

	struct Mat4
	{
		union
		{
			float elements[16];
			Vec4 columns[4];
		};

		Mat4();
		Mat4(float diagonal);

		static Mat4 identity();

		Mat4& multiply(const Mat4& other);
		friend Mat4 operator*(const Mat4& left, const Mat4& right);
		Mat4& operator*=(const Mat4& other);

		Vec3 transformDirection(const Vec3& dir) const;
		Vec3 multiply(const Vec3& other) const;
		friend Vec3 operator*(const Mat4& left, const Vec3& right);

		Vec4 multiply(const Vec4& other) const;
		friend Vec4 operator*(const Mat4& left, const Vec4& right);

		static Mat4 orthographic(float left, float right, float bottom, float top, float near, float far);
		static Mat4 perspective(float fov, float aspectRatio, float near, float far);

		Mat4 transposed() const;
		static Mat4 translation(const Vec3& trans);
		static Mat4 rotation(float angle, const Vec3& axis);
		static Mat4 scale(const Vec3& scale);
		static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
		Mat4 inverse() const;
		std::string toString() const;
		inline float* data() { return elements; }
		inline const float* data() const { return elements; }
	};
}