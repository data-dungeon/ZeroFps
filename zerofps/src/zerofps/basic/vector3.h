#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include "basic_x.h"
#include <iostream>

using namespace std;

class BASIC_API Vector3 {
	public:
		float x,y,z;
	
		Vector3(float fX,float fY,float fZ);
		Vector3(void);
	
		// Assign
		Vector3 operator=(const Vector3 &kOtherV3);		
		void set(float nx, float ny, float nz);			
		
		// Arithmetic operations
		Vector3 operator+(const Vector3 &kOtherV3)	const;
		Vector3 operator+(const float &fAdd)	const;		
		Vector3 operator-(const Vector3 &kOtherV3)	const;
		Vector3 operator-(const float &fAdd)	const;				
		Vector3 operator*(const float &fOther)		const;

		Vector3 operator+=(const Vector3 &kOtherV3);
		Vector3 operator-=(const Vector3 &kOtherV3);
		Vector3 operator*=(const float &fOther);
		Vector3 operator-();											

		// Comparison
		bool operator==(const Vector3 &kOtherV3)	const;
		bool operator!=(const Vector3 &kOtherV3)	const;

		// Vector operations.
		float length(void)const;				
		void normalize(void);						
		float dot( const Vector3& v  )		const; 	
		Vector3 unit(void)					const;						
		Vector3 cross( const Vector3& v )	const;	
		
		static const Vector3 ZERO;
		static const Vector3 AXIS_X;
		static const Vector3 AXIS_Y;
		static const Vector3 AXIS_Z;
};



#endif
















