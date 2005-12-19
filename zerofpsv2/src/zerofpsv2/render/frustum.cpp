#include "frustum.h"
 
Frustum::Frustum()// : ZFSubSystem("Frustum") 
{
	m_iCubeCulls = 0;
  m_iSphereCulls = 0;
  m_iPointCulls = 0;

}

void Frustum::GetFrustum(const Matrix4& kProjectionMatrix,const Matrix4& kModelViewMatrix) 
{
   static	Matrix4   clip;
	
	clip = kModelViewMatrix*kProjectionMatrix;
	  
	// Extract the numbers for the RIGHT plane 
   m_akFrustum[0].x = clip.data[ 3] - clip.data[ 0];
   m_akFrustum[0].y = clip.data[ 7] - clip.data[ 4];
   m_akFrustum[0].z = clip.data[11] - clip.data[ 8];
   m_akFrustum[0].w = clip.data[15] - clip.data[12];

   // Extract the numbers for the LEFT plane 
   m_akFrustum[1].x = clip.data[ 3] + clip.data[ 0];
   m_akFrustum[1].y = clip.data[ 7] + clip.data[ 4];
   m_akFrustum[1].z = clip.data[11] + clip.data[ 8];
   m_akFrustum[1].w = clip.data[15] + clip.data[12];  
  
   // Extract the BOTTOM plane 
   m_akFrustum[2].x = clip.data[ 3] + clip.data[ 1];
   m_akFrustum[2].y = clip.data[ 7] + clip.data[ 5];
   m_akFrustum[2].z = clip.data[11] + clip.data[ 9];
   m_akFrustum[2].w = clip.data[15] + clip.data[13];
   
   
   // Extract the TOP plane 
   m_akFrustum[3].x = clip.data[ 3] - clip.data[ 1];
   m_akFrustum[3].y = clip.data[ 7] - clip.data[ 5];
   m_akFrustum[3].z = clip.data[11] - clip.data[ 9];
   m_akFrustum[3].w = clip.data[15] - clip.data[13];
   
   // Extract the FAR plane 
   m_akFrustum[4].x = clip.data[ 3] - clip.data[ 2];
   m_akFrustum[4].y = clip.data[ 7] - clip.data[ 6];
   m_akFrustum[4].z = clip.data[11] - clip.data[10];
   m_akFrustum[4].w = clip.data[15] - clip.data[14];
   
   
   // Extract the NEAR plane 
   m_akFrustum[5].x = clip.data[ 3] + clip.data[ 2];
   m_akFrustum[5].y = clip.data[ 7] + clip.data[ 6];
   m_akFrustum[5].z = clip.data[11] + clip.data[10];
   m_akFrustum[5].w = clip.data[15] + clip.data[14];   

	for(int i=0;i<6;i++){
		if(m_akFrustum[i].PlainLength()>0)
			m_akFrustum[i].PlainNormalize();

		m_akFrustumPlanes[i].Set(m_akFrustum[i].x, m_akFrustum[i].y,m_akFrustum[i].z,
			m_akFrustum[i].w);
	}
}


bool Frustum::PointInFrustum( const Vector3& kPoint) const
{
// 	m_iPointCulls++;

	for(int p = 0; p < 6; p++ ){
		if( m_akFrustum[p].x * kPoint.x + m_akFrustum[p].y * kPoint.y + m_akFrustum[p].z * kPoint.z + m_akFrustum[p].w <= 0 )
			return false;         
	}
	
   return true;
}

 
bool Frustum::SphereInFrustum(const Vector4& kPoint) const
{
// 	m_iSphereCulls++;

	static float d;

	for(int p = 0; p < 6; p++ )
	{
		d = m_akFrustum[p].x * kPoint.x + m_akFrustum[p].y * kPoint.y + m_akFrustum[p].z * kPoint.z + m_akFrustum[p].w;
		if( d <= -kPoint.w )
			return false;
	}

	return true;
}



bool Frustum::SphereInFrustum(const Vector3& kPos,float fRadius) const
{
// 	m_iSphereCulls++;

	static float d;

	for(int p = 0; p < 6; p++ )
	{
		d = m_akFrustum[p].x * kPos.x + m_akFrustum[p].y * kPos.y + m_akFrustum[p].z * kPos.z + m_akFrustum[p].w;
		if( d <= -fRadius )
			return false;
	}

	return true;
}


