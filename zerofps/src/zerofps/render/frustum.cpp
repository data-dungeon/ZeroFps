#include "frustum.h"

Frustum::Frustum() : ZFObject("Frustum") 
{
	


}

void Frustum::GetFrustum() {
   Matrix4   proj;
   Matrix4   modl;
   Matrix4   clip;
   
	// Get the current PROJECTION matrix from OpenGL 
   glGetFloatv( GL_PROJECTION_MATRIX, (float*)&proj[0] );

   // Get the current MODELVIEW matrix from OpenGL 
   glGetFloatv( GL_MODELVIEW_MATRIX, (float*)&modl[0] );

//		proj.Print();
//		exit(0);
	
	 clip=modl*proj;
//	clip=proj*modl;
	
/*	
        // Concatenate (Multiply) The Two Matricies
        clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
        clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
        clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
        clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

        clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
        clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
        clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
        clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

        clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
        clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
        clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
        clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

        clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
        clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
        clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
        clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

*/
	

	
	// Extract the numbers for the RIGHT plane 
   m_akFrustum[0][0] = clip[ 3] - clip[ 0];
   m_akFrustum[0][1] = clip[ 7] - clip[ 4];
   m_akFrustum[0][2] = clip[11] - clip[ 8];
   m_akFrustum[0][3] = clip[15] - clip[12];

   // Extract the numbers for the LEFT plane 
   m_akFrustum[1][0] = clip[ 3] + clip[ 0];
   m_akFrustum[1][1] = clip[ 7] + clip[ 4];
   m_akFrustum[1][2] = clip[11] + clip[ 8];
   m_akFrustum[1][3] = clip[15] + clip[12];  
  
   // Extract the BOTTOM plane 
   m_akFrustum[2][0] = clip[ 3] + clip[ 1];
   m_akFrustum[2][1] = clip[ 7] + clip[ 5];
   m_akFrustum[2][2] = clip[11] + clip[ 9];
   m_akFrustum[2][3] = clip[15] + clip[13];
   
   
   // Extract the TOP plane 
   m_akFrustum[3][0] = clip[ 3] - clip[ 1];
   m_akFrustum[3][1] = clip[ 7] - clip[ 5];
   m_akFrustum[3][2] = clip[11] - clip[ 9];
   m_akFrustum[3][3] = clip[15] - clip[13];
   
   // Extract the FAR plane 
   m_akFrustum[4][0] = clip[ 3] - clip[ 2];
   m_akFrustum[4][1] = clip[ 7] - clip[ 6];
   m_akFrustum[4][2] = clip[11] - clip[10];
   m_akFrustum[4][3] = clip[15] - clip[14];
   
   
   // Extract the NEAR plane 
   m_akFrustum[5][0] = clip[ 3] + clip[ 2];
   m_akFrustum[5][1] = clip[ 7] + clip[ 6];
   m_akFrustum[5][2] = clip[11] + clip[10];
   m_akFrustum[5][3] = clip[15] + clip[14];   
   
	for(int i=0;i<6;i++){
		if(m_akFrustum[i].PlainLength()>0)
			m_akFrustum[i].PlainNormalize();

		m_akFrustumPlanes[i].Set(m_akFrustum[i][0], m_akFrustum[i][1],m_akFrustum[i][2],
			m_akFrustum[i][3]);
	}
}


bool Frustum::PointInFrustum( Vector3 kPoint)
{
	for(int p = 0; p < 6; p++ ){
		if( m_akFrustum[p][0] * kPoint.x + m_akFrustum[p][1] * kPoint.y + m_akFrustum[p][2] * kPoint.z + m_akFrustum[p][3] <= 0 )
			return false;         
	}
	
   return true;
}

bool Frustum::PointInFrustum( Vector3& kPoint)
{
	for(int p = 0; p < 6; p++ ){
		if( m_akFrustum[p][0] * kPoint.x + m_akFrustum[p][1] * kPoint.y + m_akFrustum[p][2] * kPoint.z + m_akFrustum[p][3] <= 0 )
			return false;         
	}
	
   return true;
}


bool Frustum::SphereInFrustum(Vector4 kPoint)
{
	float d;

	for(int p = 0; p < 6; p++ )
	{
		d = m_akFrustum[p][0] * kPoint.x + m_akFrustum[p][1] * kPoint.y + m_akFrustum[p][2] * kPoint.z + m_akFrustum[p][3];
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
		d = m_akFrustum[p][0] * kPoint.x + m_akFrustum[p][1] * kPoint.y + m_akFrustum[p][2] * kPoint.z + m_akFrustum[p][3];
		if( d <= -kPoint.w )
			return false;
	}

	return true;
}

/*
bool Frustum::SphereInFrustum(Vector3& kPos,float& fRadius)
{
	float d;

	for(int p = 0; p < 6; p++ )
	{
		d = m_akFrustum[p][0] * kPos.x + m_akFrustum[p][1] * kPos.y + m_akFrustum[p][2] * kPos.z + m_akFrustum[p][3];
		if( d <= -fRadius )
			return false;
	}

	return true;
}*/


bool Frustum::SphereInFrustum(Vector3 kPos,float fRadius)
{
	float d;

	for(int p = 0; p < 6; p++ )
	{
		d = m_akFrustum[p][0] * kPos.x + m_akFrustum[p][1] * kPos.y + m_akFrustum[p][2] * kPos.z + m_akFrustum[p][3];
		if( d <= -fRadius )
			return false;
	}

	return true;
}


bool Frustum::CubeInFrustum( float x, float y, float z, float sizex,float sizey,float sizez )
{
	int p;

	for( p = 0; p < 6; p++ )
	{
		if( m_akFrustum[p][0] * (x - sizex) + m_akFrustum[p][1] * (y - sizey) + m_akFrustum[p][2] * (z - sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x + sizex) + m_akFrustum[p][1] * (y - sizey) + m_akFrustum[p][2] * (z - sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x - sizex) + m_akFrustum[p][1] * (y + sizey) + m_akFrustum[p][2] * (z - sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x + sizex) + m_akFrustum[p][1] * (y + sizey) + m_akFrustum[p][2] * (z - sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x - sizex) + m_akFrustum[p][1] * (y - sizey) + m_akFrustum[p][2] * (z + sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x + sizex) + m_akFrustum[p][1] * (y - sizey) + m_akFrustum[p][2] * (z + sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x - sizex) + m_akFrustum[p][1] * (y + sizey) + m_akFrustum[p][2] * (z + sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x + sizex) + m_akFrustum[p][1] * (y + sizey) + m_akFrustum[p][2] * (z + sizez) + m_akFrustum[p][3] > 0 )
			continue;
		return false;
	}
	return true;
}
