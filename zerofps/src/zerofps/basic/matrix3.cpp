
#include "matrix3.h"
#include "matrix4.h"
#include "cassert"

using namespace std;

const Matrix3 Matrix3::ZERO(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f);
const Matrix3 Matrix3::IDENTITY(1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f);


Matrix3::Matrix3(float f00,float f01,float f02,
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

Vector3 Matrix3::GetColumn (int iCol) const
{
	assert( 0 <= iCol && iCol < 3);
	return Vector3(m_aafRowCol[0][iCol], m_aafRowCol[1][iCol], m_aafRowCol[2][iCol]);
}

// assignment and comparison
Matrix3& Matrix3::operator= (const Matrix3& rkMatrix)
{
    memcpy(m_aafRowCol,rkMatrix.m_aafRowCol,9*sizeof(float));
    return *this;
}

bool Matrix3::operator== (const Matrix3& rkMatrix) const
{
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            if ( m_aafRowCol[iRow][iCol] != rkMatrix.m_aafRowCol[iRow][iCol] )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool Matrix3::operator!= (const Matrix3& rkMatrix) const
{
    return !operator==(rkMatrix);
}

// Arithmetic operations
Matrix3 Matrix3::operator+ (const Matrix3& rkMatrix) const
{
    Matrix3 kSum;
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

Matrix3 Matrix3::operator+= (const Matrix3& rkMatrix)
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

Matrix3 Matrix3::operator- (const Matrix3& rkMatrix) const
{
    Matrix3 kDiff;
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

Matrix3 Matrix3::operator-= (const Matrix3& rkMatrix)
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

Matrix3 Matrix3::operator* (const Matrix3& rkMatrix) const
{
    Matrix3 kProd;
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

Matrix3 Matrix3::operator*= (const Matrix3& rkMatrix)
{
    Matrix3 kProd;
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

void Matrix3::operator= (const Matrix4& rkMatrix)
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


/*
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
	Matrix4 ny;
	
	for(int y=0;y<4;y++){
		for(int x=0;x<4;x++){
			ny.data[y*4+x]=0;			
			for(int i=0;i<4;i++){
				ny.data[y*4+x]+=data[y*4+i]*kOther.data[i*4+x];	

			}
		}
	}
	
/*	
//	float *ny=new float[4];
	Vector4 ny;

	for(int i=0;i<4;i++)
		ny[i]=0;
	
	for(int y=0;y<4;y++)
		for(int x=0;x<4;x++)
			ny[y]+=data[y*4+x]*kOther.data[x*4+y];		


	return ny;
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


*/














