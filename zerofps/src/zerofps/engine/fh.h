/*
	Offical FulHack.cpp fil =).
	For Engine.h.

	All code in this file have to be inside name tags that state the name of he who dares to write
	code in this file.
*/

/*** Start: Your Name ***/
	// Ugly Code From Hell Here...
/*** End: Your Name ***/

#ifndef _ENGINE_FH_H_
#define _ENGINE_FH_H_
#include "zfresource.h"
#include "../basic/basic.pkg"

#include <vector>


using namespace std;

/*** Start: Vim ***/
void ENGINE_API DrawTest(float x, float y, float z);

class ResTexture : public ZFResource
{
private:

public:
	int		m_iTextureID;
	string	strTextureName;

	ResTexture();
	~ResTexture();

	bool Create(string strName);
};

class DebugGraph
{
public:
	DebugGraph();

	Vector3			m_kBackColor;

	vector<float>	m_kValues;
	int				m_iSize;

	float				m_fMin;
	float				m_fMax;

	int				m_iWritePos;

	int				m_iWidth;
	int				m_iHeight;

	void SetSize(int iSize, int iWidth, int iHeight);
	void SetMinMax(float fMin, float fMax);

	void PushValue(float fValue);

	void DrawGraph(int x, int y);
	void DrawSolidBox(int x, int y, int x2, int y2);
	
	void SetBackColor(float r, float g, float b)	{ m_kBackColor.Set(r,g,b); }

	float	GetSampleHeight(float fValue);
	void	SetSampleColor(float fValue);
};

/*** End: Vim ***/


#endif



