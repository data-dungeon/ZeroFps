#ifndef _BASIC_MATRIX4_H_
#define _BASIC_MATRIX4_H_

#include <iostream>
#include "basic_x.h"
//#include "os.h"
#include <algorithm>
#include "globals.h"
#include "math.h"

class Matrix3;
class Quaternion;

class TestVec4
{
public:
	float x,y,z,w;

};

/**	\brief	A Class for matrix math with 4 x 4 matrix.
		\ingroup Basic
*/
class BASIC_API Matrix4 
{
// Data
	public:
		union 
		{
			TestVec4	vec[4];
			float		RowCol[4][4];
			float		data[16];	
		};

// Constructors
		Matrix4(void);
		Matrix4(float v1,float v2,float v3 ,float v4,
					float v5,float v6,float v7 ,float v8,						
					float v9,float v10,float v11 ,float v12,
					float v13,float v14,float v15 ,float v16);
		Matrix4(	const Matrix3& kOther);
	
// Access 
		float &operator[](const int i);				
		void SetAxis(int iAxisNum, const Vector3& kNewAxis);
		Vector3 GetAxis(int iAxisNum);
		Vector3 GetPos() const;
		void SetPos(const Vector3& kPos);

// Assignment 
		Matrix4 operator=(const Matrix4 &kOther);
		void operator=(const Matrix3 &rkMatrix);
		void operator=(const Quaternion& rkQuaternion);
		void Set(float v1,float v2,float v3 ,float v4,
				 	float v5,float v6,float v7 ,float v8,						
				 	float v9,float v10,float v11 ,float v12,
				 	float v13,float v14,float v15 ,float v16);		
		void Zero();								// Set whole matrix to zero.
		void Identity();							// Set matrix to the identity matrix.

// Comparison
		bool operator== (const Matrix4& rkMatrix) const;
		bool operator!= (const Matrix4& rkMatrix) const;
		
// Arithmetic operations
		Matrix4 operator+ (const Matrix4& rkMatrix) const;
		Matrix4 operator+=(const Matrix4& rkMatrix);
		Matrix4 operator- (const Matrix4& rkMatrix) const;
		Matrix4 operator-=(const Matrix4& rkMatrix);
		Matrix4 operator* (const Matrix4& kOther) const;
		Matrix4 operator*=(const Matrix4& rkMatrix);
		
		Matrix4 operator* (const float &f) const;
		Matrix4 operator*=(const float &f);		
		Vector4 operator* (const Vector4 &f);

// Matrix/Vector 
		Vector3 VectorIRotate (const Vector3& kVec) const;
		Vector3 VectorRotate (const Vector3& kVec) const;
		Vector3 VectorTransform (const Vector3& kVec) const;

// Common Matrix Methods
		void 		Transponse();
		Matrix4 	Inverse( );
		float 	Determinant(void);

// Other
		string ToString();
		string ToString_Vec(int iIndex);

// NoSort
		void 		SetZeroDelta(float delta);
		Matrix3 	submat(int i, int j);

		void RadRotate(float fX, float fY, float fZ);
		void RadRotate(const Vector3& kRot);
		void Rotate(float fX, float fY, float fZ);
		void Rotate(const Vector3& kRot);
		void Scale(float fX, float fY, float fZ);
		void Scale(float fScale);
		void Scale(const Vector3& kScale);
		void OldTranslate(float x, float y, float z);		
		void Translate(float fX, float fY, float fZ);
		void Translate(const Vector3& kPos);
		void LookDir(Vector3 kDir,Vector3 kUp);

		Vector3 GetRotVector();
		//Vector3 GetPosVector();
};

#include "matrix3.h"

// Constructors
inline Matrix4::Matrix4(void)
{
	for(int i=0;i<16;i++)
		data[i]=0;
}

inline Matrix4::Matrix4(float v1,float v2,float v3 ,float v4,
					float v5,float v6,float v7 ,float v8,						
					float v9,float v10,float v11 ,float v12,
					float v13,float v14,float v15 ,float v16)
{
	data[0]=v1;		data[1]=v2;		data[2]=v3;		data[3]=v4;
	data[4]=v5;		data[5]=v6;		data[6]=v7;		data[7]=v8;
	data[8]=v9;		data[9]=v10;	data[10]=v11;	data[11]=v12;	
	data[12]=v13;	data[13]=v14;	data[14]=v15;	data[15]=v16;	
}
		
