#ifndef _BASIC_MATRIX3_H_
#define _BASIC_MATRIX3_H_

#include <iostream>
#include "basic_x.h"
#include "os.h"
#include "vector3.h"
#include "vector4.h"
#include "string.h"

class Matrix4;
class Quaternion;

/// A Class for matrix math with 3 x 3 matrix.
class BASIC_API Matrix3 
{
	public:
// Data
		union 
		{
			float m_aafRowCol[3][3];
			float m_afData[9];	
		};

	   static const Matrix3 ZERO;
		static const Matrix3 IDENTITY;
		

// Constructors
		Matrix3(void) { };
		Matrix3(float f00,float f01,float f02,
				float f10,float f11,float f12,						
				float f20,float f21,float f22);
		
// Operators
		// assignment and comparison
		Matrix3& operator= (const Matrix3& rkMatrix);
		void operator= (const Matrix4& rkMatrix);
		void operator= (const Quaternion& rkQuaternion);
		
		// Comparison
		bool operator== (const Matrix3& rkMatrix) const;
		bool operator!= (const Matrix3& rkMatrix) const;

		// Arithmetic operations
		Matrix3 operator+ (const Matrix3& rkMatrix) const;
		Matrix3 operator+= (const Matrix3& rkMatrix);
		Matrix3 operator- (const Matrix3& rkMatrix) const;
		Matrix3 operator-= (const Matrix3& rkMatrix);
		Matrix3 operator* (const Matrix3& rkMatrix) const;
		Matrix3 operator*= (const Matrix3& rkMatrix);

		Matrix3 operator*(const float &f) const;
		Matrix3 operator*=(const float &f);		

// Methods
		void Zero();								// Set whole matrix to zero.
		void Identity();							// Set matrix to the identity matrix.

		void Transponse();
		Matrix3 GetTransponse() const;

		bool Inverse (Matrix3& inv, float tolerance) const;
		float Determinant(void)	 const;

		Vector3 VectorTransform (const Vector3& kVec) 
		{
			return Vector3 (
				kVec.x * m_aafRowCol[0][0] + kVec.y * m_aafRowCol[1][0] + kVec.z * m_aafRowCol[2][0],
				kVec.x * m_aafRowCol[0][1] + kVec.y * m_aafRowCol[1][1] + kVec.z * m_aafRowCol[2][1],
				kVec.x * m_aafRowCol[0][2] + kVec.y * m_aafRowCol[1][2] + kVec.z * m_aafRowCol[2][2]
				);
		}

		void Rotate(float fX, float fY, float fZ);
		void Rotate(Vector3 kRot);
		Vector3 GetRotVector();

// Accessors 
		Vector3 GetColumn (int iCol) const;
		
		inline Vector3 Row1() const { return Vector3 (m_aafRowCol[0][0], m_aafRowCol[0][1], m_aafRowCol[0][2]); }
		inline Vector3 Row2() const { return Vector3 (m_aafRowCol[1][0], m_aafRowCol[1][1], m_aafRowCol[1][2]); }
		inline Vector3 Row3() const { return Vector3 (m_aafRowCol[2][0], m_aafRowCol[2][1], m_aafRowCol[2][2]); }

		inline Vector3 Col1() const { return Vector3 (m_aafRowCol[0][0], m_aafRowCol[1][0], m_aafRowCol[2][0]); }
		inline Vector3 Col2() const { return Vector3 (m_aafRowCol[0][1], m_aafRowCol[1][1], m_aafRowCol[2][1]); }
		inline Vector3 Col3() const { return Vector3 (m_aafRowCol[0][2], m_aafRowCol[1][2], m_aafRowCol[2][2]); }

// Other
		void Print();


};


#endif





