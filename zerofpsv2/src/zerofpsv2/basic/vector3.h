#ifndef _BASIC_VECTOR3_H_
#define _BASIC_VECTOR3_H_

#include <iostream>
#include <cmath>
#include "basic_x.h"
//#include "os.h"

using namespace std;

class Matrix3;

/**	\brief	A Class for vector math operations in 3D..
		\ingroup Basic
*/
class BASIC_API Vector3 {
	public:
// Std Values
		static const Vector3 ZERO;									
		static const Vector3 AXIS_X;								
		static const Vector3 AXIS_Y;								
		static const Vector3 AXIS_Z;								
		static const float degtorad;

// Data
		float x,y,z;                                
	
// Metoder
		Vector3() {	};											
		Vector3(float fX,float fY,float fZ) : x(fX), y(fY), z(fZ) { };


		// Assign
		Vector3& operator=(const Vector3 &kOtherV3) {
			x=kOtherV3.x;
			y=kOtherV3.y;
			z=kOtherV3.z;
			return *this;
			}	

		void Set(float fXIn, float fYIn, float fZIn) {
			x = fXIn;
			y = fYIn;
			z = fZIn;
			}

		float& operator[] (const int i);

		// Arithmetic operations
		Vector3 operator+(const Vector3 &kOtherV3)	const {
			return Vector3(x+kOtherV3.x,y+kOtherV3.y,z+kOtherV3.z);
			}

		Vector3 operator+(const float &fAdd)		const {
			return Vector3(x+fAdd,y+fAdd,z+fAdd);
			}

		Vector3 operator-(const Vector3 &kOtherV3)	const {
			return Vector3(x-kOtherV3.x,y-kOtherV3.y,z-kOtherV3.z);
			}

		Vector3 operator-(const float &fAdd)		const {
			return Vector3(x-fAdd,y-fAdd,z-fAdd);
			}

		Vector3 operator*(const float &fOther)		const {
			return Vector3(x * fOther,y * fOther,z * fOther);
			}
		
		
		float operator*(const Vector3 &kOther)		const {
			return Dot(kOther);
			}
		
//		Vector3 operator*(const Matrix4 &f)	const;
	
	
		Vector3 operator /  ( const float  s )	const {
			float invs = 1/s; 
			return Vector3( invs*x, invs*y, invs*z );
		}

		Vector3& operator+=(const Vector3 &kOtherV3) {
			x += kOtherV3.x;
			y += kOtherV3.y;
			z += kOtherV3.z;
			return *this;
			}

		Vector3& operator-=(const Vector3 &kOtherV3) {
			x -= kOtherV3.x;
			y -= kOtherV3.y;
			z -= kOtherV3.z;
			return *this;
			}

		Vector3& operator*=(const float &fOther) {
			x *= fOther;
			y *= fOther;
			z *= fOther;
			return *this;
			}

		Vector3& operator/=(const float fOther) {
			x /= fOther;
			y /= fOther;
			z /= fOther;
			return *this;
			}

		Vector3& operator /= ( const float& s )					
		{
			float r = 1 / s; x *= r; y *= r; z *= r; 
			return *this; 
		}

		Vector3 operator-() const {
			return Vector3(-x,-y,-z);
			}


		// Comparison
		bool operator==(const Vector3 &kOtherV3)	const;			// NC
		bool operator!=(const Vector3 &kOtherV3)	const;			// NC

		// Vector operations.
		float Length(void) const {
			return (float) sqrt( x*x + y*y + z*z );  
			}

		// Squared length of vector.
		float LengthSqr(void) const {
			return (float)( x*x + y*y + z*z );  
			}

		// Returns squaredlength length of vector.
		float SquaredLength(void) const {
			return x*x + y*y +z*z;  
			}

		void Normalize(void);												///< Normalize this to unit vector.
		float Dot( const Vector3& v  )		const; 					///< Return dot product between this vec and another one.
		Vector3 Unit(void)						const;					///< Return unit vector of this.		
		Vector3 Cross( const Vector3& v )	const;					///< Return cross product between this vector and another one.
		
		Vector3 PEP(const Vector3 &fOther) const;				//per element pruduct
	
	
		bool IsZero(void) const;									///< True if this = <0,0,0>
		bool NearlyEquals( const Vector3& v, const float e ) const;	///< True if this is == v within the range of +-e
		bool NearlyZero( const float e ) const; 					///< True if this is <0,0,0> within the range of +-e
		
		Vector3 Proj(const Vector3& v );									///< Projects v on this.
		Vector3 Perp(const Vector3& v );									///< Perp v on this.
		float Angle(const Vector3& v );									///< Angle (in rad) between two vectors. 
		void Abs(void);												///< Do a fabs() on coo of this vectors.
		int	AbsMaxCoo(void);										///< Returns Index för den coo med > fabs(value).
		void Lerp(Vector3& from, Vector3& to, float flerp);			///< Linjär interpolering mellan två vectors.
		
		Vector3 Angels(void);
		Vector3 AToU();												///< convert Angel Vector to a Unit Vector

      double DistanceTo (const Vector3& to);
      double DistanceXZTo (const Vector3& to);

		void Print(void);



};

BASIC_API Vector3 operator* ( const float& s, const Vector3& v );

BASIC_API Vector3 operator* (const Vector3& v, const Matrix3 m) ;
BASIC_API Vector3 operator* ( const Matrix3 m,const Vector3& v) ;

BASIC_API Vector3 operator/ (const Vector3& v, const Matrix3 m) ;
BASIC_API Vector3 operator/ ( const Matrix3 m,const Vector3& v) ;

#endif
















