#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <iostream>
#include "basic_x.h"
#include "vector3.h"
#include "vector4.h"


class BASIC_API Matrix4 {
	private:	
	
	public:
		union 
		{
/*			struct
			{
				Vector3		AxisX();	// X - Axis
				float		m03;
				Vector3		AxisY();	// Y - Axis
				float		m13;
				Vector3		AxisZ();	// Z - Axis
				float		m23;	
				union
				{
					struct  { Vector3	Pos();	};// Position as a vector 
					struct	{ float		x,y,z;	};// Position as coordinaters
				};
				float		m33;
			};*/


			float RowCol[4][4];
			float data[16];	
		};

		// float data[16];	
	
		Matrix4(float v1,float v2,float v3 ,float v4,
						float v5,float v6,float v7 ,float v8,						
						float v9,float v10,float v11 ,float v12,
						float v13,float v14,float v15 ,float v16);
		
		Matrix4(void);
						
		float &operator[](const int i);						
		
		Matrix4 operator=(const Matrix4 &kOther);
		Matrix4 operator*(const float &f) const;
		Matrix4 operator*(const Matrix4 &kOther) const;
		Matrix4 operator*=(const float &f);		
		Vector4 operator*(const Vector4 &f);
	
		void Print();
		void Identity();

		// ************************************************
};


#endif





