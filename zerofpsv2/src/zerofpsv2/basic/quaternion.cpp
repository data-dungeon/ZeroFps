#include "cmath"
#include "math.h"

using namespace std;

#pragma warning(disable : 4244)		// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable : 4305)		// 'identifier' : truncation from 'type1' to 'type2'

#define SLERP_DELA 0.01

Quaternion Quaternion::ZERO(0.0,0.0,0.0,0.0);
Quaternion Quaternion::IDENTITY(1.0,0.0,0.0,0.0);

Vector3 Quaternion::RotateVector3(const Vector3& v) const
{
	Quaternion t;
	
	t = (*this) * v * conjugate();
	
	return t.GetVector3();
}

string Quaternion::ToString()
{
	char szVec[128];
	sprintf(szVec, "%f,%f,%f,%f", x,y,z,w);
	return string(szVec);	
}

void Quaternion::FromRotationMatrix (const Matrix3& kRot)
{
    float fTrace = kRot.m_aafRowCol[0][0] + kRot.m_aafRowCol[1][1] + kRot.m_aafRowCol[2][2];
    float fRoot;

    if ( fTrace > 0.0f )
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = sqrt(fTrace + 1.0f);			// 2w
        w = 0.5f * fRoot;
        fRoot = 0.5f / fRoot;						// 1/(4w)
        x = (kRot.m_aafRowCol[2][1] - kRot.m_aafRowCol[1][2])*fRoot;
        y = (kRot.m_aafRowCol[0][2] - kRot.m_aafRowCol[2][0])*fRoot;
        z = (kRot.m_aafRowCol[1][0] - kRot.m_aafRowCol[0][1])*fRoot;
    }
    else
    {
        // |w| <= 1/2
        static int s_iNext[3] = { 1, 2, 0 };
        int i = 0;
        if ( kRot.m_aafRowCol[1][1] > kRot.m_aafRowCol[0][0] )
            i = 1;
        if ( kRot.m_aafRowCol[2][2] > kRot.m_aafRowCol[i][i] )
            i = 2;
        int j = s_iNext[i];
        int k = s_iNext[j];

        fRoot = sqrt(kRot.m_aafRowCol[i][i]-kRot.m_aafRowCol[j][j]-kRot.m_aafRowCol[k][k] + 1.0f);
        float* apkQuat[3] = { &x, &y, &z };
        *apkQuat[i] = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;
        w = (kRot.m_aafRowCol[k][j]-kRot.m_aafRowCol[j][k])*fRoot;
        *apkQuat[j] = (kRot.m_aafRowCol[j][i]+kRot.m_aafRowCol[i][j])*fRoot;
        *apkQuat[k] = (kRot.m_aafRowCol[k][i]+kRot.m_aafRowCol[i][k])*fRoot;
    }
}

void Quaternion::ToRotationMatrix (Matrix3& kRot) const
{
    float fTx  = 2.0f*x;
    float fTy  = 2.0f*y;
    float fTz  = 2.0f*z;
    float fTwx = fTx*w;
    float fTwy = fTy*w;
    float fTwz = fTz*w;
    float fTxx = fTx*x;
    float fTxy = fTy*x;
    float fTxz = fTz*x;
    float fTyy = fTy*y;
    float fTyz = fTz*y;
    float fTzz = fTz*z;

	kRot.m_aafRowCol[0][0] = 1.0f-(fTyy+fTzz);
    kRot.m_aafRowCol[0][1] = fTxy-fTwz;
    kRot.m_aafRowCol[0][2] = fTxz+fTwy;
    kRot.m_aafRowCol[1][0] = fTxy+fTwz;
    kRot.m_aafRowCol[1][1] = 1.0f-(fTxx+fTzz);
    kRot.m_aafRowCol[1][2] = fTyz-fTwx;
    kRot.m_aafRowCol[2][0] = fTxz-fTwy;
    kRot.m_aafRowCol[2][1] = fTyz+fTwx;
    kRot.m_aafRowCol[2][2] = 1.0f-(fTxx+fTyy);
}

