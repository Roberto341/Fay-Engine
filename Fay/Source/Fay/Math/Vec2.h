#pragma once

#include <iostream>

namespace Fay
{
	struct Vec2
	{
		float x, y;

		Vec2();
		Vec2(const float& x, const float& y);


		Vec2& add(const Vec2& other);
		Vec2& sub(const Vec2& other);
		Vec2& mul(const Vec2& other);
		Vec2& div(const Vec2& other);

		friend Vec2 operator+(Vec2 left, const Vec2& right);
		friend Vec2 operator-(Vec2 left, const Vec2& right);
		friend Vec2 operator*(Vec2 left, const Vec2& right);
		friend Vec2 operator/(Vec2 left, const Vec2& right);

		bool operator==(const Vec2& other);
		bool operator!=(const Vec2& other);

		Vec2& operator+=(const Vec2& other);
		Vec2& operator-=(const Vec2& other);
		Vec2& operator*=(const Vec2& other);
		Vec2& operator/=(const Vec2& other);
		friend std::ostream& operator<<(std::ostream& stream, const Vec2& vector);
	};
}