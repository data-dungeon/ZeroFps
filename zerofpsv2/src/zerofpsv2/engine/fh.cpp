/*
	Offical FulHack.cpp fil =).
	For Engine.DLL.

	All code in this file have to be inside name tags that state the name of he who dares to write
	code in this file.
*/

/*** Start: Your Name ***/
	// Ugly Code From Hell Here...
/*** End: Your Name ***/

#include "fh.h"
#include "zerofps.h"
#include "../basic/zfobjectmanger.h"

/*** Start: Vim ***/





DebugGraph::DebugGraph()
{
	m_iSize		= 0;
	m_iWidth		= 0;
	m_iHeight	= 0;
	m_iWritePos	= 0;
	m_kBackColor.Set(1,1,1);
}

void DebugGraph::SetSize(int iSize,int iWidth, int iHeight)
{
	m_kValues.resize(iSize);
	m_iSize = iSize;

	for(unsigned int i=0; i<m_kValues.size(); i++) {
		m_kValues[i] = 0;
		}

	m_iWritePos	= 0;
	m_iWidth		= iWidth;
	m_iHeight	= iHeight;
}

void DebugGraph::SetMinMax(float fMin, float fMax)
{
	m_fMin = fMin;
	m_fMax = fMax;
}

void DebugGraph::PushValue(float fValue)
{
	m_kValues[m_iWritePos] = fValue;
	m_iWritePos++;
	if(m_iWritePos >= int(m_kValues.size()))
		m_iWritePos = 0;
}

void DebugGraph::DrawSolidBox(int x, int y, int x2, int y2)
{
	glColor3f(m_kBackColor.x, m_kBackColor.y, m_kBackColor.z);

	// Draw a Solid Color Box.
	glBegin(GL_QUADS);
		glVertex3i(x,y,0);
		glVertex3i(x2,y,0);
		glVertex3i(x2,y2,0);
		glVertex3i(x,y2,0);
	glEnd();
}

float	DebugGraph::GetSampleHeight(float fValue)
{
	if(fValue >= m_fMax)
		m_fMax = fValue;
	if(fValue <= m_fMin)
		m_fMin = fValue;

	float fDiff = m_fMax - m_fMin;
	float fP = (fValue - m_fMin) / fDiff;
	fValue = fP * m_iHeight;
	return fValue;
}

void	DebugGraph::SetSampleColor(float fValue)
{

}

void DebugGraph::DrawGraph(int x, int y)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 1024, 0, 768);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTranslated(x,y,0);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glDisable(	GL_FOG);

	int iValueIndex = m_iWritePos;
	int iOffset = 0;
	float fSize;
	glColor3f(1, 0, 0);

	glBegin(GL_LINES);
	for(;;) {
		fSize = GetSampleHeight(m_kValues[iValueIndex]);
		glVertex3i(m_iWidth - iOffset,0,0);
		glVertex3i(m_iWidth - iOffset,(int)fSize,0);
		iOffset++;
		if(iOffset >= m_iWidth)
			break;
		iValueIndex--;
		if(iValueIndex < 0)
			iValueIndex += m_kValues.size();
		if(iValueIndex == m_iWritePos)
			break;
		}
	glEnd();

	DrawSolidBox(0,0, m_iWidth, m_iHeight);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();

}

/*** End: Vim ***/


/*** Start: Vim ***/
class AxisAlignedBBox
{
public:
	Vector3 m_kMin;
	Vector3 m_kMax;

	void Set(Vector3 kMin, Vector3 kMax);
	bool InContact(Vector3 kCenter, float fRadius);
};

void AxisAlignedBBox::Set(Vector3 kMin, Vector3 kMax)
{
	m_kMin = kMin;
	m_kMax = kMax;
}

/* From Gamasutra: Simple Intersection Tests For Games */
bool AxisAlignedBBox::InContact(Vector3 kCenter, float fRadius)
{
	float s, d = 0; 

	//find the square of the distance
	//from the sphere to the box
	for( long i=0 ; i<3 ; i++ ) 
	{ 
		if( kCenter[i] < m_kMin[i] )
		{

			s = kCenter[i] - m_kMin[i];
			d += s*s; 
		}

		else if( kCenter[i] > m_kMax[i] )
		{ 
		s = kCenter[i] - m_kMax[i];	
		d += s*s; 
		}
	}

	return d <= fRadius * fRadius;
}

/*** End: Vim ***/

