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
Rect2::Rect2(double left,double top,double right,double bottom):min(left,top),max(right,bottom) {
}
double Rect2::width() const {
	return max.x-min.x;
}
double Rect2::height() const {
	return max.y-min.y;
}
Vector2 Rect2::screenToClient(const Vector2 v) const {
	Vector2 ret(
		(v.x-min.x)/width(),
		(v.y-min.y)/height()
	);
	return ret;
}
Vector3::Vector3(double x,double y,double z):x(x),y(y),z(z) {
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
Complex::Complex(double x,double y):x(x),y(y) {
}
Complex::Complex(const Complex &c):x(c.x),y(c.y) {
}
double Complex::length() const {
	return sqrt(x*x+y*y);
}
double Complex::angle() const {
	return atan2(y,x);
}
Complex operator * (const Complex &c1,double c2) {
	return Complex(c1.x*c2,c1.y*c2);
}
Complex operator * (const Complex &c1,const Complex &c2) {
	return Complex(c1.x*c2.x-c1.y*c2.y,c1.x*c2.y+c1.y*c2.x);
}
std::ostream & operator << (std::ostream & cout,const Complex &c) {
	return cout<<c.x<<" + i "<<c.y;
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
}
Quaternion operator - (const Quaternion &q) {
	return Quaternion(q.w,-q.x,-q.y,-q.z);
}
Vector3 Quaternion::rotate(const Vector3 &p) const {
	Quaternion q(0,p.x,p.y,p.z);
	q=(*this*q)*(-*this);
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
