#include "cmath"
#include "quaternion.h"

using namespace std;

#pragma warning(disable : 4244)		// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable : 4305)		// 'identifier' : truncation from 'type1' to 'type2'

#define SLERP_DELA 0.01

Quaternion Quaternion::ZERO(0.0,0.0,0.0,0.0);
Quaternion Quaternion::IDENTITY(1.0,0.0,0.0,0.0);

Quaternion::Quaternion()
{

}

Quaternion::Quaternion( float fW, float fX, float fY , float fZ)
{
	w = fW;
	x = fX;
	y = fY;
	z = fZ;
}

Quaternion::Quaternion(const Quaternion& rkQ)
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
}

Quaternion& Quaternion::operator= ( Quaternion& rkQ )
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
	return *this;
}


Quaternion Quaternion::operator + (const Quaternion& rkQ ) const 
{
	return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
}

Quaternion Quaternion::operator - (const Quaternion& rkQ ) const 
{
	return Quaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
}

Quaternion Quaternion::operator += (const Quaternion& rkQ )
{
    w += rkQ.w;
    x += rkQ.x;
    y += rkQ.y;
    z += rkQ.z;
	return *this;
}

Quaternion Quaternion::operator -= (const Quaternion& rkQ )
{
    w -= rkQ.w;
    x -= rkQ.x;
    y -= rkQ.y;
    z -= rkQ.z;
	return *this;
}

Quaternion Quaternion::operator * (const Quaternion& rkQ) const
{
    return Quaternion
    (
		w*rkQ.w - x*rkQ.x - y*rkQ.y - z*rkQ.z,
		w*rkQ.x + x*rkQ.w + y*rkQ.z - z*rkQ.y,
		w*rkQ.y + y*rkQ.w + z*rkQ.x - x*rkQ.z,
		w*rkQ.z + z*rkQ.w + x*rkQ.y - y*rkQ.x
    );

/*	Quaternion qtmp;
	
    qtmp.w		  = w * q.w		   - vector.x * q.vector.x - vector.y * q.vector.y - vector.z * q.vector.z;
    qtmp.vector.x = w * q.vector.x + vector.x * q.w		   + vector.y * q.vector.z - vector.z * q.vector.y;
    qtmp.vector.y = w * q.vector.y + vector.y * q.w        + vector.z * q.vector.x - vector.x * q.vector.z;
    qtmp.vector.z = w * q.vector.z + vector.z * q.w        + vector.x * q.vector.y - vector.y * q.vector.x;
	return qtmp;*/
}

Quaternion Quaternion::operator *= (const Quaternion& rkQ) 
{
    Quaternion res
    (
		w*rkQ.w - x*rkQ.x - y*rkQ.y - z*rkQ.z,
		w*rkQ.x + x*rkQ.w + y*rkQ.z - z*rkQ.y,
		w*rkQ.y + y*rkQ.w + z*rkQ.x - x*rkQ.z,
		w*rkQ.z + z*rkQ.w + x*rkQ.y - y*rkQ.x
    );

	*this = res;
	return res;
/*	Quaternion qtmp;
	
    qtmp.w		  = w * q.w		   - vector.x * q.vector.x - vector.y * q.vector.y - vector.z * q.vector.z;
    qtmp.vector.x = w * q.vector.x + vector.x * q.w		   + vector.y * q.vector.z - vector.z * q.vector.y;
    qtmp.vector.y = w * q.vector.y + vector.y * q.w        + vector.z * q.vector.x - vector.x * q.vector.z;
    qtmp.vector.z = w * q.vector.z + vector.z * q.w        + vector.x * q.vector.y - vector.y * q.vector.x;
	*this = qtmp;
	return qtmp;*/
}


Quaternion Quaternion::operator* (float fScalar) const
{
    return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
}

Quaternion operator* (float fScalar, const Quaternion& rkQ)
{
    return Quaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y, fScalar*rkQ.z);
}

Quaternion Quaternion::operator- () const
{
    return Quaternion(-w,-x,-y,-z);
}

float Quaternion::Dot (const Quaternion& rkQ) const
{
	return w*rkQ.w + x*rkQ.x + y*rkQ.y + z*rkQ.z;
}

float Quaternion::Norm(void)
{
    return w*w + x*x + y*y + z*z;
}

float Quaternion::Length(void)
{
	return sqrt(Norm());
}

Quaternion Quaternion::Inverse (void) 
{
    float fNorm = Norm();

    if ( fNorm > 0.0f )
    {
        float fInvNorm = 1.0f / fNorm;
        return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
    }
    else
    {
        // return an invalid result to flag the error
        return ZERO;
    }
}

void Quaternion::Normalize(void)
{
    float fNorm = Norm();
    if ( fNorm > 0.0f )
    {
        float fInvNorm = 1.0f / fNorm;
        w *= fInvNorm;
        x *= fInvNorm;
        y *= fInvNorm;
        z *= fInvNorm;
    }
}

