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
std::ostream & operator << (std::ostream & cout,const Vector3 &v) {
	return cout<<"("<<v.x<<","<<v.y<<","<<v.z<<")";
}
