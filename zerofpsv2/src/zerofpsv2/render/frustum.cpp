#include "frustum.h"
 
Frustum::Frustum()// : ZFSubSystem("Frustum") 
{
	
  

}

void Frustum::GetFrustum() {
   Matrix4   proj;
   Matrix4   modl;
   Matrix4   clip;
   
	// Get the current PROJECTION matrix from OpenGL 
   glGetFloatv( GL_PROJECTION_MATRIX, (float*) &proj.data);

   // Get the current MODELVIEW matrix from OpenGL 
   glGetFloatv( GL_MODELVIEW_MATRIX, (float*)&modl.data );

	 clip=modl*proj;
	  
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


bool Frustum::PointInFrustum( Vector3 kPoint)
{
	for(int p = 0; p < 6; p++ ){
		if( m_akFrustum[p].x * kPoint.x + m_akFrustum[p].y * kPoint.y + m_akFrustum[p].z * kPoint.z + m_akFrustum[p].w <= 0 )
			return false;         
	}
	
   return true;
}


bool Frustum::SphereInFrustum(Vector4 kPoint)
{
	float d;

	for(int p = 0; p < 6; p++ )
	{
		d = m_akFrustum[p].x * kPoint.x + m_akFrustum[p].y * kPoint.y + m_akFrustum[p].z * kPoint.z + m_akFrustum[p].w;
		if( d <= -kPoint.w )
			return false;
	}

	return true;
}


bool Frustum::SphereInFrustum(Vector4& kPoint)
{
	float d;

	for(int p = 0; p < 6; p++ )
	{
		d = m_akFrustum[p].x * kPoint.x + m_akFrustum[p].y * kPoint.y + m_akFrustum[p].z * kPoint.z + m_akFrustum[p].w;
		if( d <= -kPoint.w )
			return false;
	}

	return true;
}

bool Frustum::SphereInFrustum(Vector3 kPos,float fRadius)
{
	float d;

	for(int p = 0; p < 6; p++ )
	{
		d = m_akFrustum[p].x * kPos.x + m_akFrustum[p].y * kPos.y + m_akFrustum[p].z * kPos.z + m_akFrustum[p].w;
		if( d <= -fRadius )
			return false;
	}

	return true;
}


bool Frustum::CubeInFrustum( float x, float y, float z, float sizex,float sizey,float sizez )
{
	int p;

	Vector3 kCubeNeg(x - sizex, y - sizey, z - sizez); 
	Vector3 kCubePos(x + sizex, y + sizey, z + sizez); 

	for( p = 0; p < 6; p++ )
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


