#include<math.h>
#include<stdio.h>
#include<iostream>
#include"geom.h"
Vector2::Vector2(double x=0,double y=0):x(x),y(y) {
}
Vector2::Vector2(const Vector2 &v) {
	x=v.x;
	y=v.y;
}
std::ostream & operator << (std::ostream & cout,const Vector2 &v) {
	return cout<<"("<<v.x<<","<<v.y<<")";
}
Vector3::Vector3(double x=0,double y=0,double z=0):x(x),y(y),z(z) {
}
Vector3::Vector3(const Vector3 &v) {
	x=v.x;
	y=v.y;
	z=v.z;
}
double Vector3::length() const {
	return sqrt(x*x+y*y+z*z);
}
std::ostream & operator << (std::ostream & cout,const Vector3 &v) {
	return cout<<"("<<v.x<<","<<v.y<<","<<v.z<<")";
}
Quaternion::Quaternion(double w,double x,double y,double z):w(w),x(x),y(y),z(z) {
}
Quaternion::Quaternion(const Vector3 &p,double a) {
	a/=2;
	w=cos(a);
	double s=sin(a)/p.length();
	x=s*p.x;
	y=s*p.y;
	z=s*p.z;
	printf("%f %f %f %f\n",w,x,y,z);
}
Vector3 Quaternion::rotate(const Vector3 &p) const {
	Quaternion q(0,p.x,p.y,p.z);
	q=(*this*q)*Quaternion(w,-x,-y,-z);
	return Vector3(
		q.x,
		q.y,
		q.z
	);
}
Quaternion operator * (const Quaternion &q1,const Quaternion &q2) {
	return Quaternion(
		q1.w*q2.w-q1.x*q2.x-q1.y*q2.y-q1.z*q2.z,
		q1.w*q2.x+q1.x*q2.w+q1.y*q2.z-q1.z*q2.y,
		q1.w*q2.y+q1.y*q2.w+q1.z*q2.x-q1.x*q2.z,
		q1.w*q2.z+q1.z*q2.w+q1.x*q2.y-q1.y*q2.x
	);
}