inline Matrix4::Matrix4(	const Matrix3& kOther)
{
	data[0]=kOther.m_afData[0];	data[1]=kOther.m_afData[1];	data[2]=kOther.m_afData[2];	data[3]=0;
	data[4]=kOther.m_afData[3];	data[5]=kOther.m_afData[4];	data[6]=kOther.m_afData[5];	data[7]=0;
	data[8]=kOther.m_afData[6];	data[9]=kOther.m_afData[7];	data[10]=kOther.m_afData[8];	data[11]=0;	
	data[12]=0;							data[13]=0;							data[14]=0;							data[15]=1;	
}

// Access 
inline float &Matrix4::operator[](const int i)
{
	return data[i];
}

inline void Matrix4::SetAxis(int iAxisNum, const Vector3& kNewAxis)
{
        RowCol[iAxisNum][0] = kNewAxis.x;
        RowCol[iAxisNum][1] = kNewAxis.y;
        RowCol[iAxisNum][2] = kNewAxis.z;
}

inline Vector3 Matrix4::GetAxis(int iAxisNum)
{
	return Vector3(RowCol[iAxisNum][0], RowCol[iAxisNum][1], RowCol[iAxisNum][2]);
}

inline Vector3 Matrix4::GetPos() const
{
	return Vector3(RowCol[3][0],RowCol[3][1],RowCol[3][2]);
}

inline void Matrix4::SetPos(const Vector3& kPos)
{
	RowCol[3][0] = kPos.x;
	RowCol[3][1] = kPos.y;
	RowCol[3][2] = kPos.z;
}

// Assignment 
inline Matrix4 Matrix4::operator=(const Matrix4 &kOther)
{
	memcpy(data,kOther.data,sizeof(data));		
	return *this;
}

inline void Matrix4::operator=(const Matrix3 &rkMatrix)
{
// 	Identity();

	RowCol[0][0] = rkMatrix.m_aafRowCol[0][0];
	RowCol[0][1] = rkMatrix.m_aafRowCol[0][1];
	RowCol[0][2] = rkMatrix.m_aafRowCol[0][2];
	RowCol[0][3] = 0;

	RowCol[1][0] = rkMatrix.m_aafRowCol[1][0];
	RowCol[1][1] = rkMatrix.m_aafRowCol[1][1];
	RowCol[1][2] = rkMatrix.m_aafRowCol[1][2];
	RowCol[1][3] = 0;

	RowCol[2][0] = rkMatrix.m_aafRowCol[2][0];
	RowCol[2][1] = rkMatrix.m_aafRowCol[2][1];
	RowCol[2][2] = rkMatrix.m_aafRowCol[2][2];
	RowCol[2][3] = 0;	
	
	RowCol[3][0] = 0;
	RowCol[3][1] = 0;
	RowCol[3][2] = 0;
	RowCol[3][3] = 1;	
}

inline void Matrix4::operator= (const Quaternion& rkQuaternion)
{
	static Matrix3 mat3;
	mat3 = Matrix3::IDENTITY;
	mat3 = rkQuaternion;
	Identity();
	*this = mat3;
}

inline void Matrix4::Set( float v1,float v2,float v3 ,float v4,
						float v5,float v6,float v7 ,float v8,						
						float v9,float v10,float v11 ,float v12,
						float v13,float v14,float v15 ,float v16)
{							 	 
	data[0]=v1;		data[1]=v2;		data[2]=v3;		data[3]=v4;
	data[4]=v5;		data[5]=v6;		data[6]=v7;		data[7]=v8;
	data[8]=v9;		data[9]=v10;	data[10]=v11;	data[11]=v12;	
	data[12]=v13;	data[13]=v14;	data[14]=v15;	data[15]=v16;	
};

inline void Matrix4::Zero(void) 
{
	memset(this, 0, sizeof(Matrix4));
}

inline void Matrix4::Identity() 
{
	Set(	1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1);
}

// Comparison
inline bool Matrix4::operator== (const Matrix4& rkMatrix) const
{
	return (memcmp(data,rkMatrix.data,sizeof(data)) == 0);
}	

inline bool Matrix4::operator!= (const Matrix4& rkMatrix) const
{
	return !operator==(rkMatrix);
}

// Arithmetic operations
inline Matrix4 Matrix4::operator+ (const Matrix4& rkMatrix) const
{
    static Matrix4 kSum;
	 
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            kSum.RowCol[iRow][iCol] = RowCol[iRow][iCol] +
                rkMatrix.RowCol[iRow][iCol];
        }
    }
    return kSum;

}

