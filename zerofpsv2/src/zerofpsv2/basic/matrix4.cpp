#include "matrix4.h"
#include "matrix3.h"
#include <math.h>
#include <cstdlib>
#include <cmath>

// Constructors
Matrix4::Matrix4(void) 
{
	for(int i=0;i<16;i++)
		data[i]=0;
}

Matrix4::Matrix4( float v1,float v2,float v3 ,float v4,
						float v5,float v6,float v7 ,float v8,						
						float v9,float v10,float v11 ,float v12,
						float v13,float v14,float v15 ,float v16)
{							 	 
	data[0]=v1;		data[1]=v2;		data[2]=v3;		data[3]=v4;
	data[4]=v5;		data[5]=v6;		data[6]=v7;		data[7]=v8;
	data[8]=v9;		data[9]=v10;	data[10]=v11;	data[11]=v12;	
	data[12]=v13;	data[13]=v14;	data[14]=v15;	data[15]=v16;	
};

Matrix4::Matrix4(	const Matrix3& kOther)
{
	data[0]=kOther.m_afData[0];	data[1]=kOther.m_afData[1];	data[2]=kOther.m_afData[2];	data[3]=0;
	data[4]=kOther.m_afData[3];	data[5]=kOther.m_afData[4];	data[6]=kOther.m_afData[5];	data[7]=0;
	data[8]=kOther.m_afData[6];	data[9]=kOther.m_afData[7];	data[10]=kOther.m_afData[8];	data[11]=0;	
	data[12]=0;							data[13]=0;							data[14]=0;							data[15]=1;	
}

void Matrix4::Set( float v1,float v2,float v3 ,float v4,
						float v5,float v6,float v7 ,float v8,						
						float v9,float v10,float v11 ,float v12,
						float v13,float v14,float v15 ,float v16)
{							 	 
	data[0]=v1;		data[1]=v2;		data[2]=v3;		data[3]=v4;
	data[4]=v5;		data[5]=v6;		data[6]=v7;		data[7]=v8;
	data[8]=v9;		data[9]=v10;	data[10]=v11;	data[11]=v12;	
	data[12]=v13;	data[13]=v14;	data[14]=v15;	data[15]=v16;	
};

	
// Operators	-	Assignment
Matrix4 Matrix4::operator=(const Matrix4 &kOther)
{
	memcpy(data,kOther.data,sizeof(data));
		
	return *this;
}

