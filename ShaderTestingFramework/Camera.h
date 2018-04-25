#ifndef KAMERA_H
#define KAMERA_H

#include "Util.h"

class Camera {
private :
	Point3D eye, lookat;
	Vector3D up;
	Vector3D u, v, n;
	double viewAngle, aspect, nearDist, farDist;
	void set();
public :
	void setModelViewMatrix();
	Camera();
	Vector3D getU() {return u;}
	void setPerspective(float vAng, float asp, float nearD, float farD);
	void set(Point3D eye, Point3D lookat, Vector3D up);
	void setEye(const Point3D & eye);
	Point3D getEye() {return eye;};
	void setLookat(const Point3D & lookat);
	Point3D getLookat() {return lookat;}
	void roll(float angle);
	void pitch(float angle);
	void yaw(float angle);
	void slide(float delU, float delV, float delN);
};



#endif