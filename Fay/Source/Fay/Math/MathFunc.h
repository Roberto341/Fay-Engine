#pragma once

#include <math.h>

namespace Fay
{
	inline float toRadians(float degrees)
	{
		return degrees * (M_PI / 180.0f);
	}
	inline float clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}
}