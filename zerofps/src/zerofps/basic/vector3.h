#ifndef _BASIC_VECTOR3_H_
#define _BASIC_VECTOR3_H_

#include <iostream>
#include "basic_x.h"
#include "os.h"

using namespace std;

class BASIC_API Vector3 {
	public:
		float x,y,z;												// NC
	
		Vector3(float fX,float fY,float fZ);						// NC
		Vector3() { /*x=0; y=0; z=0;*/ };												// NC

		// Assign
		Vector3 operator=(const Vector3 &kOtherV3);					// NC
		void Set(float fXIn, float fYIn, float fZIn);				// NC
		
		// Arithmetic operations
		Vector3 operator+(const Vector3 &kOtherV3)	const;			// NC
		Vector3 operator+(const float &fAdd)		const;			// NC
		Vector3 operator-(const Vector3 &kOtherV3)	const;			// NC
		Vector3 operator-(const float &fAdd)		const;			// NC
		Vector3 operator*(const float &fOther)		const;			// NC

		Vector3 operator+=(const Vector3 &kOtherV3);				// NC
		Vector3 operator-=(const Vector3 &kOtherV3);				// NC
		Vector3 operator*=(const float &fOther);					// NC
		Vector3 operator/=(const float fOther);
		Vector3 operator-();										// NC	

		// Comparison
		bool operator==(const Vector3 &kOtherV3)	const;			// NC
		bool operator!=(const Vector3 &kOtherV3)	const;			// NC

		// Vector operations.
		float Length(void)const;									// NC
		float LengthSqr(void)const;									// Squared length of vector.

		void Normalize(void);										// NC
		float Dot( const Vector3& v  )		const; 					// NC
		Vector3 Unit(void)					const;					// NC		
		Vector3 Cross( const Vector3& v )	const;					// NC
		
		static const Vector3 ZERO;									// NC
		static const Vector3 AXIS_X;								// NC
		static const Vector3 AXIS_Y;								// NC
		static const Vector3 AXIS_Z;								// NC
		static const float degtorad;
	

		// ************************************************



		bool IsZero(void) const;									// True if this = <0,0,0>
		bool NearlyEquals( const Vector3& v, const float e ) const;	// True if this is == v within the range of +-e
		bool NearlyZero( const float e ) const; 					// True if this is <0,0,0> within the range of +-e
		
		Vector3  operator /  ( const float  s )	const;					// Div with scalar
		Vector3& operator /= ( const float& s );						// Div = with scalar
		float SquaredLength(void) const;				// Returns squaredlength length of vector.
		Vector3 Proj( Vector3& v );					// Projects v on this.
		Vector3 Perp( Vector3& v );					// Perp v on this.
		float Angle( Vector3& v );					// Angle (in rad) between two vectors. 
		void Abs(void);									// Do a fabs() on coo of this vectors.
		int	AbsMaxCoo(void);	// Returns Index för den coo med > fabs(value).
		void Lerp(Vector3& from, Vector3& to, float flerp);		// Linjär interpolering mellan två vectors.
		
		Vector3 Angels(void);

//		friend void operator<<(ostream& os, const Vector3 &v);
//		friend Vector3 operator * ( const float& s, const Vector3& v );
		void Print(void);

		float& Vector3::operator[] (int i);
};


BASIC_API Vector3 operator * ( const float& s, const Vector3& v );
//BASIC_API ostream& operator<<(ostream& os, const Vector3 &v);

#endif
















