
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

void Matrix3::Set(float f00,float f01,float f02,
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

void Matrix3::SetAxis(int iAxisNum, Vector3 kNewAxis)
{
        m_aafRowCol[iAxisNum][0] = kNewAxis.x;
        m_aafRowCol[iAxisNum][1] = kNewAxis.y;
        m_aafRowCol[iAxisNum][2] = kNewAxis.z;
}

Vector3 Matrix3::GetAxis(int iAxisNum)
{
	return Vector3(m_aafRowCol[iAxisNum][0], m_aafRowCol[iAxisNum][1], m_aafRowCol[iAxisNum][2]);
}


// Operators	-	Assignment

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

void Matrix3::Transponse()
{
	swap(m_aafRowCol[0][1], m_aafRowCol[1][0]);
	swap(m_aafRowCol[0][2], m_aafRowCol[2][0]);
	swap(m_aafRowCol[0][3], m_aafRowCol[3][0]);
	
	swap(m_aafRowCol[1][2], m_aafRowCol[2][1]);
	swap(m_aafRowCol[1][3], m_aafRowCol[3][1]);

	swap(m_aafRowCol[2][3], m_aafRowCol[3][2]);
}

Matrix3 Matrix3::GetTransponse() const
{
	Matrix3 kMat3 = *this;
	kMat3.Transponse();
	return kMat3;
}

bool Matrix3::Inverse (Matrix3& inv, float tolerance) const
{
	float det = Determinant();
	
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

float Matrix3::Determinant(void)	 const					
{
	float det;

	det = m_aafRowCol[0][0] * (m_aafRowCol[1][1] * m_aafRowCol[2][2] - m_aafRowCol[1][2] * m_aafRowCol[2][1])
		- m_aafRowCol[1][0] * (m_aafRowCol[0][1] * m_aafRowCol[2][2] - m_aafRowCol[0][2] * m_aafRowCol[2][1])
		+ m_aafRowCol[2][0] * (m_aafRowCol[0][1] * m_aafRowCol[1][2] - m_aafRowCol[0][2] * m_aafRowCol[1][1]);

	return det;
}

void Matrix3::Scale(float fX, float fY, float fZ)
{
	*this *= Matrix3(fX		,0 	,0		,
						  0		,fY	,0		,
						  0		,0		,fZ	);;
}

void Matrix3::Scale(Vector3 kScale){
	Scale(kScale.x, kScale.y, kScale.z);
}


void Matrix3::Rotate(float fX, float fY, float fZ)
{
	RadRotate(DegToRad(fX),DegToRad(fY),DegToRad(fZ));
}


void Matrix3::Rotate(Vector3 kRot){
	RadRotate(DegToRad(kRot.x), DegToRad(kRot.y), DegToRad(kRot.z));
}

void Matrix3::RadRotate(float fX, float fY, float fZ)
{
	static Matrix3 rotatex,rotatey,rotatez;
	
	float cx = float(cos(fX));
	float sx = float(sin(fX));
	
	float cy = float(cos(fY));
	float sy = float(sin(fY));	
	
	float cz = float(cos(fZ));
	float sz = float(sin(fZ));

	 rotatex.Set(1		,0		,0,
					0		,cx	,-sx,
					0		,sx	,cx);	
												 
	 rotatey.Set(cy	,0		,sy,
					0		,1		,0	,
					-sy	,0		,cy);	
												 
	 rotatez.Set(cz	,-sz	,0,
					sz		,cz	,0,
					0		,0		,1);	
						 
	 
	 *this *= rotatez*rotatey*rotatex;
}

void Matrix3::RadRotate(Vector3 kRot){
	RadRotate(kRot.x, kRot.y, kRot.z);
}

// Accessors 
Vector3 Matrix3::GetRotVector()
{
	float D;
	float C;
	float angle_x;
	float angle_y;				
	float angle_z;
	float ftrx;
	float ftry;
	
	
	angle_y = D = float(-asin( m_afData[2]));
	C           = float(cos( angle_y ));
	angle_y    *= degtorad;
    
	ftrx      =  m_afData[7] / C;
	ftry      = -m_afData[6]  / C;
	angle_x  = float( atan2( ftry, ftrx ) * degtorad );
	ftrx      =  m_afData[0] / C;
	ftry      = -m_afData[1] / C;
	angle_z  = float(atan2( ftry, ftrx ) * degtorad );


	angle_x = Clamp( angle_x, 0, 360 );
	angle_y = Clamp( angle_y, 0, 360 );
	angle_z = Clamp( angle_z, 0, 360 );

	return Vector3(angle_x,angle_y,angle_z);
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







