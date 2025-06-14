#include <math.h>
#include <cstdlib>
#include <cmath>
#include "math.h"

// Methods
void Matrix4::Transponse()
{
	swap(RowCol[0][1], RowCol[1][0]);
	swap(RowCol[0][2], RowCol[2][0]);
	swap(RowCol[0][3], RowCol[3][0]);
	
	swap(RowCol[1][2], RowCol[2][1]);
	swap(RowCol[1][3], RowCol[3][1]);

	swap(RowCol[2][3], RowCol[3][2]);

}

void Matrix4::OldTranslate(float x, float y, float z)
{
	Identity();
	SetPos(Vector3(x,y,z));
}

void Matrix4::Rotate(float fX, float fY, float fZ)
{
	RadRotate(Math::DegToRad(fX),Math::DegToRad(fY),Math::DegToRad(fZ));
}

void Matrix4::Rotate(const Vector3& kRot)
{
	RadRotate(Math::DegToRad(kRot.x), Math::DegToRad(kRot.y), Math::DegToRad(kRot.z));
}

void Matrix4::RadRotate(float fX, float fY, float fZ)
{
	static Matrix4 rotatex,rotatey,rotatez;	
	static float cx,sx,cy,sy,cz,sz;

	cx = float(cos(fX));
	sx = float(sin(fX));
	
	cy = float(cos(fY));
	sy = float(sin(fY));	
	
	cz = float(cos(fZ));
	sz = float(sin(fZ));


	rotatex.Set(1			,0			,0				,0,
					0			,cx		,-sx			,0,
					0			,sx		,cx	  		,0,
					0			,0			,0				,1);	
												 
	rotatey.Set(cy			,0			,sy		   ,0,
					0			,1			,0				,0,
					-sy	 	,0			,cy			,0,
					0			,0			,0				,1);	
												 
	rotatez.Set(cz			,-sz		,0				,0,
					sz			,cz		,0				,0,
					0			,0			,1				,0,
					0			,0			,0				,1);	
						 
									
	 *this *= rotatez*rotatey*rotatex;
}

void Matrix4::RadRotate(const Vector3& kRot)
{
	RadRotate(kRot.x, kRot.y, kRot.z);
}

void Matrix4::Scale(float fX, float fY, float fZ)
{
	static Matrix4 kMatrix(	0,0,0,0,
									0,0,0,0,
									0,0,0,0,
									0,0,0,1);
	
	kMatrix.RowCol[0][0] = fX;		
	kMatrix.RowCol[1][1] = fY;		
	kMatrix.RowCol[2][2] = fZ;			
	
	*this *= kMatrix;

}

void Matrix4::Scale(const Vector3& kScale)
{
	Scale(kScale.x, kScale.y, kScale.z);
}

void Matrix4::Scale(float fScale)
{
	Scale(fScale,fScale,fScale);
}


void Matrix4::Translate(float fX, float fY, float fZ)
{
	static Matrix4 kMatrix(	1,0,0,0,
									0,1,0,0,
									0,0,1,0,
									0,0,0,1);
	
	kMatrix.RowCol[3][0] = fX;		
	kMatrix.RowCol[3][1] = fY;		
	kMatrix.RowCol[3][2] = fZ;			
	
	*this *= kMatrix;
}

void Matrix4::Translate(const Vector3& kTrans)
{
	Translate(kTrans.x, kTrans.y, kTrans.z);
}

void Matrix4::LookDir(Vector3 kDir,Vector3 kUp)
{
	static Vector3 kRight;
	
	kDir.Normalize();
	kUp.Normalize();

	kRight = kUp.Cross(kDir);
	kRight.Normalize();
	kUp = kDir.Cross(kRight);
	kUp.Normalize();

	Identity();
	SetAxis(0,kRight);
	SetAxis(1,kUp);
	SetAxis(2,kDir);
	Transponse();
}

void Matrix4::SetZeroDelta(float delta)
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            if ( fabs(RowCol[iRow][iCol]) < delta )
				RowCol[iRow][iCol] = 0.0;
        }
    }
}

float Matrix4::Determinant(void)
{
	static Matrix3 msub;

	float det, result = 0, i = 1;
	int n;

	for(n=0; n<4; n++, i*= -1) {
		msub = submat(0,n);
		det = msub.Determinant();
		result += RowCol[0][n] * det * i;
		}

	return result;
}

Matrix3 Matrix4::submat(int i, int j)
{
	static Matrix3 b;
	
	int ti, tj, idst=0, jdst=0;
	
	for(ti = 0; ti < 4; ti++) {
		if( ti < i)	idst = ti;
		 
		else if (ti > i)	idst = ti-1;

		for(tj = 0; tj < 4; tj++) {
			if(tj < j)	jdst = tj;
			else if (tj > j)	jdst = tj - 1;

			if(ti != i && tj != j)
				b.m_aafRowCol[idst][jdst] = RowCol[ti][tj];
			}
		}

	return b;
}

