#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "vector3.h"
#include "matrix4.h"
#include "matrix3.h"

//	A class for quaternions (4d num that describes rotation).
class BASIC_API Quaternion	{
public:
	//Vector3 vector;
	float	x, y, z, w;

// Construction and Destruction
	Quaternion();
	Quaternion( float fW, float fX, float fY, float fZ);
	Quaternion(const Quaternion& rkQ);

// Arithmetic operations
	Quaternion& operator= ( Quaternion& rkQ );
	Quaternion operator+ (const Quaternion& rkQ ) const;
	Quaternion operator- (const Quaternion& rkQ ) const;
	Quaternion operator+= (const Quaternion& rkQ );
	Quaternion operator-= (const Quaternion& rkQ );
	Quaternion operator* (const Quaternion& rkQ ) const;
	Quaternion operator*= (const Quaternion& rkQ );
    Quaternion operator* (float fScalar) const;
    friend Quaternion operator* (float fScalar, const Quaternion& rkQ);
    Quaternion operator- () const;

// Functions
	float Dot (const Quaternion& q) const;
	float Norm(void);
	float Length(void);
	Quaternion Inverse (void);
	Quaternion UnitInverse (void);
	void Normalize(void);

// Compare
	int operator == ( const Quaternion& rkQ ) const; 
	int operator != ( const Quaternion& rkQ ) const;

// conversion between quaternions, matrices, and angle-axes
    void FromRotationMatrix (const Matrix3& kRot);
    void ToRotationMatrix (Matrix3& kRot) const;
    void FromAngleAxis (const float& rfAngle, const Vector3& rkAxis);
    void ToAngleAxis (float& rfAngle, Vector3& rkAxis) const;
    void FromAxes (const Vector3* akAxis);
    void ToAxes (Vector3* akAxis) const;

    float& operator[] (int i);

	void AngleQuaternion( const Vector3 angles );
	void QuaternionSlerp( Quaternion* from, Quaternion* to, float t);

/*
	// Assign
	void operator = ( Matrix4& m );				// *



	float length(void);
	void  normalize(void);
	Quaternion conjugate(void);
	void make(Vector3 axis, float angle);
	void get(Vector3& axis, float& angle);

	void print(void);
	void rotate(float x, float y, float z);
	void assign_index(int index, float value);
*/

	static Quaternion ZERO;
	static Quaternion IDENTITY;
};

#endif
