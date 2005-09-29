#ifndef _BASIC_MATRIX3_H_
#define _BASIC_MATRIX3_H_

#include <iostream>
#include "basic_x.h"
#include "string.h"
#include "math.h"

class Matrix4;
class Quaternion;

/**	\brief	A Class for matrix math with 3 x 3 matrix.
		\ingroup Basic
*/
class BASIC_API Matrix3 
{
	public:
// Std Values
	   static const Matrix3 ZERO;
		static const Matrix3 IDENTITY;

// Data
		union 
		{
			float m_aafRowCol[3][3];
			float m_afData[9];	
		};

// Constructors
		Matrix3(void) { };
		Matrix3(float f00,float f01,float f02,
					float f10,float f11,float f12,						
					float f20,float f21,float f22);

// Access 
		Vector3 GetRow (int iRow) const;
		void SetRow(int iRow,const Vector3& kVec);
		Vector3 GetColumn (int iCol) const;
		void SetColum(int iCol,const Vector3& kVec);
		
		Vector3 GetAxis(int iAxisNum);
		void SetAxis(int iAxisNum, const Vector3& kNewAxis);

		inline Vector3 Row1() const { return Vector3 (m_aafRowCol[0][0], m_aafRowCol[0][1], m_aafRowCol[0][2]); }
		inline Vector3 Row2() const { return Vector3 (m_aafRowCol[1][0], m_aafRowCol[1][1], m_aafRowCol[1][2]); }
		inline Vector3 Row3() const { return Vector3 (m_aafRowCol[2][0], m_aafRowCol[2][1], m_aafRowCol[2][2]); }

		inline Vector3 Col1() const { return Vector3 (m_aafRowCol[0][0], m_aafRowCol[1][0], m_aafRowCol[2][0]); }
		inline Vector3 Col2() const { return Vector3 (m_aafRowCol[0][1], m_aafRowCol[1][1], m_aafRowCol[2][1]); }
		inline Vector3 Col3() const { return Vector3 (m_aafRowCol[0][2], m_aafRowCol[1][2], m_aafRowCol[2][2]); }

// Assignment 
		Matrix3& operator= (const Matrix3& rkMatrix);
		void operator= (const Matrix4& rkMatrix);
		void operator= (const Quaternion& rkQuaternion);
		void Set(float f00,float f01,float f02,
					float f10,float f11,float f12,						
					float f20,float f21,float f22);
		void Zero();								// Set whole matrix to zero.
		void Identity();							// Set matrix to the identity matrix.
		
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

// Matrix/Vector 
		const Vector3& VectorTransform (const Vector3& kVec) const;

// Common Matrix Methods
		void Transponse();
		Matrix3 GetTransponse() const;
		bool Inverse (Matrix3& inv, float tolerance) const;
		float Determinant(void)	 const;

// Other
		string ToString();

// NoSort
		void Scale(float fX, float fY, float fZ);
		void Scale(const Vector3& kScale);		
		void RadRotate(float fX, float fY, float fZ);
		void RadRotate(const Vector3& kRot);
		void Rotate(float fX, float fY, float fZ);
		void Rotate(const Vector3& kRot);
		Vector3 GetRotVector();
};

#include "matrix4.h"

// Constructors
inline Matrix3::Matrix3(float f00,float f01,float f02,
				float f10,float f11,float f12,						
				float f20,float f21,float f22)
{							 	 
	m_aafRowCol[0][0] = f00;
	m_aafRowCol[0][1] = f01;
	m_aafRowCol[0][2] = f02;

	m_aafRowCol[1][0] = f10;
	m_aafRowCol[1][1] = f11;
	m_aafRowCol[1][2] = f12;

	m_aafRowCol[2][0] = f20;
	m_aafRowCol[2][1] = f21;
	m_aafRowCol[2][2] = f22;
};

// Access 
inline Vector3 Matrix3::GetRow (int iRow) const
{
	ZFAssert(0 <= iRow && iRow < 3, "Matrix3::GetRow: Index out of range");
	return Vector3(m_aafRowCol[iRow][0], m_aafRowCol[iRow][1], m_aafRowCol[iRow][2]);
}

