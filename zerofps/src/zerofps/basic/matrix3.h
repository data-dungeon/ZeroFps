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

class BASIC_API Matrix3 {
	private:	
	
	public:
		union 
		{
			float m_aafRowCol[4][4];
			float m_afData[16];	
		};

	    static const Matrix3 ZERO;
		static const Matrix3 IDENTITY;


		Matrix3(void) { };
		Matrix3(float f00,float f01,float f02,
				float f10,float f11,float f12,						
				float f20,float f21,float f22);
		
						
		Vector3 Matrix3::GetColumn (int iCol) const;

		// assignment and comparison
		Matrix3& Matrix3::operator= (const Matrix3& rkMatrix);
		bool Matrix3::operator== (const Matrix3& rkMatrix) const;
		bool Matrix3::operator!= (const Matrix3& rkMatrix) const;

		void Matrix3::operator= (const Matrix4& rkMatrix);
		void Matrix3::operator= (const Quaternion& rkQuaternion);

		
		// Arithmetic operations
		Matrix3 Matrix3::operator+ (const Matrix3& rkMatrix) const;
		Matrix3 Matrix3::operator+= (const Matrix3& rkMatrix);
		Matrix3 Matrix3::operator- (const Matrix3& rkMatrix) const;
		Matrix3 Matrix3::operator-= (const Matrix3& rkMatrix);
		Matrix3 Matrix3::operator* (const Matrix3& rkMatrix) const;
		Matrix3 Matrix3::operator*= (const Matrix3& rkMatrix);

};


#endif