Matrix4 Matrix4::Inverse( )
{
	static Matrix4	inverse;

	float *mat = data;
	float *dst = inverse.data;
	

	float tmp[12]; /* temp array for pairs */
	float src[16]; /* array of transpose source matrix */
	float det; /* determinant */
	/* transpose matrix */
	for ( int i = 0; i < 4; i++) {
	src[i] = mat[i*4];
	src[i + 4] = mat[i*4 + 1];
	src[i + 8] = mat[i*4 + 2];
	src[i + 12] = mat[i*4 + 3];
	}
	/* calculate pairs for first 8 elements (cofactors) */
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];
	/* calculate first 8 elements (cofactors) */
	dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
	dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
	dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
	dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
	dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
	dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
	dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
	dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
	dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
	dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
	dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
	dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
	dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
	dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
	dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
	dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = src[2]*src[7];
	tmp[1] = src[3]*src[6];
	tmp[2] = src[1]*src[7];
	tmp[3] = src[3]*src[5];
	tmp[4] = src[1]*src[6];
	tmp[5] = src[2]*src[5];
	tmp[6] = src[0]*src[7];
	tmp[7] = src[3]*src[4];
	tmp[8] = src[0]*src[6];
	tmp[9] = src[2]*src[4];
	tmp[10] = src[0]*src[5];
	tmp[11] = src[1]*src[4];
	/* calculate second 8 elements (cofactors) */
	dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
	dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
	dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
	dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
	dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
	dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
	dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
	dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
	dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
	dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
	dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
	dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
	dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
	dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
	dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
	dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
	/* calculate determinant */
	det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];
	/* calculate matrix inverse */
	det = 1/det;
	for ( int j = 0; j < 16; j++)
		dst[j] *= det;

	return inverse;
}

// Accessors 
Vector3 Matrix4::GetRotVector()
{
	float D;
	float C;
	float angle_x;
	float angle_y;				
	float angle_z;
	float ftrx;
	float ftry;
	
	
	angle_y = D = float(-asin( data[2]));
	C           =  float(cos( angle_y ));
	angle_y     = Math::DegToRad(angle_y);
    
	ftrx      =  data[9] / C;
	ftry      = -data[8]  / C;
	angle_x   = Math::DegToRad(float(atan2( ftry, ftrx )));
	ftrx      =  data[0] / C;
	ftry      = -data[1] / C;
	angle_z   = Math::DegToRad(float(atan2( ftry, ftrx )));

	angle_x = Math::Clamp( angle_x, 0, 360 );
	angle_y = Math::Clamp( angle_y, 0, 360 );
	angle_z = Math::Clamp( angle_z, 0, 360 );
	

	return Vector3(angle_x,angle_y,angle_z);
}

/*
Vector3 Matrix4::GetPosVector()
{
	return Vector3(RowCol[3][0],RowCol[3][1],RowCol[3][2]);
}
*/

string Matrix4::ToString()
{
	char szMat[512];
	szMat[0] = 0;

	for(int i=0; i<16; i++)
	{
		sprintf(&szMat[strlen(szMat)], "%.2f, ", data[i]);
	}

	return string(szMat);
};

string Matrix4::ToString_Vec(int iIndex)
{
	char szMat[512];
	szMat[0] = 0;

	sprintf(szMat, "%.2f,%.2f,%.2f,%.2f ", vec[iIndex].x, vec[iIndex].y, vec[iIndex].z, vec[iIndex].w);

	return string(szMat);
}


/*
Vector3 Matrix4::VectorRotate (const Vector3& kVec)
{
	Vector3 res;
	
	res.x = kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0];
	res.y = kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1];
	res.z = kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2];
	return res;
}*/

/*
Vector3 Matrix4::VectorTransform (const Vector3& kVec)
{
	Vector3 res;

	res.x = kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0] + RowCol[3][0];
	res.y = kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1] + RowCol[3][1];
	res.z = kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2] + RowCol[3][2];
	


//	res.x = kVec.x * data[0] + kVec.y * data[4] + kVec.z * data[8] + data[12];
//	res.y = kVec.x * data[1] + kVec.y * data[5] + kVec.z * data[9] + data[13];
//	res.z = kVec.x * data[2] + kVec.y * data[6] + kVec.z * data[10] + data[14];
	

	return res;
}*/

/*
bool Matrix4::Inverse(void)
{
	float d = Determinant();
	Matrix3 mtmp;
	Matrix4	calc;

	int i,j,sign;

	if(fabs(d) < 0.0005)	return 0;

	for(i=0; i<4; i++) {
		for(j=0; j<4; j++) {
			sign = 1 - ((i+j) % 2) * 2;
			mtmp = submat(i,j);
		    calc.RowCol[i][j] = (mtmp.Determinant() * sign) / d;
			}
		}

	return true;
}*/









