#ifndef _POINTTEXT_H_
#define _POINTTEXT_H_

#include <iostream>
#include <string>
#include "../zerofpsv2/engine/zerofps.h"

using namespace std;

class PointText
{
	private:
		ZeroFps*	m_pkZeroFps;
		Render*	m_pkRender;
	
		class PText
		{
			public:
				string	m_strText;
				Vector3	m_kPos;
				Vector3	m_kVel;
				float		m_fTTL;
				float		m_fCreated;
				int		m_iType;
		};	
	
		vector<PText>	m_kText;
		
		//font
		ZMaterial*	m_pkRedMaterial;
		ZMaterial*	m_pkGreenMaterial;
		ZMaterial*	m_pkBlueMaterial;
		ZMaterial*	m_pkYellowMaterial;
		ZGuiFont*	m_pkFont;		
		
	
	public:	
		PointText();
	
		void AddText(const string& strText,const Vector3& kPos,const Vector3& kVel,float fTTL,int iType);
		void Draw();
};


#endif

