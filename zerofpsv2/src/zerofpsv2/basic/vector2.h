#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#include <iostream>
#include <cmath>
#include "basic_x.h"
//#include "os.h"

using namespace std;

/**	\brief	A Class for vector math operations in 2D.
		\ingroup Basic
*/
class BASIC_API Vector2
{
	public:
// Std Values
		static const Vector2 ZERO;									
		static const Vector2 AXIS_X;								
		static const Vector2 AXIS_Y;								
	
// Data
		float x,y;

// Constructors
		Vector2() { };
		Vector2(float fX, float fY) : x(fX), y(fY) { };
		Vector2(const Vector2& kIn);

// Operators
		float& operator[] (const int i);

		Vector2& operator=(const Vector2 &kOtherV2) {
			x=kOtherV2.x;
			y=kOtherV2.y;
			return *this;
			}	

		Vector2 operator+(const Vector2 &kOtherV3)	const {
			return Vector2(x+kOtherV3.x,y+kOtherV3.y);
			}

		Vector2& operator+=(const Vector2 &kOtherV3) {
			x += kOtherV3.x;
			y += kOtherV3.y;
			return *this;
			}
		

		Vector2 operator-(const Vector2 &kOtherV3)	const {
			return Vector2(x-kOtherV3.x,y-kOtherV3.y);
			}

		Vector2& operator-=(const Vector2 &kOtherV3) {
			x -= kOtherV3.x;
			y -= kOtherV3.y;
			return *this;
			}

		Vector2 operator*(const float &fOther)		const {
			return Vector2(x * fOther,y * fOther);
			}

		Vector2& operator*=(const float &fOther) {
			x *= fOther;
			y *= fOther;
			return *this;
			}

		bool operator==(const Vector2 &kOtherV3)	const;			
		bool operator!=(const Vector2 &kOtherV3)	const;		

		Vector2 operator-() {
			return Vector2(-x,-y);
			}

// Methods
		void Set(float fXIn, float fYIn) {
			x = fXIn;
			y = fYIn;
			}

		float Length(void) const {
			return (float) sqrt( x*x + y*y );  
			}

		void Normalize(void);										///< Normalize this to unit vector.
		float Dot( const Vector2& v  ); 					///< Return dot product between this vec and another one.
		bool IsZero() const;
};

#endif