void Matrix4::operator=(const Matrix3 &rkMatrix)
{

	Identity();

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


// Operators	-	Comparison
bool Matrix4::operator== (const Matrix4& rkMatrix) const
{
	if(memcmp(data,rkMatrix.data,sizeof(data)) == 0)
		return true;

	return false;
}

bool Matrix4::operator!= (const Matrix4& rkMatrix) const
{
    return !operator==(rkMatrix);
}

// Operators	-	Arithmetic operations
Matrix4 Matrix4::operator+ (const Matrix4& rkMatrix) const
{
    Matrix4 kSum;
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

Matrix4 Matrix4::operator+= (const Matrix4& rkMatrix)
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

Matrix4 Matrix4::operator- (const Matrix4& rkMatrix) const
{
    Matrix4 kDiff;
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

Matrix4 Matrix4::operator-= (const Matrix4& rkMatrix)
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

Matrix4 Matrix4::operator*(const Matrix4 &kOther) const 
{
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
}

Matrix4 Matrix4::operator*= (const Matrix4& rkMatrix)
{
    Matrix4 kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            kProd.RowCol[iRow][iCol] =
                RowCol[iRow][0]*rkMatrix.RowCol[0][iCol] +
                RowCol[iRow][1]*rkMatrix.RowCol[1][iCol] +
                RowCol[iRow][2]*rkMatrix.RowCol[2][iCol] +
                RowCol[iRow][3]*rkMatrix.RowCol[3][iCol];
        }
    }

	*this = kProd;
    return kProd;

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

Vector4 Matrix4::operator*(const Vector4 &f)
{
	Vector4 ny;
	
	ny.x=data[0]*f.x + data[1]*f.y + data[2]*f.z + data[3]*f.w;
	ny.y=data[4]*f.x + data[5]*f.y + data[6]*f.z + data[7]*f.w;
	ny.z=data[8]*f.x + data[9]*f.y + data[10]*f.z + data[11]*f.w;
	ny.w=data[12]*f.x + data[13]*f.y + data[14]*f.z + data[15]*f.w;

	return ny;

}



// Methods
void Matrix4::Zero(void) 
{
	for(int i=0;i<16;i++)
		data[i]=0;
}


void Matrix4::Identity() 
{
	*this=Matrix4(1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1);
}

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
	RadRotate(DegToRad(fX),DegToRad(fY),DegToRad(fZ));
}

void Matrix4::Rotate(Vector3 kRot){
	RadRotate(DegToRad(kRot.x), DegToRad(kRot.y), DegToRad(kRot.z));
}

void Matrix4::RadRotate(float fX, float fY, float fZ)
{
	static Matrix4 rotatex,rotatey,rotatez;	


	float cx = float(cos(fX));
	float sx = float(sin(fX));
	
	float cy = float(cos(fY));
	float sy = float(sin(fY));	
	
	float cz = float(cos(fZ));
	float sz = float(sin(fZ));


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

void Matrix4::RadRotate(Vector3 kRot){
	RadRotate(kRot.x, kRot.y, kRot.z);
}

void Matrix4::Scale(float fX, float fY, float fZ)
{
	*this *= Matrix4(fX		,0 	,0		,0,
						  0		,fY	,0		,0,
						  0		,0		,fZ	,0,
						  0		,0		,0		,1);;
}

void Matrix4::Scale(Vector3 kScale){
	Scale(kScale.x, kScale.y, kScale.z);
}


void Matrix4::Translate(float fX, float fY, float fZ)
{
	*this *= Matrix4(	1		,0 	,0		,0		,
							0		,1		,0		,0		,
							0		,0		,1		,0		,
							fX		,fY	,fZ	,1		);
}

void Matrix4::Translate(Vector3 kTrans)
{
	Translate(kTrans.x, kTrans.y, kTrans.z);
}

void Matrix4::LookDir(Vector3 kDir,Vector3 kUp)
{
	Vector3 kRight;
	
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



Vector3 Matrix4::VectorIRotate (const Vector3& kVec)
{
	Vector3 res;
	
	res.x = kVec.x * RowCol[0][0] + kVec.y * RowCol[1][0] + kVec.z * RowCol[2][0];
	res.y = kVec.x * RowCol[0][1] + kVec.y * RowCol[1][1] + kVec.z * RowCol[2][1];
	res.z = kVec.x * RowCol[0][2] + kVec.y * RowCol[1][2] + kVec.z * RowCol[2][2];
	return res;
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
	float det, result = 0, i = 1;
	Matrix3 msub;
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
	Matrix3 b;
	
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
	Matrix4	inverse;

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
	angle_y    *= degtorad;
    
	ftrx      =  data[9] / C;
	ftry      = -data[8]  / C;
	angle_x  = float(atan2( ftry, ftrx ) * degtorad);
	ftrx      =  data[0] / C;
	ftry      = -data[1] / C;
	angle_z  = float(atan2( ftry, ftrx ) * degtorad);

	angle_x = Clamp( angle_x, 0, 360 );
	angle_y = Clamp( angle_y, 0, 360 );
	angle_z = Clamp( angle_z, 0, 360 );
	

	return Vector3(angle_x,angle_y,angle_z);
	
}


Vector3 Matrix4::GetPosVector()
{
	return Vector3(RowCol[3][0],RowCol[3][1],RowCol[3][2]);
}

float &Matrix4::operator[](const int i)
{
	return data[i];
}

void Matrix4::SetAxis(int iAxisNum, Vector3 kNewAxis)
{
        RowCol[iAxisNum][0] = kNewAxis.x;
        RowCol[iAxisNum][1] = kNewAxis.y;
        RowCol[iAxisNum][2] = kNewAxis.z;
}

Vector3 Matrix4::GetAxis(int iAxisNum)
{
	return Vector3(RowCol[iAxisNum][0], RowCol[iAxisNum][1], RowCol[iAxisNum][2]);
}


// Other
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









