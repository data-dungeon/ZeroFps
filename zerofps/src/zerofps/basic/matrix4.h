#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <iostream>
#include "vector4.h"
using namespace std;

class Matrix4 {
	private:	
	
	public:
		float data[16];	
	
		Matrix4(float v1,float v2,float v3 ,float v4,
						float v5,float v6,float v7 ,float v8,						
						float v9,float v10,float v11 ,float v12,
						float v13,float v14,float v15 ,float v16);
		
		Matrix4(void);
						
		Matrix4 operator=(const Matrix4 &kOther);
		Matrix4 operator*(const float &f) const;
		Vector4 operator*(const Matrix4 &kOther) const;
		Matrix4 operator*=(const float &f);		
		Vector4 operator*(const Vector4 &f);
	



};


#endif





