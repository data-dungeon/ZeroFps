#include "pointtext.h"


PointText::PointText()
{
	m_pkZeroFps= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkRender=	 static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			


	//setup material
	m_pkTextMaterial = new ZMaterial;
	m_pkTextMaterial->GetPass(0)->m_kTUs[0]->SetRes("data/textures/text/fetfont.tga");
	m_pkTextMaterial->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
	m_pkTextMaterial->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
	m_pkTextMaterial->GetPass(0)->m_bLighting = 				false;		
	m_pkTextMaterial->GetPass(0)->m_bColorMaterial = 		true;
	m_pkTextMaterial->GetPass(0)->m_kVertexColor =			Vector3(1,0,0);
	m_pkTextMaterial->GetPass(0)->m_bFog = 					false;		
	m_pkTextMaterial->GetPass(0)->m_bAlphaTest =				true;		
	m_pkTextMaterial->GetPass(0)->m_bDepthTest = 			false;

	//setup font
	m_pkFont = new ZGuiFont("CharacterFont");
	m_pkFont->Create("/data/textures/text/fetfont.fnt",-1);	
}


void PointText::AddText(const string& strText,const Vector3& kPos,const Vector3& kVel,float fTTL,int iType)
{
	cout<<"adding new text "<<strText<<endl;

	PText kNew;
	
	kNew.m_strText = 	strText;
	kNew.m_fTTL = 		fTTL;
	kNew.m_kPos =		kPos;
	kNew.m_kVel = 		kVel;
	kNew.m_fCreated = m_pkZeroFps->GetEngineTime();
	kNew.m_iType = 	iType;
	
	m_kText.push_back(kNew);
}

void PointText::Draw()
{
	float fTime = m_pkZeroFps->GetEngineTime();
	float fAlpha = m_pkZeroFps->GetFrameTime();
	
	//remove old text
	for(vector<PText>::iterator it = m_kText.begin();it<m_kText.end();it++)
	{		
		if(fTime - (*it).m_fCreated > (*it).m_fTTL)
		{
			m_kText.erase(it);
			it = m_kText.begin();
		}	
	}

	//draw text
 	for(int i = 0;i<m_kText.size();i++)
	{
		m_kText[i].m_kPos += m_kText[i].m_kVel * fAlpha;
	
	
		float fDistance = m_pkZeroFps->GetCam()->GetRenderPos().DistanceTo(m_kText[i].m_kPos);
		if(fDistance < 20)
		{
  			float fScale = 0.05 * fDistance;
// 			float fScale = 0.2;			

			m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),m_kText[i].m_kPos,
												fScale,m_kText[i].m_strText,m_pkTextMaterial,m_pkFont,true);													
		}	
	}
}