inline Matrix4 Matrix4::operator+= (const Matrix4& rkMatrix)
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            RowCol[iRow][iCol] += rkMatrix.RowCol[iRow][iCol];
        }
    }
    return *this;
}

inline Matrix4 Matrix4::operator- (const Matrix4& rkMatrix) const
{
    static Matrix4 kDiff;
	 
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            kDiff.RowCol[iRow][iCol] = RowCol[iRow][iCol] -
                rkMatrix.RowCol[iRow][iCol];
        }
    }
    return kDiff;

}

inline Matrix4 Matrix4::operator-= (const Matrix4& rkMatrix)
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            RowCol[iRow][iCol] -= rkMatrix.RowCol[iRow][iCol];
        }
    }
    return *this;
}

inline Matrix4 Matrix4::operator*(const Matrix4 &kOther) const 
{
    static Matrix4 kProd;
	 
//     for (int iRow = 0; iRow < 4; iRow++)
//     {
//         for (int iCol = 0; iCol < 4; iCol++)
//         {
//             kProd.RowCol[iRow][iCol] =
//                 RowCol[iRow][0]*kOther.RowCol[0][iCol] +
//                 RowCol[iRow][1]*kOther.RowCol[1][iCol] +
//                 RowCol[iRow][2]*kOther.RowCol[2][iCol] +
//                 RowCol[iRow][3]*kOther.RowCol[3][iCol];
//         }
//     }
    
	//dvoids new iteration less version
	//row 0
	kProd.RowCol[0][0] = RowCol[0][0]*kOther.RowCol[0][0] + RowCol[0][1]*kOther.RowCol[1][0] + RowCol[0][2]*kOther.RowCol[2][0] + RowCol[0][3]*kOther.RowCol[3][0];
	kProd.RowCol[0][1] = RowCol[0][0]*kOther.RowCol[0][1] + RowCol[0][1]*kOther.RowCol[1][1] + RowCol[0][2]*kOther.RowCol[2][1] + RowCol[0][3]*kOther.RowCol[3][1];
	kProd.RowCol[0][2] = RowCol[0][0]*kOther.RowCol[0][2] + RowCol[0][1]*kOther.RowCol[1][2] + RowCol[0][2]*kOther.RowCol[2][2] +	RowCol[0][3]*kOther.RowCol[3][2];
	kProd.RowCol[0][3] = RowCol[0][0]*kOther.RowCol[0][3] + RowCol[0][1]*kOther.RowCol[1][3] + RowCol[0][2]*kOther.RowCol[2][3] + RowCol[0][3]*kOther.RowCol[3][3];
	//row 1
	kProd.RowCol[1][0] = RowCol[1][0]*kOther.RowCol[0][0] + RowCol[1][1]*kOther.RowCol[1][0] + RowCol[1][2]*kOther.RowCol[2][0] + RowCol[1][3]*kOther.RowCol[3][0];
	kProd.RowCol[1][1] = RowCol[1][0]*kOther.RowCol[0][1] + RowCol[1][1]*kOther.RowCol[1][1] + RowCol[1][2]*kOther.RowCol[2][1] + RowCol[1][3]*kOther.RowCol[3][1];
	kProd.RowCol[1][2] = RowCol[1][0]*kOther.RowCol[0][2] + RowCol[1][1]*kOther.RowCol[1][2] + RowCol[1][2]*kOther.RowCol[2][2] +	RowCol[1][3]*kOther.RowCol[3][2];
	kProd.RowCol[1][3] = RowCol[1][0]*kOther.RowCol[0][3] + RowCol[1][1]*kOther.RowCol[1][3] + RowCol[1][2]*kOther.RowCol[2][3] + RowCol[1][3]*kOther.RowCol[3][3];
	//row 2
	kProd.RowCol[2][0] = RowCol[2][0]*kOther.RowCol[0][0] + RowCol[2][1]*kOther.RowCol[1][0] + RowCol[2][2]*kOther.RowCol[2][0] + RowCol[2][3]*kOther.RowCol[3][0];
	kProd.RowCol[2][1] = RowCol[2][0]*kOther.RowCol[0][1] + RowCol[2][1]*kOther.RowCol[1][1] + RowCol[2][2]*kOther.RowCol[2][1] + RowCol[2][3]*kOther.RowCol[3][1];
	kProd.RowCol[2][2] = RowCol[2][0]*kOther.RowCol[0][2] + RowCol[2][1]*kOther.RowCol[1][2] + RowCol[2][2]*kOther.RowCol[2][2] +	RowCol[2][3]*kOther.RowCol[3][2];
	kProd.RowCol[2][3] = RowCol[2][0]*kOther.RowCol[0][3] + RowCol[2][1]*kOther.RowCol[1][3] + RowCol[2][2]*kOther.RowCol[2][3] + RowCol[2][3]*kOther.RowCol[3][3];
	//row 3
	kProd.RowCol[3][0] = RowCol[3][0]*kOther.RowCol[0][0] + RowCol[3][1]*kOther.RowCol[1][0] + RowCol[3][2]*kOther.RowCol[2][0] + RowCol[3][3]*kOther.RowCol[3][0];
	kProd.RowCol[3][1] = RowCol[3][0]*kOther.RowCol[0][1] + RowCol[3][1]*kOther.RowCol[1][1] + RowCol[3][2]*kOther.RowCol[2][1] + RowCol[3][3]*kOther.RowCol[3][1];
	kProd.RowCol[3][2] = RowCol[3][0]*kOther.RowCol[0][2] + RowCol[3][1]*kOther.RowCol[1][2] + RowCol[3][2]*kOther.RowCol[2][2] +	RowCol[3][3]*kOther.RowCol[3][2];
	kProd.RowCol[3][3] = RowCol[3][0]*kOther.RowCol[0][3] + RowCol[3][1]*kOther.RowCol[1][3] + RowCol[3][2]*kOther.RowCol[2][3] + RowCol[3][3]*kOther.RowCol[3][3];
    
    return kProd;
}

