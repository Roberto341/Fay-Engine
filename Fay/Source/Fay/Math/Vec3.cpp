#include <Math/Vec3.h>

namespace Fay
{
	Vec3::Vec3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vec3::Vec3(const float& x, const float& y, const float& z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vec3& Vec3::add(const Vec3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;

		return *this;
	}

	Vec3& Vec3::sub(const Vec3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;

		return *this;
	}

	Vec3& Vec3::mul(const Vec3& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;

		return *this;
	}

	Vec3& Vec3::div(const Vec3& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;

		return *this;
	}

	Vec3 operator+(Vec3 left, const Vec3& right)
	{
		return left.add(right);
	}

	Vec3 operator-(Vec3 left, const Vec3& right)
	{
		return left.sub(right);
	}

	Vec3 operator*(Vec3 left, const Vec3& right)
	{
		return left.mul(right);
	}

	Vec3 operator/(Vec3 left, const Vec3& right)
	{
		return left.div(right);
	}

	Vec3& Vec3::operator+=(const Vec3& other)
	{
		*this = *this + other;
		return *this;
	}

	Vec3& Vec3::operator-=(const Vec3& other)
	{
		*this = *this - other;
		return *this;
	}

	Vec3& Vec3::operator*=(const Vec3& other)
	{
		*this = *this * other;
		return *this;
	}

	Vec3& Vec3::operator/=(const Vec3& other)
	{
		*this = *this / other;
		return *this;
	}

	bool Vec3::operator==(const Vec3& other)
	{
		return x == other.x && y == other.y && z == other.z;
	}

	bool Vec3::operator!=(const Vec3& other)
	{
		return x != other.x && y != other.y && z != other.z;
	}
	std::ostream& operator<<(std::ostream& stream, const Vec3& vector)
	{
		stream << "Vec3: (" << vector.x << ", " << vector.y << ", " << vector.z << ")";
		return stream;
	}
}