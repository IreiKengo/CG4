#pragma once
#include <cmath>

struct Vector3
{
	float x;
	float y;
	float z;
};


inline Vector3 operator+(Vector3 v, float s)
{
	v.x += s;
	v.y += s;
	v.z += s;

	return v;
}
inline Vector3 operator-(Vector3 v, float s)
{
	v.x -= s;
	v.y -= s;
	v.z -= s;

	return v;
}
inline Vector3 operator*(Vector3 v, float s)
{
	v.x *= s;
	v.y *= s;
	v.z *= s;

	return v;
}
inline Vector3 operator/(Vector3 v, float s)
{
	v.x /= s;
	v.y /= s;
	v.z /= s;

	return v;
}
inline Vector3& operator+=(Vector3& v, float s)
{

	v.x += s;
	v.y += s;
	v.z += s;

	return v;
}
inline Vector3& operator-=(Vector3& v, float s)
{

	v.x -= s;
	v.y -= s;
	v.z -= s;

	return v;
}
inline Vector3& operator*=(Vector3& v, float s)
{

	v.x *= s;
	v.y *= s;
	v.z *= s;

	return v;
}
inline Vector3& operator/=(Vector3& v, float s)
{

	v.x /= s;
	v.y /= s;
	v.z /= s;

	return v;
}

inline Vector3 operator+(float s,Vector3 v)
{
	v.x += s;
	v.y += s;
	v.z += s;

	return v;
}
inline Vector3 operator*(float s,Vector3 v)
{
	v.x *= s;
	v.y *= s;
	v.z *= s;

	return v;
}

inline Vector3 operator+(Vector3 a, const Vector3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;

	return a;

}
inline Vector3 operator-(Vector3 a, const Vector3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;

	return a;
}
inline Vector3 operator*(Vector3 a, const Vector3& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;

	return a;
}
inline Vector3 operator/(Vector3 a, const Vector3& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;

	return a;
}
inline Vector3& operator+=(Vector3& a, const Vector3& b)
{

	a.x += b.x;
	a.y += b.y;
	a.z += b.z;

	return a;
}
inline Vector3& operator-=(Vector3& a, const Vector3& b)
{

	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;

	return a;
}
inline Vector3& operator*=(Vector3& a, const Vector3& b)
{

	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;

	return a;
}
inline Vector3& operator/=(Vector3& a, const Vector3& b)
{

	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;

	return a;
}

// 内積
inline float Dot(const Vector3& v1, const Vector3& v2)
{
	float result;

	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

	return result;

}

inline Vector3 Normalize(const Vector3& v)
{
	float length = std::sqrt(Dot(v, v));
	return { v.x / length,v.y / length,v.z / length };
}