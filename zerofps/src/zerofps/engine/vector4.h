#ifndef _Vector4_H_
#define _Vector4_H_

#include <iostream>
//#include "Matrix4.h"

class Matrix4;

using namespace std;

class Vector4 {
	public:
		float x,y,z,w;
	
		Vector4(float fX,float fY,float fZ,float fW);
		Vector4(void);
	
		// Assign
		Vector4 operator=(const Vector4 &kOtherV3);		
		void set(float nx, float ny, float nz,float nw);			
		
		float &operator[](const int i);
		
		// Arithmetic operations
		Vector4 operator+(const Vector4 &kOtherV3)	const;
		Vector4 operator+(const float &fAdd)	const;		
		Vector4 operator-(const Vector4 &kOtherV3)	const;
		Vector4 operator-(const float &fAdd)	const;				
		Vector4 operator*(const float &fOther)		const;
		Vector4 operator*(const Matrix4 &f)	const;


		Vector4 operator+=(const Vector4 &kOtherV3);
		Vector4 operator-=(const Vector4 &kOtherV3);
		Vector4 operator*=(const float &fOther);
		Vector4 operator-();											

		// Comparison
		bool operator==(const Vector4 &kOtherV3)	const;
		bool operator!=(const Vector4 &kOtherV3)	const;

		float dot( const Vector4& v  )		const; 	
/*		
		// Vector operations.
		float length(void)const;				
		void normalize(void);						
		float dot( const Vector4& v  )		const; 	
		Vector4 unit(void)					const;						
		Vector4 cross( const Vector4& v )	const;	
		*/
		
		/*
		static const Vector4 ZERO;
		static const Vector4 AXIS_X;
		static const Vector4 AXIS_Y;
		static const Vector4 AXIS_Z;
		static const Vector4 AXIS_W;		
		*/
};



#endif
