inline void Matrix3::SetRow(int iRow,const Vector3& kVec)
{
	ZFAssert(0 <= iRow && iRow < 3, "Matrix3::SetRow: Index out of range");
	m_aafRowCol[iRow][0] = kVec.x;
	m_aafRowCol[iRow][1] = kVec.y;
	m_aafRowCol[iRow][2] = kVec.z;
}

inline Vector3 Matrix3::GetColumn (int iCol) const
{
	ZFAssert(0 <= iCol && iCol < 3, "Matrix3::GetColumn: Index out of range");
	return Vector3(m_aafRowCol[0][iCol], m_aafRowCol[1][iCol], m_aafRowCol[2][iCol]);
}

inline void Matrix3::SetColum (int iCol, const Vector3& kVec) 
{
	ZFAssert(0 <= iCol && iCol < 3, "Matrix3::SetColumn: Index out of range");
	m_aafRowCol[0][iCol] = kVec.x;
	m_aafRowCol[1][iCol] = kVec.y;
	m_aafRowCol[2][iCol] = kVec.z;
}

inline Vector3 Matrix3::GetAxis(int iAxisNum)
{
	return Vector3(m_aafRowCol[iAxisNum][0], m_aafRowCol[iAxisNum][1], m_aafRowCol[iAxisNum][2]);
}

inline void Matrix3::SetAxis(int iAxisNum, const Vector3& kNewAxis)
{
        m_aafRowCol[iAxisNum][0] = kNewAxis.x;
        m_aafRowCol[iAxisNum][1] = kNewAxis.y;
        m_aafRowCol[iAxisNum][2] = kNewAxis.z;
}

// Assignment 
inline Matrix3& Matrix3::operator= (const Matrix3& rkMatrix)
{
		memcpy(m_afData,rkMatrix.m_afData,sizeof(m_afData));
		return *this;
}


inline void Matrix3::Set(float f00,float f01,float f02,
						float f10,float f11,float f12,						
						float f20,float f21,float f22)
{							 	 
	m_aafRowCol[0][0] = f00;
	m_aafRowCol[0][1] = f01;
	m_aafRowCol[0][2] = f02;

	m_aafRowCol[1][0] = f10;
	m_aafRowCol[1][1] = f11;
	m_aafRowCol[1][2] = f12;

	m_aafRowCol[2][0] = f20;
	m_aafRowCol[2][1] = f21;
	m_aafRowCol[2][2] = f22;
};

inline void Matrix3::operator= (const Matrix4& rkMatrix)
{
	m_aafRowCol[0][0] = rkMatrix.RowCol[0][0];
	m_aafRowCol[0][1] = rkMatrix.RowCol[0][1];
	m_aafRowCol[0][2] = rkMatrix.RowCol[0][2];

	m_aafRowCol[1][0] = rkMatrix.RowCol[1][0];
	m_aafRowCol[1][1] = rkMatrix.RowCol[1][1];
	m_aafRowCol[1][2] = rkMatrix.RowCol[1][2];

	m_aafRowCol[2][0] = rkMatrix.RowCol[2][0];
	m_aafRowCol[2][1] = rkMatrix.RowCol[2][1];
	m_aafRowCol[2][2] = rkMatrix.RowCol[2][2];
}

inline void Matrix3::operator= (const Quaternion& rkQuaternion)
{
	rkQuaternion.ToRotationMatrix(*this);
}

inline void Matrix3::Zero(void) 
{
	for(int i=0;i<9;i++)
		m_afData[i]=0;
}


inline void Matrix3::Identity() 
{
	Set(1,0,0,
		 0,1,0,
		 0,0,1);

/*	*this=Matrix3( 1,0,0,
						0,1,0,
						0,0,1);*/
}

// Comparison
inline  bool Matrix3::operator== (const Matrix3& rkMatrix) const
{
	return (memcmp(m_afData,rkMatrix.m_afData,sizeof(m_afData)) == 0);
}

