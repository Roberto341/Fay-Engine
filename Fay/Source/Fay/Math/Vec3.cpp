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
	Vec3 Vec3::normalized() const
	{
		float len = vecLength();
		if (len == 0) return Vec3(0, 0, 0); // avoid divide by zero
		return Vec3(x / len, y / len, z / len);
	}
	float Vec3::dot(const Vec3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}
	float Vec3::vecLength() const
	{
		return sqrt(x * x + y * y + z * z);
	}
	Vec3 Vec3::cross(const Vec3& other) const
	{
		return Vec3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
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
	Vec3 Vec3::operator*(float scalar) const
	{
		return Vec3(x * scalar, y * scalar, z * scalar);
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

	Vec3 operator*(float scalar, const Vec3& vec)
	{
		return Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
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