inline Matrix4 Matrix4::operator*= (const Matrix4& kOther)
{
    static Matrix4 kProd;
	 
//     for (int iRow = 0; iRow < 4; iRow++)
//     {
//         for (int iCol = 0; iCol < 4; iCol++)
//         {
//             kProd.RowCol[iRow][iCol] =
//                 RowCol[iRow][0]*rkMatrix.RowCol[0][iCol] +
//                 RowCol[iRow][1]*rkMatrix.RowCol[1][iCol] +
//                 RowCol[iRow][2]*rkMatrix.RowCol[2][iCol] +
//                 RowCol[iRow][3]*rkMatrix.RowCol[3][iCol];
//         }
//     }


	//dvoids new iteration less version
	//row 0
	kProd.RowCol[0][0] = RowCol[0][0]*kOther.RowCol[0][0] + RowCol[0][1]*kOther.RowCol[1][0] + RowCol[0][2]*kOther.RowCol[2][0] + RowCol[0][3]*kOther.RowCol[3][0];
	kProd.RowCol[0][1] = RowCol[0][0]*kOther.RowCol[0][1] + RowCol[0][1]*kOther.RowCol[1][1] + RowCol[0][2]*kOther.RowCol[2][1] + RowCol[0][3]*kOther.RowCol[3][1];
	kProd.RowCol[0][2] = RowCol[0][0]*kOther.RowCol[0][2] + RowCol[0][1]*kOther.RowCol[1][2] + RowCol[0][2]*kOther.RowCol[2][2] +	RowCol[0][3]*kOther.RowCol[3][2];
	kProd.RowCol[0][3] = RowCol[0][0]*kOther.RowCol[0][3] + RowCol[0][1]*kOther.RowCol[1][3] + RowCol[0][2]*kOther.RowCol[2][3] + RowCol[0][3]*kOther.RowCol[3][3];
	//row 1
	kProd.RowCol[1][0] = RowCol[1][0]*kOther.RowCol[0][0] + RowCol[1][1]*kOther.RowCol[1][0] + RowCol[1][2]*kOther.RowCol[2][0] + RowCol[1][3]*kOther.RowCol[3][0];
	kProd.RowCol[1][1] = RowCol[1][0]*kOther.RowCol[0][1] + RowCol[1][1]*kOther.RowCol[1][1] + RowCol[1][2]*kOther.RowCol[2][1] + RowCol[1][3]*kOther.RowCol[3][1];
	kProd.RowCol[1][2] = RowCol[1][0]*kOther.RowCol[0][2] + RowCol[1][1]*kOther.RowCol[1][2] + RowCol[1][2]*kOther.RowCol[2][2] +	RowCol[1][3]*kOther.RowCol[3][2];
	kProd.RowCol[1][3] = RowCol[1][0]*kOther.RowCol[0][3] + RowCol[1][1]*kOther.RowCol[1][3] + RowCol[1][2]*kOther.RowCol[2][3] + RowCol[1][3]*kOther.RowCol[3][3];
	//row 2
	kProd.RowCol[2][0] = RowCol[2][0]*kOther.RowCol[0][0] + RowCol[2][1]*kOther.RowCol[1][0] + RowCol[2][2]*kOther.RowCol[2][0] + RowCol[2][3]*kOther.RowCol[3][0];
	kProd.RowCol[2][1] = RowCol[2][0]*kOther.RowCol[0][1] + RowCol[2][1]*kOther.RowCol[1][1] + RowCol[2][2]*kOther.RowCol[2][1] + RowCol[2][3]*kOther.RowCol[3][1];
	kProd.RowCol[2][2] = RowCol[2][0]*kOther.RowCol[0][2] + RowCol[2][1]*kOther.RowCol[1][2] + RowCol[2][2]*kOther.RowCol[2][2] +	RowCol[2][3]*kOther.RowCol[3][2];
	kProd.RowCol[2][3] = RowCol[2][0]*kOther.RowCol[0][3] + RowCol[2][1]*kOther.RowCol[1][3] + RowCol[2][2]*kOther.RowCol[2][3] + RowCol[2][3]*kOther.RowCol[3][3];
	//row 3
	kProd.RowCol[3][0] = RowCol[3][0]*kOther.RowCol[0][0] + RowCol[3][1]*kOther.RowCol[1][0] + RowCol[3][2]*kOther.RowCol[2][0] + RowCol[3][3]*kOther.RowCol[3][0];
	kProd.RowCol[3][1] = RowCol[3][0]*kOther.RowCol[0][1] + RowCol[3][1]*kOther.RowCol[1][1] + RowCol[3][2]*kOther.RowCol[2][1] + RowCol[3][3]*kOther.RowCol[3][1];
	kProd.RowCol[3][2] = RowCol[3][0]*kOther.RowCol[0][2] + RowCol[3][1]*kOther.RowCol[1][2] + RowCol[3][2]*kOther.RowCol[2][2] +	RowCol[3][3]*kOther.RowCol[3][2];
	kProd.RowCol[3][3] = RowCol[3][0]*kOther.RowCol[0][3] + RowCol[3][1]*kOther.RowCol[1][3] + RowCol[3][2]*kOther.RowCol[2][3] + RowCol[3][3]*kOther.RowCol[3][3];
    
 	*this = kProd;
    return kProd;
}

