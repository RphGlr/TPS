#pragma once
#include <cmath>


union float2
{
	float e[2];
	struct { float x; float y; };
};
union float3
{
	float e[3];
	struct { float x; float y; float z; };
	float2 xy;
};
union float4
{
	float4() = default;
	float4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{}

	float4(float3 xyz, float w)
		: x(xyz.x), y(xyz.y), z(xyz.z), w(w)
	{}

	float e[4];
	struct { float x; float y; float z; float w; };
	struct { float r; float g; float b; float a; };
	float3 xyz;
	float2 xy;
};
union mat4x4
{
	float e[16];
	float4 c[4];
};

namespace Maths
{
	const float TAU = 6.283185307179586476925f;

	inline float cos(float x) { return cosf(x); }
	inline float sin(float x) { return sinf(x); }
	inline float tan(float x) { return tanf(x); }
	inline float toRadians(float degrees) { return degrees * TAU / 360.f; }


	template<typename T>
	inline T min(T x, T y) { return (x < y) ? x : y; };

	template<typename T>
	inline T max(T x, T y) { return (x > y) ? x : y; };

	template<typename T>
	inline T Clamp(T value, T a, T b)
	{
		return min(a, max(b, value));
	}

	inline mat4x4 Identity()
	{
		return {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	}
	inline mat4x4 Translate(float3 t)
	{
		return {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			t.x, t.y, t.z, 1.f,
		};
	}
	inline mat4x4 Scale(float3 scale)
	{
		return {
			scale.x, 0.f, 0.f, 0.f,
			0.f,scale.y, 0.f, 0.f,
			0.f, 0.f, scale.z, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	}

	mat4x4 frustum(float left, float right, float bottom, float top, float near, float far);
	mat4x4 perspective(float fovY, float aspect, float near, float far);
	mat4x4 rotateX(float angleRadians);
	mat4x4 rotateY(float angleRadians);
	mat4x4 rotateZ(float angleRadians);


#pragma region Mat4 fcts 

	inline float4 operator*(const mat4x4& m, float4 v)
	{
		float4 r;
		r.x = v.x * m.c[0].e[0] + v.y * m.c[1].e[0] + v.z * m.c[2].e[0] + v.w * m.c[3].e[0];
		r.y = v.x * m.c[0].e[1] + v.y * m.c[1].e[1] + v.z * m.c[2].e[1] + v.w * m.c[3].e[1];
		r.z = v.x * m.c[0].e[2] + v.y * m.c[1].e[2] + v.z * m.c[2].e[2] + v.w * m.c[3].e[2];
		r.w = v.x * m.c[0].e[3] + v.y * m.c[1].e[3] + v.z * m.c[2].e[3] + v.w * m.c[3].e[3];
		return r;
	}
	inline mat4x4 operator*(const mat4x4& a, const mat4x4& b)
	{
		mat4x4 res = {};
		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				for (int i = 0; i < 4; ++i)
					res.c[c].e[r] += a.c[i].e[r] * b.c[c].e[i];
		return res;
	}
	inline mat4x4& operator*=(mat4x4& a, const mat4x4& b)
	{
		a = a * b;
		return a;
	}
	inline bool operator==(float3 u, float3 v)
	{
		return (u.x == v.x && u.y == v.y && u.z == v.z);
	}

	inline float3 operator-(float3 a)
	{
		return { -a.x, -a.y, -a.z };
	}
	inline float3 operator-(float3 a, float3 b)
	{
		return { a.x - b.x, a.y - b.y, a.z - b.z };
	}
	inline float3 operator/(float3 v, float w)
	{
		return { v.x / w, v.y / w, v.z / w };
	}
	inline float3 operator*(float3 v, float w)
	{
		return { v.x * w, v.y * w, v.z * w };
	}
	inline float3 operator+(float3 u, float3 v)
	{
		return { u.x + v.x , u.y + v.y , u.z + v.z };
	}
	inline float3 operator+=(float3 u, float3 v)
	{
		return u + v;
	}
	inline float3 operator-=(float3 u, float3 v)
	{
		return u - v;
	}
	inline float3 CrossProduct(float3 v1, float3 v2)
	{
		return { v1.y * v2.z - v1.z * v2.y,
					 v1.z * v2.x - v1.x * v2.z,
					 v1.x * v2.y - v1.y * v2.x };
	}
	inline float3 Vector3RotateByQuaternion(float3 v, float4 q)
	{
		float3 result = { 0,0,0};

		result.x = v.x * (q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z) + v.y * (2 * q.x * q.y - 2 * q.w * q.z) + v.z * (2 * q.x * q.z + 2 * q.w * q.y);
		result.y = v.x * (2 * q.w * q.z + 2 * q.x * q.y) + v.y * (q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z) + v.z * (-2 * q.w * q.x + 2 * q.y * q.z);
		result.z = v.x * (-2 * q.w * q.y + 2 * q.x * q.z) + v.y * (2 * q.w * q.x + 2 * q.y * q.z) + v.z * (q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);

		return result;
	}
	inline float4 QuaternionFromEuler(float pitch, float yaw, float roll)
	{
		float4 q = { 0,0,0,0 };

		float x0 = cosf(pitch * 0.5f);
		float x1 = sinf(pitch * 0.5f);
		float y0 = cosf(yaw * 0.5f);
		float y1 = sinf(yaw * 0.5f);
		float z0 = cosf(roll * 0.5f);
		float z1 = sinf(roll * 0.5f);

		q.x = x1 * y0 * z0 - x0 * y1 * z1;
		q.y = x0 * y1 * z0 + x1 * y0 * z1;
		q.z = x0 * y0 * z1 - x1 * y1 * z0;
		q.w = x0 * y0 * z0 + x1 * y1 * z1;

		return q;
	}

	inline float DotProduct(float3 a, float3 b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}
	inline float Norm(float3 a)
	{
		return sqrtf((a.x * a.x + a.y * a.y + a.z * a.z));
	}
	inline float3 Normalize(float3 a)
	{
		if (a == float3{ 0,0,0 })
			return a;

		float norm = Norm(a);
		a.x /= norm;
		a.y /= norm;
		a.z /= norm;

		return a;
	}
	inline float Distance(float3 a, float3 b)
	{
		return Norm(a - b);
	}

	inline float2 rotateByAngle(float2 v, float angle)
	{
		angle *= 0.0174533; // degree to rad
		return float2{ cos(angle) * v.x - sin(angle) * v.y, sin(angle) * v.x + cos(angle) * v.y };
	}
#pragma endregion
}