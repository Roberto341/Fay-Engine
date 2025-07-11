#include <Math/Vec2.h>

namespace Fay
{
	Vec2::Vec2()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vec2::Vec2(const float& x, const float& y)
	{
		this->x = x;
		this->y = y;
	}
	Vec2& Vec2::add(const Vec2& other)
	{
		x += other.x;
		y += other.y;

		return *this;
	}

	Vec2& Vec2::sub(const Vec2& other)
	{
		x -= other.x;
		y -= other.y;

		return *this;
	}

	Vec2& Vec2::mul(const Vec2& other)
	{
		x *= other.x;
		y *= other.y;

		return *this;
	}

	Vec2& Vec2::div(const Vec2& other)
	{
		x /= other.x;
		y /= other.y;

		return *this;
	}

	Vec2 operator+(Vec2 left, const Vec2& right)
	{
		return left.add(right);
	}

	Vec2 operator-(Vec2 left, const Vec2& right)
	{
		return left.sub(right);
	}

	Vec2 operator*(Vec2 left, const Vec2& right)
	{
		return left.mul(right);
	}

	Vec2 operator/(Vec2 left, const Vec2& right)
	{
		return left.div(right);
	}

	Vec2& Vec2::operator+=(const Vec2& other)
	{
		*this = *this + other;
		return *this;
	}

	Vec2& Vec2::operator-=(const Vec2& other)
	{
		*this = *this - other;
		return *this;
	}

	Vec2& Vec2::operator*=(const Vec2& other)
	{
		*this = *this * other;
		return *this;
	}

	Vec2& Vec2::operator/=(const Vec2& other)
	{
		*this = *this / other;
		return *this;
	}

	bool Vec2::operator==(const Vec2& other)
	{
		return x == other.x && y == other.y;
	}

	bool Vec2::operator!=(const Vec2& other)
	{
		return x != other.x && y != other.y;
	}
	std::ostream& operator<<(std::ostream& stream, const Vec2& vector)
	{
		stream << "Vec2: (" << vector.x << ", " << vector.y << ")";
		return stream;
	}
}