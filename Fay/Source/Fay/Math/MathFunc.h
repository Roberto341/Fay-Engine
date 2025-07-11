#pragma once

#include <math.h>

namespace Fay
{
	inline float toRadians(float degrees)
	{
		return degrees * (M_PI / 180.0f);
	}
}