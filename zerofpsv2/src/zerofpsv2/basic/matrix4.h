#ifndef _BASIC_MATRIX4_H_
#define _BASIC_MATRIX4_H_

#include <iostream>
#include "basic_x.h"
//#include "os.h"
#include "vector3.h"
#include "vector4.h"
#include <algorithm>
#include "globals.h"

class Matrix3;
class Quaternion;

/**	\brief	A Class for matrix math with 4 x 4 matrix.
		\ingroup Basic
*/
class BASIC_API Matrix4 
{
// Data
	public:
		union 
		{
			float RowCol[4][4];
			float data[16];	
		};

// Constructors
		Matrix4(void);
		Matrix4(float v1,float v2,float v3 ,float v4,
					float v5,float v6,float v7 ,float v8,						
					float v9,float v10,float v11 ,float v12,
					float v13,float v14,float v15 ,float v16);
		
		Matrix4(	const Matrix3& kOther);
					
// Operators
		// Assignment 
		Matrix4 operator=(const Matrix4 &kOther)
			{
				memcpy(data,kOther.data,sizeof(data));		
				return *this;
			}

		void operator=(const Matrix3 &rkMatrix);
		void operator= (const Quaternion& rkQuaternion);
		float &operator[](const int i);				
		void Set(float v1,float v2,float v3 ,float v4,
				 	float v5,float v6,float v7 ,float v8,						
				 	float v9,float v10,float v11 ,float v12,
				 	float v13,float v14,float v15 ,float v16);		
		
		// Comparison
		bool operator== (const Matrix4& rkMatrix) const
			{
				return (memcmp(data,rkMatrix.data,sizeof(data)) == 0);
			}			
		bool operator!= (const Matrix4& rkMatrix) const
			{
				return !operator==(rkMatrix);
			}
		
		// Arithmetic operations
		Matrix4 operator+ (const Matrix4& rkMatrix) const;
		Matrix4 operator+= (const Matrix4& rkMatrix);
		Matrix4 operator- (const Matrix4& rkMatrix) const;
		Matrix4 operator-= (const Matrix4& rkMatrix);
		Matrix4 operator* (const Matrix4& kOther) const;
		Matrix4 operator*= (const Matrix4& rkMatrix);

		Matrix4 operator*(const float &f) const;
		Matrix4 operator*=(const float &f);		
		Vector4 operator*(const Vector4 &f);
	
		
// Methods
		void 		Zero();								// Set whole matrix to zero.
		void 		Identity();							// Set matrix to the identity matrix.
		void 		Transponse();
		void 		SetZeroDelta(float delta);
		float 	Determinant(void);
		Matrix3 	submat(int i, int j);
		Matrix4 	Inverse( );

				
		void RadRotate(float fX, float fY, float fZ);
		void RadRotate(Vector3 kRot);
		
		void Rotate(float fX, float fY, float fZ);
		void Rotate(Vector3 kRot);
		
		void Scale(float fX, float fY, float fZ);
		void Scale(Vector3 kScale);
		
		void OldTranslate(float x, float y, float z);		
		void Translate(float fX, float fY, float fZ);
		void Translate(Vector3 kPos);
		
		void LookDir(Vector3 kDir,Vector3 kUp);

		// ************************************************
		
		Vector3 VectorIRotate (const Vector3& kVec);
		
		Vector3 VectorRotate (const Vector3& kVec)
		{
			return Vector3 (
				kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0],
				kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1],
				kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2]
				);
		}

		Vector3 VectorTransform (const Vector3& kVec) 
		{
			return Vector3 (
				kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0] + RowCol[3][0],
				kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1] + RowCol[3][1],
				kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2] + RowCol[3][2]
				);
		}


// Accessors 
		Vector3 GetRotVector();
		Vector3 GetPosVector();
		void SetAxis(int iAxisNum, Vector3 kNewAxis);
		Vector3 GetAxis(int iAxisNum);

				
		Vector3 GetPos()
		{
			return Vector3(RowCol[3][0],RowCol[3][1],RowCol[3][2]);
		}

		void SetPos(Vector3 kPos)
		{
			RowCol[3][0] = kPos.x;
			RowCol[3][1] = kPos.y;
			RowCol[3][2] = kPos.z;
		}


// Other
		void Print();

};

#endif