void Quaternion::FromRotationMatrix (const Matrix4& kRot)
{
    float fTrace = kRot.RowCol[0][0] + kRot.RowCol[1][1] + kRot.RowCol[2][2];
    float fRoot;

    if ( fTrace > 0.0f )
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = sqrt(fTrace + 1.0f);			// 2w
        w = 0.5f * fRoot;
        fRoot = 0.5f / fRoot;						// 1/(4w)
        x = (kRot.RowCol[2][1] - kRot.RowCol[1][2])*fRoot;
        y = (kRot.RowCol[0][2] - kRot.RowCol[2][0])*fRoot;
        z = (kRot.RowCol[1][0] - kRot.RowCol[0][1])*fRoot;
    }
    else
    {
        // |w| <= 1/2
        static int s_iNext[3] = { 1, 2, 0 };
        int i = 0;
        if ( kRot.RowCol[1][1] > kRot.RowCol[0][0] )
            i = 1;
        if ( kRot.RowCol[2][2] > kRot.RowCol[i][i] )
            i = 2;
        int j = s_iNext[i];
        int k = s_iNext[j];

        fRoot = sqrt(kRot.RowCol[i][i]-kRot.RowCol[j][j]-kRot.RowCol[k][k] + 1.0f);
        float* apkQuat[3] = { &x, &y, &z };
        *apkQuat[i] = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;
        w = (kRot.RowCol[k][j]-kRot.RowCol[j][k])*fRoot;
        *apkQuat[j] = (kRot.RowCol[j][i]+kRot.RowCol[i][j])*fRoot;
        *apkQuat[k] = (kRot.RowCol[k][i]+kRot.RowCol[i][k])*fRoot;
    }
}

void Quaternion::ToRotationMatrix (Matrix4& kRot) const
{
    float fTx  = 2.0f*x;
    float fTy  = 2.0f*y;
    float fTz  = 2.0f*z;
    float fTwx = fTx*w;
    float fTwy = fTy*w;
    float fTwz = fTz*w;
    float fTxx = fTx*x;
    float fTxy = fTy*x;
    float fTxz = fTz*x;
    float fTyy = fTy*y;
    float fTyz = fTz*y;
    float fTzz = fTz*z;

	 kRot.RowCol[0][0] = 1.0f-(fTyy+fTzz);
    kRot.RowCol[0][1] = fTxy-fTwz;
    kRot.RowCol[0][2] = fTxz+fTwy;
    kRot.RowCol[1][0] = fTxy+fTwz;
    kRot.RowCol[1][1] = 1.0f-(fTxx+fTzz);
    kRot.RowCol[1][2] = fTyz-fTwx;
    kRot.RowCol[2][0] = fTxz-fTwy;
    kRot.RowCol[2][1] = fTyz+fTwx;
    kRot.RowCol[2][2] = 1.0f-(fTxx+fTyy);
}

void Quaternion::FromAngleAxis (const float& rfAngle, const Vector3& rkAxis)
{
    // assert:  axis[] is unit length
    //
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    float fHalfAngle = 0.5f*rfAngle;
    float fSin = sin(fHalfAngle);
    w = cos(fHalfAngle);
    x = fSin*rkAxis.x;
    y = fSin*rkAxis.y;
    z = fSin*rkAxis.z;
}

void Quaternion::ToAngleAxis (float& rfAngle, Vector3& rkAxis) const
{
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

/*    float fSqrLength = x*x+y*y+z*z;
    if ( fSqrLength > 0.0f )
    {
        rfAngle = 2.0f*acos(w);
        Real fInvLength = Math::InvSqrt(fSqrLength);
        rkAxis.x = x*fInvLength;
        rkAxis.y = y*fInvLength;
        rkAxis.z = z*fInvLength;
    }
    else
    {
        // angle is 0 (mod 2*pi), so any axis will do
        rfAngle = 0.0f;
        rkAxis.x = 1.0f;
        rkAxis.y = 0.0f;
        rkAxis.z = 0.0f;
    }*/
}

void Quaternion::FromAxes (const Vector3* akAxis)
{
    Matrix3 kRot;

    for (int iCol = 0; iCol < 3; iCol++)
    {
        kRot.m_aafRowCol[0][iCol] = akAxis[iCol].x;
        kRot.m_aafRowCol[1][iCol] = akAxis[iCol].y;
        kRot.m_aafRowCol[2][iCol] = akAxis[iCol].z;
    }

    FromRotationMatrix(kRot);
}

void Quaternion::ToAxes (Vector3* akAxis) const
{
    Matrix3 kRot;

    ToRotationMatrix(kRot);

    for (int iCol = 0; iCol < 3; iCol++)
    {
        akAxis[iCol].x = kRot.m_aafRowCol[0][iCol];
        akAxis[iCol].y = kRot.m_aafRowCol[1][iCol];
        akAxis[iCol].z = kRot.m_aafRowCol[2][iCol];
    }
}

