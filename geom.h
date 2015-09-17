struct Vector2;
struct Vector3;
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
};
std::ostream & operator << (std::ostream & cout,const Vector3 &v);