inline Matrix4 Matrix4::operator*(const float &f) const
{
	return Matrix4(	data[0]*f,data[1]*f,data[2]*f,data[3]*f,
      					data[4]*f,data[5]*f,data[6]*f,data[7]*f,
      					data[8]*f,data[9]*f,data[10]*f,data[11]*f,
      					data[12]*f,data[13]*f,data[14]*f,data[15]*f);
      					
}

inline Matrix4 Matrix4::operator*=(const float &f) 
{
	for(int i=0;i<16;i++)
		data[i]*=f;
		
	return *this;
}

inline Vector4 Matrix4::operator*(const Vector4 &f)
{
	static Vector4 ny;
	
	ny.x=data[0]*f.x + data[1]*f.y + data[2]*f.z + data[3]*f.w;
	ny.y=data[4]*f.x + data[5]*f.y + data[6]*f.z + data[7]*f.w;
	ny.z=data[8]*f.x + data[9]*f.y + data[10]*f.z + data[11]*f.w;
	ny.w=data[12]*f.x + data[13]*f.y + data[14]*f.z + data[15]*f.w;

	return ny;

}

// Common Matrix Methods
inline  Vector3 Matrix4::VectorIRotate (const Vector3& kVec) const
{
	static Vector3 res;
	
	res.x = kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0];
	res.y = kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1];
	res.z = kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2];
	return res;
}

inline Vector3 Matrix4::VectorRotate (const Vector3& kVec) const
{
	static Vector3 kV3;			
	kV3.Set(	kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0],
				kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1],
				kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2]
				);
	
	return kV3;
}

inline Vector3 Matrix4::VectorTransform (const Vector3& kVec) const
{
	static Vector3 kV3;			
	kV3.Set(	kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0] + RowCol[3][0],
				kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1] + RowCol[3][1],
				kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2] + RowCol[3][2]
				);
	
	return kV3;
}


#endif