bool Frustum::CubeInFrustum( float x, float y, float z, float sizex,float sizey,float sizez ) const
{
// 	m_iCubeCulls++;

	static Vector3 kCubeNeg;
	static Vector3 kCubePos;
	
	kCubeNeg.Set(x - sizex, y - sizey, z - sizez); 
	kCubePos.Set(x + sizex, y + sizey, z + sizez); 

	for(int p = 0; p < 6; p++ )
	{
		if( m_akFrustum[p].x * (kCubeNeg.x) + m_akFrustum[p].y * (kCubeNeg.y) + m_akFrustum[p].z * (kCubeNeg.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kCubePos.x) + m_akFrustum[p].y * (kCubeNeg.y) + m_akFrustum[p].z * (kCubeNeg.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kCubeNeg.x) + m_akFrustum[p].y * (kCubePos.y) + m_akFrustum[p].z * (kCubeNeg.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kCubePos.x) + m_akFrustum[p].y * (kCubePos.y) + m_akFrustum[p].z * (kCubeNeg.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kCubeNeg.x) + m_akFrustum[p].y * (kCubeNeg.y) + m_akFrustum[p].z * (kCubePos.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kCubePos.x) + m_akFrustum[p].y * (kCubeNeg.y) + m_akFrustum[p].z * (kCubePos.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kCubeNeg.x) + m_akFrustum[p].y * (kCubePos.y) + m_akFrustum[p].z * (kCubePos.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kCubePos.x) + m_akFrustum[p].y * (kCubePos.y) + m_akFrustum[p].z * (kCubePos.z) + m_akFrustum[p].w > 0 )
			continue;
		return false;
	}
	return true;
}

bool Frustum::CubeInFrustum( const Vector3& kMin,const Vector3& kMax) const
{
// 	m_iCubeCulls++;

	for(int p = 0; p < 6; p++ )
	{
		if( m_akFrustum[p].x * (kMin.x) + m_akFrustum[p].y * (kMin.y) + m_akFrustum[p].z * (kMin.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kMax.x) + m_akFrustum[p].y * (kMin.y) + m_akFrustum[p].z * (kMin.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kMin.x) + m_akFrustum[p].y * (kMax.y) + m_akFrustum[p].z * (kMin.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kMax.x) + m_akFrustum[p].y * (kMax.y) + m_akFrustum[p].z * (kMin.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kMin.x) + m_akFrustum[p].y * (kMin.y) + m_akFrustum[p].z * (kMax.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kMax.x) + m_akFrustum[p].y * (kMin.y) + m_akFrustum[p].z * (kMax.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kMin.x) + m_akFrustum[p].y * (kMax.y) + m_akFrustum[p].z * (kMax.z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kMax.x) + m_akFrustum[p].y * (kMax.y) + m_akFrustum[p].z * (kMax.z) + m_akFrustum[p].w > 0 )
			continue;
		return false;
	}
	return true;
}


bool Frustum::CubeInFrustum ( const Vector3& kPos, const Vector3& kCenter, const Vector3& kSize, Matrix4 kRotation ) const
{
// 	m_iCubeCulls++;

	static int p;
	static Vector3 kEdge[8];


	kEdge[0] = Vector3(  kSize.x, -kSize.y, -kSize.z );
	kEdge[1] = Vector3( -kSize.x, -kSize.y, -kSize.z );
	kEdge[2] = Vector3( -kSize.x, -kSize.y,  kSize.z );
	kEdge[3] = Vector3(  kSize.x, -kSize.y,  kSize.z );
	kEdge[4] = Vector3(  kSize.x,  kSize.y, -kSize.z );
	kEdge[5] = Vector3( -kSize.x,  kSize.y, -kSize.z );
	kEdge[6] = Vector3( -kSize.x,  kSize.y,  kSize.z );
	kEdge[7] = Vector3(  kSize.x,  kSize.y,  kSize.z );

	for ( int i = 0; i < 8; i++ )
	{	
		kEdge[i] = kRotation.VectorRotate ( kEdge[i] + kCenter );
		kEdge[i] += kPos;
	}

	for( p = 0; p < 6; p++ )
	{
		if( m_akFrustum[p].x * (kEdge[0].x) + m_akFrustum[p].y * (kEdge[0].y) + m_akFrustum[p].z * (kEdge[0].z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kEdge[1].x) + m_akFrustum[p].y * (kEdge[1].y) + m_akFrustum[p].z * (kEdge[1].z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kEdge[2].x) + m_akFrustum[p].y * (kEdge[2].y) + m_akFrustum[p].z * (kEdge[2].z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kEdge[3].x) + m_akFrustum[p].y * (kEdge[3].y) + m_akFrustum[p].z * (kEdge[3].z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kEdge[4].x) + m_akFrustum[p].y * (kEdge[4].y) + m_akFrustum[p].z * (kEdge[4].z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kEdge[5].x) + m_akFrustum[p].y * (kEdge[5].y) + m_akFrustum[p].z * (kEdge[5].z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kEdge[6].x) + m_akFrustum[p].y * (kEdge[6].y) + m_akFrustum[p].z * (kEdge[6].z) + m_akFrustum[p].w > 0 )
			continue;
		if( m_akFrustum[p].x * (kEdge[7].x) + m_akFrustum[p].y * (kEdge[7].y) + m_akFrustum[p].z * (kEdge[7].z) + m_akFrustum[p].w > 0 )
			continue;

		return false;
	}
	return true;

}