inline bool Matrix3::operator!= (const Matrix3& rkMatrix) const
{
	return !operator==(rkMatrix);
}

// Arithmetic operations
inline Matrix3 Matrix3::operator+ (const Matrix3& rkMatrix) const
{
    static Matrix3 kSum;
	 
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kSum.m_aafRowCol[iRow][iCol] = m_aafRowCol[iRow][iCol] +
                rkMatrix.m_aafRowCol[iRow][iCol];
        }
    }
    return kSum;
}

inline Matrix3 Matrix3::operator+= (const Matrix3& rkMatrix)
{
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            m_aafRowCol[iRow][iCol] += rkMatrix.m_aafRowCol[iRow][iCol];
        }
    }
    return *this;
}

inline Matrix3 Matrix3::operator- (const Matrix3& rkMatrix) const
{
    static Matrix3 kDiff;
	 
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kDiff.m_aafRowCol[iRow][iCol] = m_aafRowCol[iRow][iCol] -
                rkMatrix.m_aafRowCol[iRow][iCol];
        }
    }
    return kDiff;
}

inline Matrix3 Matrix3::operator-= (const Matrix3& rkMatrix)
{
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            m_aafRowCol[iRow][iCol] -= rkMatrix.m_aafRowCol[iRow][iCol];
        }
    }
    return *this;
}

inline Matrix3 Matrix3::operator* (const Matrix3& rkMatrix) const
{
    static Matrix3 kProd;
	 
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kProd.m_aafRowCol[iRow][iCol] =
                m_aafRowCol[iRow][0]*rkMatrix.m_aafRowCol[0][iCol] +
                m_aafRowCol[iRow][1]*rkMatrix.m_aafRowCol[1][iCol] +
                m_aafRowCol[iRow][2]*rkMatrix.m_aafRowCol[2][iCol];
        }
    }
    return kProd;
}

inline Matrix3 Matrix3::operator*= (const Matrix3& rkMatrix)
{
    static Matrix3 kProd;
	 
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kProd.m_aafRowCol[iRow][iCol] =
                m_aafRowCol[iRow][0]*rkMatrix.m_aafRowCol[0][iCol] +
                m_aafRowCol[iRow][1]*rkMatrix.m_aafRowCol[1][iCol] +
                m_aafRowCol[iRow][2]*rkMatrix.m_aafRowCol[2][iCol];
        }
    }

	*this = kProd;
    return kProd;
}

inline Matrix3 Matrix3::operator*(const float &f) const
{
	return Matrix3(	m_afData[0]*f,m_afData[1]*f,m_afData[2]*f,m_afData[3]*f,
      					m_afData[4]*f,m_afData[5]*f,m_afData[6]*f,m_afData[7]*f,
      					m_afData[8]*f);
      					
}

inline Matrix3 Matrix3::operator*=(const float &f) 
{
	for(int i=0;i<9;i++)
		m_afData[i]*=f;
		
	return *this;
}

// Matrix/Vector 
inline const Vector3& Matrix3::VectorTransform (const Vector3& kVec) const
{
	static Vector3 kV3;
	
	kV3.Set(
		kVec.x * m_aafRowCol[0][0] + kVec.y * m_aafRowCol[1][0] + kVec.z * m_aafRowCol[2][0],
		kVec.x * m_aafRowCol[0][1] + kVec.y * m_aafRowCol[1][1] + kVec.z * m_aafRowCol[2][1],
		kVec.x * m_aafRowCol[0][2] + kVec.y * m_aafRowCol[1][2] + kVec.z * m_aafRowCol[2][2]
		);

	return kV3;
		
	/*return Vector3 (
		kVec.x * m_aafRowCol[0][0] + kVec.y * m_aafRowCol[1][0] + kVec.z * m_aafRowCol[2][0],
		kVec.x * m_aafRowCol[0][1] + kVec.y * m_aafRowCol[1][1] + kVec.z * m_aafRowCol[2][1],
		kVec.x * m_aafRowCol[0][2] + kVec.y * m_aafRowCol[1][2] + kVec.z * m_aafRowCol[2][2]
		);*/
}


#endif





