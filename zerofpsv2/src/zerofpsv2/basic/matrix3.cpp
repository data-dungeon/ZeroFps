#include "zfassert.h"
#include <cmath>
#include "math.h"

using namespace std;

const Matrix3 Matrix3::ZERO(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f);
const Matrix3 Matrix3::IDENTITY(1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f);

// Methods
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
	static Matrix3 kMat3;
	
	kMat3 = *this;
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
	static Matrix3 kMatrix(	0,0,0,
									0,0,0,
									0,0,0);
	
	kMatrix.m_aafRowCol[0][0] = fX;		
	kMatrix.m_aafRowCol[1][1] = fY;		
	kMatrix.m_aafRowCol[2][2] = fZ;			
	
	*this *= kMatrix;	


// 	*this *= Matrix3(fX		,0 	,0		,
// 						  0		,fY	,0		,
// 						  0		,0		,fZ	);;
}

void Matrix3::Scale(const Vector3& kScale)
{
	Scale(kScale.x, kScale.y, kScale.z);
}


void Matrix3::Rotate(float fX, float fY, float fZ)
{
	RadRotate(Math::DegToRad(fX),Math::DegToRad(fY),Math::DegToRad(fZ));
}


void Matrix3::Rotate(const Vector3& kRot)
{
	RadRotate(Math::DegToRad(kRot.x), Math::DegToRad(kRot.y), Math::DegToRad(kRot.z));
}

void Matrix3::RadRotate(float fX, float fY, float fZ)
{
	static Matrix3 rotatex,rotatey,rotatez;
	static float cx,sx,cy,sy,cz,sz;
	
	cx = float(cos(fX));
	sx = float(sin(fX));
	
	cy = float(cos(fY));
	sy = float(sin(fY));	
	
	cz = float(cos(fZ));
	sz = float(sin(fZ));

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

void Matrix3::RadRotate(const Vector3& kRot){
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
	angle_y     = Math::RadToDeg(angle_y);
    
	ftrx      =  m_afData[7] / C;
	ftry      = -m_afData[6]  / C;
	angle_x   = Math::RadToDeg(float( atan2( ftry, ftrx )));
	ftrx      =  m_afData[0] / C;
	ftry      = -m_afData[1] / C;
	angle_z   = Math::RadToDeg(float(atan2( ftry, ftrx )));


	angle_x = Math::Clamp( angle_x, 0, 360 );
	angle_y = Math::Clamp( angle_y, 0, 360 );
	angle_z = Math::Clamp( angle_z, 0, 360 );

	return Vector3(angle_x,angle_y,angle_z);
}

string Matrix3::ToString()
{
	char szMat[512];
	szMat[0] = 0;

	for(int i=0; i<9; i++)
	{
		sprintf(&szMat[strlen(szMat)], "%.2f, ", m_afData[i]);
	}

	return string(szMat);
};




