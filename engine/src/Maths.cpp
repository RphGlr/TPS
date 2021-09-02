#include "Maths.h"

mat4x4 Maths::frustum(float left, float right, float bottom, float top, float near, float far)
{
	return
	{
		(near * 2.f) / (right - left),   0.f,                              0.f,                               0.f,
		0.f,                             (near * 2.f) / (top - bottom),    0.f,                               0.f,
		(right + left) / (right - left), (top + bottom) / (top - bottom), -(far + near) / (far - near),      -1.f,
		0.f,                             0.f,                             -(far * near * 2.f) / (far - near), 0.f
	};
}

mat4x4 Maths::perspective(float fovY, float aspect, float near, float far)
{
	float top = near * Maths::tan(fovY * 0.5f);
	float right = top * aspect;
	return Maths::frustum(-right, right, -top, top, near, far);
}

mat4x4 Maths::rotateX(float angleRadians)
{
	float c = Maths::cos(angleRadians);
	float s = Maths::sin(angleRadians);
	return
	{
		1.f, 0.f, 0.f, 0.f,
		0.f,   c,   s, 0.f,
		0.f,  -s,   c, 0.f,
		0.f, 0.f, 0.f, 1.f,
	};
}

mat4x4 Maths::rotateY(float angleRadians)
{
	float c = Maths::cos(angleRadians);
	float s = Maths::sin(angleRadians);
	return
	{
		  c, 0.f,  -s, 0.f,
		0.f, 1.f, 0.f, 0.f,
		  s, 0.f,   c, 0.f,
		0.f, 0.f, 0.f, 1.f,
	};
}

mat4x4 Maths::rotateZ(float angleRadians)
{
	float _cos = Maths::cos(angleRadians);
	float _sin = Maths::sin(angleRadians);

	mat4x4 matrix = {
		_cos    , _sin  , 0.f  , 0.f,
		-_sin   , _cos  , 0.f  , 0.f,
		0.f     , 0.f   , 1.f  , 0.f,
		0.f     , 0.f   , 0.f  , 1.f
	};

	return matrix;
}