void Quaternion::AngleQuaternion( const Vector3 angles )
{
/*	Quaternion qX,qY,qZ;

	qX.FromAngleAxis(angles.x,Vector3(1,0,0));
	qY.FromAngleAxis(angles.y,Vector3(0,1,0));
	qZ.FromAngleAxis(angles.z,Vector3(0,0,1));

	*this = qX * qY * qZ;*/


	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle	= angles.z * 0.5;
	sy		= sin(angle);
	cy		= cos(angle);
	angle	= angles.y * 0.5;
	sp		= sin(angle);
	cp		= cos(angle);
	angle	= angles.x * 0.5;
	sr		= sin(angle);
	cr		= cos(angle);

	x = sr*cp*cy - cr*sp*sy; 
	y = cr*sp*cy + sr*cp*sy; 
	z = cr*cp*sy - sr*sp*cy; 
	w = cr*cp*cy + sr*sp*sy; 
}

void Quaternion::QuaternionSlerp( Quaternion* from, Quaternion* to, float t)
{
	float	to1[4];
	double  omega, cosom, sinom, scale0, scale1;

	// calc cosine (a dot product?)
	cosom = from->x * to->x + 
		from->y * to->y + 
		from->z * to->z + 
		from->w * to->w;

	// adjust signs (if necessary)
	if ( cosom < 0.0 ) { 
		cosom = -cosom; 
		to1[0] = - to->x;
		to1[1] = - to->y;
		to1[2] = - to->z;
		to1[3] = - to->w;
		}
	else  {
		to1[0] = to->x;
		to1[1] = to->y;
		to1[2] = to->z;
		to1[3] = to->w;
		}

	// calculate coefficients
	if ( (1.0 - cosom) > SLERP_DELA ) {
		// standard case (slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0 - t) * omega) / sinom;
		scale1 = sin(t * omega) / sinom;
		}

	else {        
		// "from" and "to" quaternions are very close so we can do a linear interpolation
		scale0 = 1.0 - t;
		scale1 = t;
	}

	// calculate final values
	x = scale0 * from->x + scale1 * to1[0];
	y = scale0 * from->y + scale1 * to1[1];
	z = scale0 * from->z + scale1 * to1[2];
	w = scale0 * from->w + scale1 * to1[3];
}

//globals--
Quaternion operator*(const Quaternion& q,const Vector3& v)
{
	return Quaternion(	-(q.x*v.x + q.y*v.y + q.z*v.z),
								  q.w*v.x + q.y*v.z - q.z*v.y,
								  q.w*v.y + q.z*v.x - q.x*v.z,
								  q.w*v.z + q.x*v.y - q.y*v.x);
}

Quaternion operator*(const Vector3& v,const Quaternion& q)
{
	return Quaternion(	-(q.x*v.x + q.y*v.y + q.z*v.z),
								  q.w*v.x + q.z*v.y - q.y*v.z,
								  q.w*v.y + q.x*v.z - q.z*v.x,
								  q.w*v.z + q.y*v.x - q.x*v.y);
}


  
  







/*
Quaternion& Quaternion::operator= (Quaternion& rkQ )
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
	return *this;
}
*/

// conversion between quaternions, matrices, and angle-axes
/*void Quaternion::operator = ( Matrix4& m )
{
    float root;
	float trace = m.RowCol[0][0] + m.RowCol[1][1] + m.RowCol[2][2];

	if(trace > 0.0) {
        root = sqrt (trace + 1.0);
        w = root * 0.5;
        root = 0.5 / root;

        vector.x = (m.RowCol[1][2] - m.RowCol[2][1]) * root;
        vector.y = (m.RowCol[2][0] - m.RowCol[0][2]) * root;
        vector.z = (m.RowCol[0][1] - m.RowCol[1][0]) * root;
		}
	else {
        int i, j, k, nxt[3] = {1,2,0};

        i = 0;
        if (m.RowCol[1][1] > m.RowCol[0][0])	i = 1;
        if (m.RowCol[2][2] > m.RowCol[i][i])	i = 2;
        j = nxt[i];
        k = nxt[j];
        
		root = sqrt((m.RowCol[i][i] - (m.RowCol[j][j] + m.RowCol[k][k])) + 1.0);
        assign_index(i, root * 0.5); 
        root = 0.5 / root;
        w = (m.RowCol[j][k] - m.RowCol[k][j]) * root;
        assign_index(j, (m.RowCol[i][j] + m.RowCol[j][i]) * root);	
        assign_index(k, (m.RowCol[i][k] + m.RowCol[k][i]) * root);
		}
}
*/
