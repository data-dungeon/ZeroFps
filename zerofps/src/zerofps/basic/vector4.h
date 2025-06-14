#ifndef _BASIC_VECTOR4_H_
#define _BASIC_VECTOR4_H_

#include <iostream>
#include "basic_x.h"
#include "os.h"
#include "vector3.h"

class Matrix4;

using namespace std;

/// A Class for vector math operations in 4D.

class BASIC_API Vector4 {
	public:
		float x,y,z,w;
	
		Vector4() { }
		Vector4(float fX,float fY,float fZ,float fW) : x(fX), y(fY), z(fZ), w(fW) { }; 
		Vector4(const Vector4 &kOtherV4) : x(kOtherV4.x), y(kOtherV4.y), z(kOtherV4.z), w(kOtherV4.w) { }; 
		Vector4(const Vector3 &kOtherV3) : x(kOtherV3.x), y(kOtherV3.y), z(kOtherV3.z), w(1.0) { }; 
	
		// Assign
		Vector4& operator=(const Vector4 &kOtherV4) {
			x=kOtherV4.x;
			y=kOtherV4.y;
			z=kOtherV4.z;
			w=kOtherV4.w;	
			return *this;
			}

		Vector4& operator=(const Vector3 &kOtherV3) {
			x=kOtherV3.x;
			y=kOtherV3.y;
			z=kOtherV3.z;
			w=1;	
			return *this;
			}

		void Set(float fX, float fY, float fZ,float fW) {
			x = fX;
			y = fY;
			z = fZ;
			w = fW;	
			}
		
		float &operator[](const int i);
		
		// Arithmetic operations
		Vector4 operator+(const Vector4 &kOtherV4)	const {
			return Vector4(x+kOtherV4.x,y+kOtherV4.y,z+kOtherV4.z,w+kOtherV4.w);
			}

		Vector4 operator+(const float &fAdd)	const {
			return Vector4(x+fAdd,y+fAdd,z+fAdd,w+fAdd);
			}
	
		Vector4 operator-(const Vector4 &kOtherV4)	const {
			return Vector4(x-kOtherV4.x,y-kOtherV4.y,z-kOtherV4.z,w-kOtherV4.w);

			}

		Vector4 operator-(const float &fAdd)	const {
			return Vector4(x-fAdd,y-fAdd,z-fAdd,w-fAdd);
			}

		Vector4 operator*(const float &fOther)		const {
			return Vector4(x * fOther,y * fOther,z * fOther,w * fOther);
			}

		Vector4& operator+=(const Vector4 &kOtherV4) {
			x += kOtherV4.x;
			y += kOtherV4.y;
			z += kOtherV4.z;
			w += kOtherV4.w;	
			return *this;
			}

		Vector4& operator-=(const Vector4 &kOtherV4) {
			x -= kOtherV4.x;
			y -= kOtherV4.y;
			z -= kOtherV4.z;
			w -= kOtherV4.w;
			return *this;
			}

		Vector4& operator*=(const float &fOther) {
			x *= fOther;
			y *= fOther;
			z *= fOther;
			w *= fOther;
			return *this;
			}

		Vector4& operator/=(const float &fOther) {
			x /= fOther;
			y /= fOther;
			z /= fOther;
			w /= fOther;
			return *this;
			}
		
		Vector4 operator-() {
			return Vector4(-x,-y,-z,-w);
			}

		Vector4 operator*(const Matrix4 &f)	const;

		// Comparison
		bool operator==(const Vector4 &kOtherV3)	const;
		bool operator!=(const Vector4 &kOtherV3)	const;

		// Vector Operations.
		float Length(void)const;		
		void Normalize(void);		
		float Dot( const Vector4& v  )		const; 	

		float PlainLength(void)const;				
		void PlainNormalize(void);				

};



#endif
















