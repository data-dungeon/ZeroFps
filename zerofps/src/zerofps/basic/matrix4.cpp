#include "matrix4.h"
#include "matrix3.h"



Matrix4::Matrix4(float v1,float v2,float v3 ,float v4,
								 float v5,float v6,float v7 ,float v8,						
								 float v9,float v10,float v11 ,float v12,
							 	 float v13,float v14,float v15 ,float v16)
{							 	 
	data[0]=v1;		data[1]=v2;		data[2]=v3;		data[3]=v4;
	data[4]=v5;		data[5]=v6;		data[6]=v7;		data[7]=v8;
	data[8]=v9;		data[9]=v10;	data[10]=v11;	data[11]=v12;	
	data[12]=v13;	data[13]=v14;	data[14]=v15;	data[15]=v16;	
	
};

Matrix4::Matrix4(void) {
	for(int i=0;i<16;i++)
		data[i]=0;
}
	
Matrix4 Matrix4::operator*(const float &f) const
{
	return Matrix4(data[0]*f,data[1]*f,data[2]*f,data[3]*f,
      					 data[4]*f,data[5]*f,data[6]*f,data[7]*f,
      					 data[8]*f,data[9]*f,data[10]*f,data[11]*f,
      					 data[12]*f,data[13]*f,data[14]*f,data[15]*f);
      					
}

Matrix4 Matrix4::operator*=(const float &f) 
{
	for(int i=0;i<16;i++)
		data[i]*=f;
		
	return *this;
}

Matrix4 Matrix4::operator=(const Matrix4 &kOther)
{
	for(int i=0;i<16;i++)
		data[i]=kOther.data[i];
	
	return *this;

}


Vector4 Matrix4::operator*(const Vector4 &f)
{
	Vector4 ny;
	
	ny.x=data[0]*f.x + data[1]*f.y + data[2]*f.z + data[3]*f.w;
	ny.y=data[4]*f.x + data[5]*f.y + data[6]*f.z + data[7]*f.w;
	ny.z=data[8]*f.x + data[9]*f.y + data[10]*f.z + data[11]*f.w;
	ny.w=data[12]*f.x + data[13]*f.y + data[14]*f.z + data[15]*f.w;

	return ny;

}

Matrix4 Matrix4::operator*(const Matrix4 &kOther) const 
{
/*	int i,j,k;
	double ab;
	Matrix4 temp;

	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			ab=0;
			for(k=0; k<4; k++)	ab += RowCol[k][i] * kOther.RowCol[j][k];
			temp.RowCol[j][i]=ab;
		}
	}

	return temp;*/


    Matrix4 kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            kProd.RowCol[iRow][iCol] =
                RowCol[iRow][0]*kOther.RowCol[0][iCol] +
                RowCol[iRow][1]*kOther.RowCol[1][iCol] +
                RowCol[iRow][2]*kOther.RowCol[2][iCol] +
                RowCol[iRow][3]*kOther.RowCol[3][iCol];
        }
    }
    return kProd;

/*
	Matrix4 ny;
	
	for(int y=0;y<4;y++){
		for(int x=0;x<4;x++){
			ny.data[y*4+x]=0;			
			for(int i=0;i<4;i++){
				ny.data[y*4+x]+=data[y*4+i]*kOther.data[i*4+x];	

			}
		}
	}

	return ny;*/
	
/*	
//	float *ny=new float[4];
	Vector4 ny;

	for(int i=0;i<4;i++)
		ny[i]=0;
	
	for(int y=0;y<4;y++)
		for(int x=0;x<4;x++)
			ny[y]+=data[y*4+x]*kOther.data[x*4+y];		
*/

}

void Matrix4::Print()
{
	cout <<" -= The Matrix =-"<<endl;
	for(int y=0;y<4;y++){
		for(int x=0;x<4;x++){
			cout<<data[y*4+x]<<"\t";			
		}
		cout<<endl;
	}


}

void Matrix4::Identity() {
	*this=Matrix4(1,0,0,0,
								0,1,0,0,
								0,0,1,0,
								0,0,0,1);

}

float &Matrix4::operator[](const int i)
{
	return data[i];
}

void Matrix4::operator=(const Matrix3 &rkMatrix)
{
	RowCol[0][0] = rkMatrix.m_aafRowCol[0][0];
	RowCol[0][1] = rkMatrix.m_aafRowCol[0][1];
	RowCol[0][2] = rkMatrix.m_aafRowCol[0][2];

	RowCol[1][0] = rkMatrix.m_aafRowCol[1][0];
	RowCol[1][1] = rkMatrix.m_aafRowCol[1][1];
	RowCol[1][2] = rkMatrix.m_aafRowCol[1][2];

	RowCol[2][0] = rkMatrix.m_aafRowCol[2][0];
	RowCol[2][1] = rkMatrix.m_aafRowCol[2][1];
	RowCol[2][2] = rkMatrix.m_aafRowCol[2][2];

}

void Matrix4::operator= (const Quaternion& rkQuaternion)
{
	Matrix3 mat3;
	mat3 = Matrix3::IDENTITY;
	mat3 = rkQuaternion;
	Identity();
	*this = mat3;
}

Vector3 Matrix4::VectorRotate (const Vector3 kVec)
{
	Vector3 res;
	
	res.x = kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0];
	res.y = kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1];
	res.z = kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2];
	return res;
}

Vector3 Matrix4::VectorIRotate (const Vector3 kVec)
{
	Vector3 res;
	
	res.x = kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0];
	res.y = kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1];
	res.z = kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2];
	return res;
}

Vector3 Matrix4::VectorTransform (const Vector3 kVec)
{
	Vector3 res;
	
/*
	res.x = kVec.x * RowCol[0][0] + kVec.y * RowCol[0][1] + kVec.z * RowCol[0][2] + RowCol[0][3];
	res.y = kVec.x * RowCol[1][0] + kVec.y * RowCol[1][1] + kVec.z * RowCol[1][2] + RowCol[1][3];
	res.z = kVec.x * RowCol[2][0] + kVec.y * RowCol[2][1] + kVec.z * RowCol[2][2] + RowCol[2][3];
*/
		
	res.x = kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0] + RowCol[3][0];
	res.y = kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1] + RowCol[3][1];
	res.z = kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2] + RowCol[3][2];
	
	return res;
}
