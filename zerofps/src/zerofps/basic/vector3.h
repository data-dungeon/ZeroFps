#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include <iostream>

using namespace std;
#include "basic_x.h"

class BASIC_API Vector3 {
	public:
		float x,y,z;
	
		Vector3(float fX,float fY,float fZ);
		Vector3(void);
	
		// Assign
		Vector3 operator=(const Vector3 &kOtherV3);		
		void Set(float nx, float ny, float nz);			
		
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
		float Length(void)const;				
		void Normalize(void);						
		float Dot( const Vector3& v  )		const; 	
		Vector3 Unit(void)					const;						
		Vector3 Cross( const Vector3& v )	const;	
		
		static const Vector3 ZERO;
		static const Vector3 AXIS_X;
		static const Vector3 AXIS_Y;
		static const Vector3 AXIS_Z;
		
};



#endif
















