#ifndef _BASIC_QUATERNION_H_
#define _BASIC_QUATERNION_H_

#include "basic_x.h"
//#include "os.h"
#include "vector3.h"
#include "matrix4.h"
#include "matrix3.h"

/**	\brief	A class for quaternions (4d num that describes rotation)
		\ingroup Basic
*/
class BASIC_API Quaternion	
{
public:
	//Vector3 vector;
	float	x, y, z, w;

// Construction and Destruction
	Quaternion();
	Quaternion( float fW, float fX, float fY, float fZ);
	Quaternion(const Quaternion& rkQ);

// Arithmetic operations
	Quaternion& operator= (const Quaternion& rkQ )
	{
	    w = rkQ.w;
   	 x = rkQ.x;
	    y = rkQ.y;
   	 z = rkQ.z;
		return *this;	
	}
	Quaternion operator+ (const Quaternion& rkQ ) const;
	Quaternion operator- (const Quaternion& rkQ ) const;
	Quaternion operator+= (const Quaternion& rkQ );
	Quaternion operator-= (const Quaternion& rkQ );
	Quaternion operator* (const Quaternion& rkQ ) const;
	Quaternion operator*= (const Quaternion& rkQ );
   Quaternion operator* (float fScalar) const;
   friend Quaternion operator* (float fScalar, const Quaternion& rkQ);
   Quaternion operator- () const;
    
	Quaternion operator/=(float s);

// Functions
	void Set( float fW, float fX, float fY, float fZ);	
	
	float Dot (const Quaternion& q) const;
	float Norm(void);
	float Length(void);
	Quaternion Inverse (void);
	Quaternion UnitInverse (void);
	void Normalize(void);
	
	Vector3 RotateVector3(Vector3 v);

// Compare
	int operator == ( const Quaternion& rkQ ) const; 
	int operator != ( const Quaternion& rkQ ) const;

// conversion between quaternions, matrices, and angle-axes
    void FromRotationMatrix (const Matrix3& kRot);
    void ToRotationMatrix (Matrix3& kRot) const;
 
    void FromRotationMatrix (const Matrix4& kRot);
    void ToRotationMatrix (Matrix4& kRot) const;
	 
	 void FromAngleAxis (const float& rfAngle, const Vector3& rkAxis);
    void ToAngleAxis (float& rfAngle, Vector3& rkAxis) const;
    void FromAxes (const Vector3* akAxis);
    void ToAxes (Vector3* akAxis) const;

    float& operator[] (int i);

	void AngleQuaternion( const Vector3 angles );
	Vector3 GetVector3();
	void QuaternionSlerp( Quaternion* from, Quaternion* to, float t);
	Quaternion conjugate(void);

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

BASIC_API Quaternion operator*(Quaternion q,Vector3 v);
BASIC_API Quaternion operator*(Vector3 v,Quaternion q);

#endif
