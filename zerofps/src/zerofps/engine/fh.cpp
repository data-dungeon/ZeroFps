#include "fh.h"

class BoneAnim
{
public:
	Vector3 m_kPos;
	Vector3 m_kRot;
	
	void Set(float x, float y, float z, float rx, float ry, float rz) {
		m_kPos.Set(x,y,z);
		m_kRot.Set(rx,ry,rz);
		}		
};

class BoneAnimFrame
{
public:
	vector<BoneAnim>	BonePositions;
};

void CreateAnim()
{
	BoneAnim	NewBone;

	BoneAnimFrame	BoneFrames;

	BoneFrames.BonePositions.push_back(NewBone);
}

void DrawTest(float x, float y, float z)
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	
	glPushMatrix();
	glColor3f(1,1,1);
	glTranslatef(x,y,z);

	glTranslatef(-0.023717, 0.008030, 0.000000);
	glRotatef(1,0,0, 3.141593 );
	glRotatef(0,1,0, 0.000000 );
	glRotatef(0,0,1, 1.541102 );


 
/* 0 joint1 -1  -0.023717 0.008030 0.000000  3.141593 0.000000 1.541102 
 1 joint2 0  1.303530 0.000000 0.000000  -3.141593 -0.000000 0.672710 
 2 joint3 1  1.535219 -0.000000 -0.000000  0.000000 0.000000 -2.197444 
*/
	glutSolidSphere(0.1, 8, 8);
	glPopMatrix();
	glPopAttrib();
}

