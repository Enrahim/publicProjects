#ifndef MATTEVERKTOY_H
#define MATTEVERKTOY_H

#include <cmath>

const double pi = 4.0 * atan(1.0);

struct Vector3D {
	float x, y, z;
	Vector3D(float dx = 0, float dy = 0, float dz = 0)
		{x = dx; y = dy; z = dz;}

	friend Vector3D operator*(const float c, const Vector3D & v)
	{
		Vector3D res;
		res.x = c * v.x;
		res.y = c * v.y;
		res.z = c * v.z;
		return res;
	}
	Vector3D operator*(const float c) const
	{
		Vector3D res;
		res.x = c * x;
		res.y = c * y;
		res.z = c * z;
		return res;
	}

	Vector3D operator+(const Vector3D & v) const
	{
		Vector3D res;
		res.x = x + v.x;
		res.y = y + v.y;
		res.z = z + v.z;
		return res;
	}

	void normalize() 
		{float nInv = 1.0 / sqrt(x * x + y * y + z * z); 
			x *= nInv; y *= nInv; z *= nInv;}
	Vector3D cross(const Vector3D & v) const 
	{	
		Vector3D res;
		res.x = y * v.z - z * v.y;
		res.y = -x * v.z + z * v.x;
		res.z = x * v.y - y * v.x;
		return res;
	}
	float dot(const Vector3D & v) const
		{return x * v.x + y * v.y + z * v.z;}
};

struct Point3D {
	float x, y, z;
	Point3D(float dx = 0, float dy = 0, float dz = 0)
		{x = dx; y = dy; z = dz;}
	void set(float dx, float dy, float dz)
		{x = dx; y = dy; z = dz;}
	void set(const Point3D & p)
		{x = p.x; y = p.y; z = p.z;}
	void build4tuple(float v[])
	{v[0] = x; v[1] = y; v[2] = z; v[3] = 1;}
	// affine sum
	Point3D operator+(const Point3D & pt) const
	{	
		Point3D p;
		p.x = x + pt.x;
		p.y = y + pt.y;
		p.z = z + pt.z;
		return p;
	}
	Point3D operator+(const Vector3D & v) const
	{	
		Point3D p;
		p.x = x + v.x;
		p.y = y + v.y;
		p.z = z + v.z;
		return p;
	}
	Point3D operator-(const Vector3D & v) const
	{	
		Point3D p;
		p.x = x - v.x;
		p.y = y - v.y;
		p.z = z - v.z;
		return p;
	}
	Vector3D operator-(const Point3D & pt) const
	{	
		Vector3D v;
		v.x = x - pt.x;
		v.y = y - pt.y;
		v.z = z - pt.z;
		return v;
	}
	friend Point3D operator*(const float m, const Point3D pt) 
	{	
		Point3D pRet;
		pRet.x = m * pt.x;
		pRet.y = m * pt.y;
		pRet.z = m * pt.z;
		return pRet;
	}
};

#endif