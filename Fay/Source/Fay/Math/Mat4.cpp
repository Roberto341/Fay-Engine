#include <Math/Mat4.h>

namespace Fay
{
	Mat4::Mat4()
	{
		for (int i = 0; i < 4 * 4; i++)
			elements[i] = 0.0f;
	}
	Mat4::Mat4(float diagonal)
	{
		for (int i = 0; i < 4 * 4; i++)
			elements[i] = 0.0f;

		elements[0 + 0 * 4] = diagonal;
		elements[1 + 1 * 4] = diagonal;
		elements[2 + 2 * 4] = diagonal;
		elements[3 + 3 * 4] = diagonal;
	}
	Mat4 Mat4::identity()
	{
		return Mat4(1.0f);
	}
	Mat4& Mat4::multiply(const Mat4& other)
	{
		float data[16] = { 0 };
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				float sum = 0.0f;
				for (int k = 0; k < 4; k++)
				{
					sum += elements[row + k * 4] * other.elements[k + col * 4];
					//sum += elements[k + row * 4] * other.elements[col + k * 4];
				}
				data[row + col * 4] = sum;
			}
		}
		memcpy(elements, data, sizeof(float) * 16);
		return *this;
	}

	Vec3 Mat4::transformDirection(const Vec3& dir) const
	{
		return Vec3(
			elements[0] * dir.x + elements[4] * dir.y + elements[8] * dir.z,
			elements[1] * dir.x + elements[5] * dir.y + elements[9] * dir.z,
			elements[2] * dir.x + elements[6] * dir.y + elements[10] * dir.z
		);
	}

	Vec3 Mat4::multiply(const Vec3& other) const
	{
		return Vec3(
			columns[0].x * other.x + columns[1].x * other.y + columns[2].x * other.z + columns[3].x,
			columns[0].y * other.x + columns[1].y * other.y + columns[2].y * other.z + columns[3].y,
			columns[0].z * other.x + columns[1].z * other.y + columns[2].z * other.z + columns[3].z
		);
	}

	Vec4 Mat4::multiply(const Vec4& other) const
	{
		return Vec4(
			columns[0].x * other.x + columns[1].x * other.y + columns[2].x * other.z + columns[3].x * other.w,
			columns[0].y * other.x + columns[1].y * other.y + columns[2].y * other.z + columns[3].y * other.w,
			columns[0].z * other.x + columns[1].z * other.y + columns[2].z * other.z + columns[3].z * other.w,
			columns[0].w * other.x + columns[1].w * other.y + columns[2].w * other.z + columns[3].w * other.w
		);
	}

	Mat4 operator*(const Mat4& left, const Mat4& right)
	{
		Mat4 result = left;
		return result.multiply(right);
	}

	Mat4& Mat4::operator*=(const Mat4& other)
	{
		return multiply(other);
	}

	Vec3 operator*(const Mat4& left, const Vec3& right)
	{
		return left.multiply(right);
	}

	Vec4 operator*(const Mat4& left, const Vec4& right)
	{
		return left.multiply(right);
	}

	Mat4 Mat4::orthographic(float left, float right, float bottom, float top, float near, float far)
	{
		Mat4 result(1.0f);

		result.elements[0 + 0 * 4] = 2.0f / (right - left);

		result.elements[1 + 1 * 4] = 2.0f / (top - bottom);

		result.elements[2 + 2 * 4] = 2.0f / (near - far);

		result.elements[0 + 3 * 4] = (left + right) / (left - right);
		result.elements[1 + 3 * 4] = (bottom + top) / (bottom - top);
		result.elements[2 + 3 * 4] = (far + near) / (far - near);

		return result;
	}
	Mat4 Mat4::perspective(float fov, float aspectRatio, float near, float far)
	{
		Mat4 result(1.0f);

		float q = 1.0f / tan(toRadians(0.5f * fov));
		float a = q / aspectRatio;

		float b = (near + far) / (near - far);
		float c = (2.0f * near * far) / (near - far);
		result.elements[0 + 0 * 4] = a;
		result.elements[1 + 1 * 4] = q;
		result.elements[2 + 2 * 4] = b;
		result.elements[3 + 2 * 4] = -1.0f;
		result.elements[2 + 3 * 4] = c;

		return result;
	}
	Mat4 Mat4::transposed() const
	{
		Mat4 result;
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
				result.elements[x + y * 4] = elements[y + x * 4]; // Do a little flippy floppy
		return result;
	}
	Mat4  Mat4::translation(const Vec3& trans)
	{
		Mat4 result(1.0f);

		result.elements[0 + 3 * 4] = trans.x;
		result.elements[1 + 3 * 4] = trans.y;
		result.elements[2 + 3 * 4] = trans.z;

		return result;
	}

	Mat4 Mat4::rotation(float angle, const Vec3& axis)
	{
		Mat4 result(1.0f);
		float r = toRadians(angle);
		float c = cos(r);
		float s = sin(r);
		float omc = 1.0f - c;

		float x = axis.x;
		float y = axis.y;
		float z = axis.z;

		result.elements[0 + 0 * 4] = x * omc + c;
		result.elements[1 + 0 * 4] = y * x * omc + z * s;
		result.elements[2 + 0 * 4] = x * z * omc - y * s;

		result.elements[0 + 1 * 4] = x * y * omc - z * s;
		result.elements[1 + 1 * 4] = y * omc + c;
		result.elements[2 + 1 * 4] = y * z * omc + x * s;

		result.elements[0 + 2 * 4] = x * z * omc + y * s;
		result.elements[1 + 2 * 4] = y * z * omc - x * s;
		result.elements[2 + 2 * 4] = z * omc + c;

		result.elements[0 + 3 * 4] = 0.0f;
		result.elements[1 + 3 * 4] = 0.0f;
		result.elements[2 + 3 * 4] = 0.0f;
		result.elements[3 + 3 * 4] = 1.0f;
		return result;
	}

	Mat4 Mat4::scale(const Vec3& scale)
	{
		Mat4 result(1.0f);

		result.elements[0 + 0 * 4] = scale.x;
		result.elements[1 + 1 * 4] = scale.y;
		result.elements[2 + 2 * 4] = scale.z;

		return result;
	}
	Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
	{
		Vec3 f = (center - eye).normalized();
		Vec3 s = f.cross(up).normalized();
		Vec3 u = s.cross(f);

		Mat4 result(1.0f);
		result.elements[0] = s.x;
		result.elements[1] = u.x;
		result.elements[2] = -f.x;
		result.elements[3] = 0.0f;

		result.elements[4] = s.y;
		result.elements[5] = u.y;
		result.elements[6] = -f.y;
		result.elements[7] = 0.0f;

		result.elements[8] = s.z;
		result.elements[9] = u.z;
		result.elements[10] = -f.z;
		result.elements[11] = 0.0f;

		result.elements[12] = -s.dot(eye);
		result.elements[13] = -u.dot(eye);
		result.elements[14] = -f.dot(eye);  // some implementations use -f.dot(eye)
		result.elements[15] = 1.0f;

		return result;
	}
	Mat4 Mat4::inverse() const
	{
		Mat4 inv;
		const float* m = elements;

		inv.elements[0] = m[5] * m[10] * m[15] -
			m[5] * m[11] * m[14] -
			m[9] * m[6] * m[15] +
			m[9] * m[7] * m[14] +
			m[13] * m[6] * m[11] -
			m[13] * m[7] * m[10];

		inv.elements[4] = -m[4] * m[10] * m[15] +
			m[4] * m[11] * m[14] +
			m[8] * m[6] * m[15] -
			m[8] * m[7] * m[14] -
			m[12] * m[6] * m[11] +
			m[12] * m[7] * m[10];

		inv.elements[8] = m[4] * m[9] * m[15] -
			m[4] * m[11] * m[13] -
			m[8] * m[5] * m[15] +
			m[8] * m[7] * m[13] +
			m[12] * m[5] * m[11] -
			m[12] * m[7] * m[9];

		inv.elements[12] = -m[4] * m[9] * m[14] +
			m[4] * m[10] * m[13] +
			m[8] * m[5] * m[14] -
			m[8] * m[6] * m[13] -
			m[12] * m[5] * m[10] +
			m[12] * m[6] * m[9];

		inv.elements[1] = -m[1] * m[10] * m[15] +
			m[1] * m[11] * m[14] +
			m[9] * m[2] * m[15] -
			m[9] * m[3] * m[14] -
			m[13] * m[2] * m[11] +
			m[13] * m[3] * m[10];

		inv.elements[5] = m[0] * m[10] * m[15] -
			m[0] * m[11] * m[14] -
			m[8] * m[2] * m[15] +
			m[8] * m[3] * m[14] +
			m[12] * m[2] * m[11] -
			m[12] * m[3] * m[10];

		inv.elements[9] = -m[0] * m[9] * m[15] +
			m[0] * m[11] * m[13] +
			m[8] * m[1] * m[15] -
			m[8] * m[3] * m[13] -
			m[12] * m[1] * m[11] +
			m[12] * m[3] * m[9];

		inv.elements[13] = m[0] * m[9] * m[14] -
			m[0] * m[10] * m[13] -
			m[8] * m[1] * m[14] +
			m[8] * m[2] * m[13] +
			m[12] * m[1] * m[10] -
			m[12] * m[2] * m[9];

		inv.elements[2] = m[1] * m[6] * m[15] -
			m[1] * m[7] * m[14] -
			m[5] * m[2] * m[15] +
			m[5] * m[3] * m[14] +
			m[13] * m[2] * m[7] -
			m[13] * m[3] * m[6];

		inv.elements[6] = -m[0] * m[6] * m[15] +
			m[0] * m[7] * m[14] +
			m[4] * m[2] * m[15] -
			m[4] * m[3] * m[14] -
			m[12] * m[2] * m[7] +
			m[12] * m[3] * m[6];

		inv.elements[10] = m[0] * m[5] * m[15] -
			m[0] * m[7] * m[13] -
			m[4] * m[1] * m[15] +
			m[4] * m[3] * m[13] +
			m[8] * m[1] * m[7] -
			m[8] * m[3] * m[5];

		inv.elements[14] = -m[0] * m[5] * m[14] +
			m[0] * m[6] * m[13] +
			m[4] * m[1] * m[14] -
			m[4] * m[2] * m[13] -
			m[8] * m[1] * m[6] +
			m[8] * m[2] * m[5];

		inv.elements[3] = -m[1] * m[6] * m[11] +
			m[1] * m[7] * m[10] +
			m[5] * m[2] * m[11] -
			m[5] * m[3] * m[10] -
			m[9] * m[2] * m[7] +
			m[9] * m[3] * m[6];

		inv.elements[7] = m[0] * m[6] * m[11] -
			m[0] * m[7] * m[10] -
			m[4] * m[2] * m[11] +
			m[4] * m[3] * m[10] +
			m[8] * m[2] * m[7] -
			m[8] * m[3] * m[6];

		inv.elements[11] = -m[0] * m[5] * m[11] +
			m[0] * m[7] * m[9] +
			m[4] * m[1] * m[11] -
			m[4] * m[3] * m[9] -
			m[8] * m[1] * m[7] +
			m[8] * m[3] * m[5];

		inv.elements[15] = m[0] * m[5] * m[10] -
			m[0] * m[6] * m[9] -
			m[4] * m[1] * m[10] +
			m[4] * m[2] * m[9] +
			m[8] * m[1] * m[6] -
			m[8] * m[2] * m[5];

		float det = m[0] * inv.elements[0] + m[1] * inv.elements[4] +
			m[2] * inv.elements[8] + m[3] * inv.elements[12];

		if (det == 0)
			return Mat4(1.0f); // returns identity as fallback

		det = 1.0f / det;
		
		for (int i = 0; i < 16; i++)
			inv.elements[i] *= det;

		return inv;
	}
	std::string Mat4::toString() const
	{
		std::ostringstream ss;
		ss << std::fixed << std::setprecision(3); // Format for readability

		for (int row = 0; row < 4; row++)
		{
			ss << "[ ";
			for (int col = 0; col < 4; col++)
			{
				ss << elements[col * 4 + row] << " ";
			}
			ss << "]\n";
		}
		return ss.str();
	}
}