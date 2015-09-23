#pragma once
#include<iostream>
#include<vector>
struct Vector2;
struct Rect2;
struct Vector3;
struct Complex;
struct Quaternion;
class Factorial{
	public:
		Factorial();
		double operator[] (int n);
	private:
		std::vector<double> v;
};
extern Factorial factorial;
class Polynomial {
	public:
		Polynomial(int n=0);
		~Polynomial();
		int size() const;
		double operator [](int i) const;
		double & operator [](int i);
		Polynomial & operator = (const Polynomial &that);
		Polynomial operator >> (int d) const;
		Polynomial operator * (double k) const;
		Polynomial operator * (const Polynomial &that) const;
		double integrateExponential (double lambda) const;
	private:
		std::vector<double> data;
};
std::ostream & operator << (std::ostream & cout,const Polynomial &p);

struct Vector2 {
	double x,y;
	Vector2(double x,double y);
	Vector2(const Vector2 &v);
};
std::ostream & operator << (std::ostream & cout,const Vector2 &v);
struct Rect2 {
	Rect2(double left,double top,double right,double bottom);
	Vector2 min,max;
	double width() const;
	double height() const;
	Vector2 screenToClient(const Vector2 v) const;
};
struct Vector3 {
	double x,y,z;
	Vector3(double x=0,double y=0,double z=0);
	Vector3(const Vector3 &v);
	double length() const;
};
std::ostream & operator << (std::ostream & cout,const Vector3 &v);
struct Complex {
	double x,y;
	Complex (double x,double y);
	Complex (const Complex &c);
	double length() const;
	double angle() const;
	Complex operator ! () const;
};
Complex operator * (const Complex &c1,double c2);
Complex operator * (const Complex &c1,const Complex &c2);
Complex operator + (const Complex &c1,const Complex &c2);
std::ostream & operator << (std::ostream & cout,const Complex &c);
struct Quaternion {
	double w,x,y,z;
	Quaternion(double w,double x,double y,double z);
	Quaternion(const Vector3 &p,double a);
	Vector3 rotate(const Vector3 &p) const ;
};
Quaternion operator - (const Quaternion &q);
Quaternion operator * (const Quaternion &q1,const Quaternion &q2);

