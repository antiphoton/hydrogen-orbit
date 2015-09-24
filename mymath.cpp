#include<math.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include"mymath.h"
Factorial::Factorial() {
	v.push_back(1);
}
double Factorial::operator [] (int n) {
	int s=v.size();
	if (n>=s) {
		int i;
		for (i=s;i<=n;i++) {
			v.push_back(v.back()*i);
		}
	}
	return v[n];
}
Factorial factorial;
Polynomial::Polynomial(int n):data(n) {
}
Polynomial::~Polynomial() {
}
int Polynomial::size() const {
	return data.size();
}
double Polynomial::operator [](int i) const {
	return data[i];
}
double & Polynomial::operator [](int i) {
	return data[i];
}
Polynomial & Polynomial::operator = (const Polynomial &that) {
	data=that.data;
	return *this;
}
Polynomial Polynomial::operator >> (int d) const {
	int n=data.size();
	Polynomial p(n-d);
	int i;
	for (i=n-1;i>=d;i--) {
		p[i-d]=data[i]*factorial[i]/factorial[i-d];
	}
	return p;
}
Polynomial Polynomial::operator * (double k) const {
	int n=data.size();
	Polynomial p(n);
	int i;
	for (i=0;i<n;i++) {
		p[i]=data[i]*k;
	}
	return p;
}
Polynomial Polynomial::operator * (const Polynomial &that) const {
	const Polynomial &x1=*this,x2=that;
	int n1=x1.data.size();
	int n2=x2.data.size();
	Polynomial y(n1+n2-1);
	int i,j;
	for (i=0;i<n1;i++) {
		for (j=0;j<n2;j++) {
			y.data[i+j]+=x1.data[i]*x2.data[j];
		}
	}
	return y;
}
double Polynomial::integrateExponential(double lambda) const {
	int n=data.size();
	double k=1;
	double y=0;
	int i;
	for (i=0;i<n;i++) {
		k/=lambda;
		y+=data[i]*factorial[i]*k;
	}
	return y;
}
std::ostream & operator << (std::ostream & cout,const Polynomial &p) {
	bool zero=true;
	int n=p.size();
	int i;
	for (i=n-1;i>=0;i--) {
		double k=p[i];
		if (k==0) {
			continue;
		}
		if (!zero||k<0) {
			cout<<' '<<(k>0?'+':'-')<<' ';
		}
		zero=false;
		k=fabs(k);
		if (k!=1) {
			cout<<k<<' ';
		}
		if (i>0) {
			cout<<'x';
		}
		else {
			if (k==1) {
				cout<<'1';
			}
		}
		if (i>1) {
			cout<<'^'<<i;
		}
	}
	if (zero) {
		cout<<'0';
	}
	return cout;
}

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
Complex Complex::operator ! () const {
	return Complex(x,-y);
}
Complex operator * (double c1,const Complex &c2) {
	return Complex(c1*c2.x,c1*c2.y);
}
Complex operator * (const Complex &c1,double c2) {
	return Complex(c1.x*c2,c1.y*c2);
}
Complex operator * (const Complex &c1,const Complex &c2) {
	return Complex(c1.x*c2.x-c1.y*c2.y,c1.x*c2.y+c1.y*c2.x);
}
Complex operator + (const Complex &c1,const Complex &c2) {
	return Complex(c1.x+c2.x,c1.y+c2.y);
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
