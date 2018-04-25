
#include <gl/glut.h>
#include "Camera.h"

Camera::Camera()
{
	up = Vector3D(0, 1, 0);
}

void Camera::setModelViewMatrix()
{
	float m[16];
	Vector3D eVec(eye.x, eye.y, eye.z);//Vektorversjon av eye
	m[0] = u.x; 
	m[4] = u.y;
	m[8] = u.z;
	m[12] = -eVec.dot(u);
	m[1] = v.x; 
	m[5] = v.y;
	m[9] = v.z;
	m[13] = -eVec.dot(v);
	m[2] = n.x; 
	m[6] = n.y;
	m[10] = n.z;
	m[14] = -eVec.dot(n);
	m[3] = 0; 
	m[7] = 0;
	m[11] = 0;
	m[15] = 1.0;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m);//Load OpenGLs modelview-matrise	
}

void Camera::set()
{
	u = up.cross(n);
	n.normalize();
	u.normalize();
	v = n.cross(u);
}

void Camera::set(Point3D _eye, Point3D _lookat, Vector3D _up)
{
	eye = _eye;
	lookat = _lookat;
	up = _up;
	n = Vector3D(eye.x - lookat.x, eye.y - lookat.y, eye.z - lookat.z);
	set();
	setModelViewMatrix();
}

void Camera::setEye(const Point3D & _eye)
{
	eye = _eye;
	n = Vector3D(eye.x - lookat.x, eye.y - lookat.y, eye.z - lookat.z);
	set();
	setModelViewMatrix();
}

void Camera::setLookat(const Point3D & _lookat)
{
	lookat = _lookat;
	n = Vector3D(eye.x - lookat.x, eye.y - lookat.y, eye.z - lookat.z);
	set();
	setModelViewMatrix();
}

void Camera::setPerspective(float vAng, float asp, float nearD, float farD)
{
	viewAngle = vAng;
	aspect = asp;
	nearDist = nearD;
	farDist = farD;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(viewAngle, aspect, nearDist, farDist);
}

void Camera::slide(float delU, float delV, float delN)
{
	eye.x += delU * u.x + delV * v.x + delN * n.x;
	eye.y += delU * u.y + delV * v.y + delN * n.y;
	eye.z += delU * u.z + delV * v.z + delN * n.z;
	setModelViewMatrix();
}

void Camera::roll(float angle)
{
	angle *= pi / 180;
	Vector3D t = u;
	u = cos(angle) * t + sin(angle) * v;
	v = -sin(angle) * t + cos(angle) * v;
	setModelViewMatrix();
}

void Camera::pitch(float angle)
{
	angle *= pi / 180;
	Vector3D t = v;
	v = cos(angle) * t + sin(angle) * n;
	n = -sin(angle) * t + cos(angle) * n;
	setModelViewMatrix();
}

void Camera::yaw(float angle)
{
	angle *= pi / 180;
	Vector3D t = n;
	n = cos(angle) * t + sin(angle) * u;
	u = -sin(angle) * t + cos(angle) * u;
	setModelViewMatrix();
}
