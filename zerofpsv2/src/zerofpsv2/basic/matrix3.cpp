
#include "matrix3.h"
#include "matrix4.h"
#include "quaternion.h"
#include "zfassert.h"
#include <math.h>

using namespace std;

const Matrix3 Matrix3::ZERO(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f);
const Matrix3 Matrix3::IDENTITY(1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f);

// Constructors

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


// Operators	-	Assignment
Matrix3& Matrix3::operator= (const Matrix3& rkMatrix)
{
    memcpy(m_aafRowCol,rkMatrix.m_aafRowCol,9*sizeof(float));
    return *this;
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

void Matrix3::operator= (const Quaternion& rkQuaternion)
{
	rkQuaternion.ToRotationMatrix(*this);
}

// Operators	-	Comparison
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

bool Matrix3::operator!= (const Matrix3& rkMatrix) const
{
    return !operator==(rkMatrix);
}

// Operators	-	Arithmetic operations
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

Matrix3 Matrix3::operator*(const float &f) const
{
	return Matrix3(m_afData[0]*f,m_afData[1]*f,m_afData[2]*f,m_afData[3]*f,
      					 m_afData[4]*f,m_afData[5]*f,m_afData[6]*f,m_afData[7]*f,
      					 m_afData[8]*f);
      					
}

Matrix3 Matrix3::operator*=(const float &f) 
{
	for(int i=0;i<9;i++)
		m_afData[i]*=f;
		
	return *this;
}

// Methods
void Matrix3::Zero(void) 
{
	for(int i=0;i<9;i++)
		m_afData[i]=0;
}


void Matrix3::Identity() 
{
	*this=Matrix3( 1,0,0,
						0,1,0,
						0,0,1);
}

bool Matrix3::inverse (Matrix3& inv, float tolerance) const
{
	float det = determinant();
	
	if(fabs(det) <= tolerance)	return false;

	inv.m_aafRowCol[0][0] =   m_aafRowCol[1][1] * m_aafRowCol[2][2] - m_aafRowCol[1][2] * m_aafRowCol[2][1];
	inv.m_aafRowCol[0][1] = -(m_aafRowCol[0][1] * m_aafRowCol[2][2] - m_aafRowCol[2][1] * m_aafRowCol[0][2]);
	inv.m_aafRowCol[0][2] =   m_aafRowCol[0][1] * m_aafRowCol[1][2] - m_aafRowCol[1][1] * m_aafRowCol[0][2];

	inv.m_aafRowCol[1][0] = -(m_aafRowCol[1][0] * m_aafRowCol[2][2] - m_aafRowCol[1][2] * m_aafRowCol[2][0]);
	inv.m_aafRowCol[1][1] =   m_aafRowCol[0][0] * m_aafRowCol[2][2] - m_aafRowCol[2][0] * m_aafRowCol[0][2];
	inv.m_aafRowCol[1][2] = -(m_aafRowCol[0][0] * m_aafRowCol[1][2] - m_aafRowCol[1][0] * m_aafRowCol[0][2]);
	 
	inv.m_aafRowCol[2][0] =   m_aafRowCol[1][0] * m_aafRowCol[2][1] - m_aafRowCol[2][0] * m_aafRowCol[1][1];
	inv.m_aafRowCol[2][1] = -(m_aafRowCol[0][0] * m_aafRowCol[2][1] - m_aafRowCol[2][0] * m_aafRowCol[0][1]);
	inv.m_aafRowCol[2][2] =   m_aafRowCol[0][0] * m_aafRowCol[1][1] - m_aafRowCol[0][1] * m_aafRowCol[1][0];

	float invd = float(1.0 / det);
	for(int i=0; i<9; i++)	inv.m_afData[i] *= invd;
	return true;
}

float Matrix3::determinant(void)	 const					
{
	float det;

	det = m_aafRowCol[0][0] * (m_aafRowCol[1][1] * m_aafRowCol[2][2] - m_aafRowCol[1][2] * m_aafRowCol[2][1])
		- m_aafRowCol[1][0] * (m_aafRowCol[0][1] * m_aafRowCol[2][2] - m_aafRowCol[0][2] * m_aafRowCol[2][1])
		+ m_aafRowCol[2][0] * (m_aafRowCol[0][1] * m_aafRowCol[1][2] - m_aafRowCol[0][2] * m_aafRowCol[1][1]);

	return det;
}

// Accessors 
Vector3 Matrix3::GetColumn (int iCol) const
{
	ZFAssert(0 <= iCol && iCol < 3, "Matrix3::GetColumn: Index out of range");
		// assert( );
	return Vector3(m_aafRowCol[0][iCol], m_aafRowCol[1][iCol], m_aafRowCol[2][iCol]);
}

// Other
void Matrix3::Print()
{
	cout <<" -= The Matrix =-"<<endl;
	for(int y=0;y<3;y++){
		for(int x=0;x<3;x++){
			cout<< m_afData[y*3+x]<<"\t";			
		}
		cout<<endl;
	}
}