Quaternion Quaternion::UnitInverse (void)
{
   // this must be unit length.
   return Quaternion (w,-x,-y,-z);
}





// Compare
int Quaternion::operator== ( const Quaternion& rkQ ) const
{
	return (rkQ.x==x && rkQ.y==y && rkQ.z==z && rkQ.w == w);
}

int Quaternion::operator!= ( const Quaternion& rkQ ) const
{
	return !(rkQ == *this);
}


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
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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





float& Quaternion::operator[] (int i) 
{
	if(i==0)	return x;
	if(i==1)	return y;
	if(i==2)	return z;
	if(i==3)	return w;
	return x;
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
/*	int i;
	float omega, cosom, sinom, sclp, sclq;

	// decide if one of the quaternions is backwards
	float a = 0;
	float b = 0;
	for (i = 0; i < 4; i++) {
		a += (p[i]-q[i])*(p[i]-q[i]);
		b += (p[i]+q[i])*(p[i]+q[i]);
	}
	if (a > b) {
		for (i = 0; i < 4; i++) {
			q[i] = -q[i];
		}
	}

	cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];

	if ((1.0 + cosom) > 0.00000001) {
		if ((1.0 - cosom) > 0.00000001) {
			omega = acos( cosom );
			sinom = sin( omega );
			sclp = sin( (1.0 - t)*omega) / sinom;
			sclq = sin( t*omega ) / sinom;
		}
		else {
			sclp = 1.0 - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++) {
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else {
		qt[0] = -p[1];
		qt[1] = p[0];
		qt[2] = -p[3];
		qt[3] = p[2];
		sclp = sin( (1.0 - t) * 0.5 * Q_PI);
		sclq = sin( t * 0.5 * Q_PI);
		for (i = 0; i < 3; i++) {
			qt[i] = sclp * p[i] + sclq * qt[i];
		}
	}*/

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



Quaternion Quaternion::conjugate(void)
{
	return Quaternion(
		-x,
		-y,
		-z,
		w);
/*	Quaternion q;

	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
	q.w = w;

	return q;*/
}







/*


Quaternion Quaternion::conjugate(void)
{
	Quaternion q;

	q.vector = -vector;
	q.w = w;

	return q;
}

void Quaternion::make(Vector3 axis, float angle)
{
	w = cos( angle / 2);

	float sin_a = sin( angle / 2);
	vector.x = axis.x * sin_a;
	vector.y = axis.y * sin_a;
	vector.z = axis.z * sin_a;
	normalize();
}

void Quaternion::get(Vector3& axis, float& angle)
{
	normalize();
	float cos_angle = w;
	angle = acos(cos_angle) * 2;
	float sin_angle = sqrt(1.0 - cos_angle * cos_angle);

    if ( fabs( sin_angle ) < 0.0005 )	sin_angle = 1;

    axis.x = vector.x / sin_angle;
    axis.y = vector.y / sin_angle;
    axis.z = vector.z / sin_angle;
}

void Quaternion::print(void)
{
//	cout << "<" << vector.x << "," << vector.y<< "," << vector.z << "," << w << ">" << endl;
}

void Quaternion::rotate(float x, float y, float z)
{
	Matrix4 mat;
	mat.Identity();
	//mat.RotateMatris(x,y,z);
	//*this = mat; 
}

void Quaternion::assign_index(int index, float value)
{
	if(index == 0)	vector.x = value;
	if(index == 1)	vector.y = value;
	if(index == 2)	vector.z = value;
	if(index == 3)	w = value;
}

Quaternion Quaternion::operator * ( const Quaternion& s ) const
{
	Quaternion res;

	res.w = w*s.w - vector.dot(s.vector);
	res.vector = vector.cross(s.vector) + s.w * vector + w * s.vector ;
//	res.normalize();

	return res;
}



// Spherical Linear intERPolation (SLERP):
void QuatSlerp(Quaternion * from, Quaternion * to, float t, Quaternion * res)
{
	float	to1[4];
	double  omega, cosom, sinom, scale0, scale1;

	// calc cosine (a dot product?)
	cosom = from->vector.x * to->vector.x + 
		from->vector.y * to->vector.y + 
		from->vector.z * to->vector.z + 
		from->w * to->w;

	// adjust signs (if necessary)
	if ( cosom < 0.0 ) { 
		cosom = -cosom; 
		to1[0] = - to->vector.x;
		to1[1] = - to->vector.y;
		to1[2] = - to->vector.z;
		to1[3] = - to->w;
		}
	else  {
		to1[0] = to->vector.x;
		to1[1] = to->vector.y;
		to1[2] = to->vector.z;
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
	res->vector.x = scale0 * from->vector.x + scale1 * to1[0];
	res->vector.y = scale0 * from->vector.y + scale1 * to1[1];
	res->vector.z = scale0 * from->vector.z + scale1 * to1[2];
	res->w = scale0 * from->w + scale1 * to1[3];
}




  */
  
  
  
  
  
  
  
  
  
  

