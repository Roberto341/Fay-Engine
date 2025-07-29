#include <Math/Vec4.h>

namespace Fay
{
	Vec4::Vec4()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}

	Vec4::Vec4(const float& x, const float& y, const float& z, const float& w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vec4& Vec4::add(const Vec4& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;

		return *this;
	}

	Vec4& Vec4::sub(const Vec4& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;

		return *this;
	}

	Vec4& Vec4::mul(const Vec4& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;

		return *this;
	}

	Vec4& Vec4::div(const Vec4& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;

		return *this;
	}

	Vec4 operator+(Vec4 left, const Vec4& right)
	{
		return left.add(right);
	}

	Vec4 operator-(Vec4 left, const Vec4& right)
	{
		return left.sub(right);
	}

	Vec4 operator*(Vec4 left, const Vec4& right)
	{
		return left.mul(right);
	}

	Vec4 operator/(Vec4 left, const Vec4& right)
	{
		return left.div(right);
	}

	Vec4& Vec4::operator+=(const Vec4& other)
	{
		*this = *this + other;
		return *this;
	}

	Vec4& Vec4::operator-=(const Vec4& other)
	{
		*this = *this - other;
		return *this;
	}

	Vec4& Vec4::operator*=(const Vec4& other)
	{
		*this = *this * other;
		return *this;
	}

	Vec4& Vec4::operator/=(const Vec4& other)
	{
		*this = *this / other;
		return *this;
	}

	bool Vec4::operator==(const Vec4& other)
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool Vec4::operator!=(const Vec4& other)
	{
		return x != other.x && y != other.y && z != other.z && w != other.w;
	}
	std::ostream& operator<<(std::ostream& stream, const Vec4& vector)
	{
		stream << "Vec4: (" << vector.x << ", " << vector.y << ", " << vector.z <<  ", " << vector.w << ")";
		return stream;
	}
}