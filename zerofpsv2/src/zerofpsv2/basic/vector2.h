#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#include <iostream>
#include <cmath>
#include "basic_x.h"
#include "os.h"

using namespace std;

/// A Class for vector math operations in 2D.
class BASIC_API Vector2 {
	private:
		
	
	public:
		float x,y;

		Vector2(float fX,float fY) : x(fX), y(fY){ };
		Vector2() {};

		// Assign
		Vector2& operator=(const Vector2 &kOtherV2) {
			x=kOtherV2.x;
			y=kOtherV2.y;
			return *this;
			}	

		void Set(float fXIn, float fYIn) {
			x = fXIn;
			y = fYIn;
			}

		// Arithmetic operations
		Vector2 operator+(const Vector2 &kOtherV3)	const {
			return Vector2(x+kOtherV3.x,y+kOtherV3.y);
			}
	
		Vector2 operator-(const Vector2 &kOtherV3)	const {
			return Vector2(x-kOtherV3.x,y-kOtherV3.y);
			}


		float Dot( const Vector2& v  ); 					///< Return dot product between this vec and another one.
		void Normalize(void);										///< Normalize this to unit vector.

		float Length(void) const {
			return (float) sqrt( x*x + y*y );  
			}

		Vector2 operator*(const float &fOther)		const {
			return Vector2(x * fOther,y * fOther);
			}

};

#endif



