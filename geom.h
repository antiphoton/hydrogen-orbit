struct Vector2;
struct Vector3;
struct Quaternion;
struct Vector2 {
	double x,y;
	Vector2(double x,double y);
	Vector2(const Vector2 &v);
};
std::ostream & operator << (std::ostream & cout,const Vector2 &v);
struct Vector3 {
	double x,y,z;
	Vector3(double x,double y,double z);
	Vector3(const Vector3 &v);
	double length() const;
};
std::ostream & operator << (std::ostream & cout,const Vector3 &v);
struct Quaternion {
	double w,x,y,z;
	Quaternion(double w,double x,double y,double z);
	Quaternion(const Vector3 &p,double a);
	Vector3 rotate(const Vector3 &p) const ;
};
Quaternion operator * (const Quaternion &q1,const Quaternion &q2);

