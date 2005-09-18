#include "pointtext.h"


PointText::PointText()
{
	m_pkZeroFps= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkRender=	 static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));			


	//setup material
	m_pkRedMaterial = new ZMaterial;
	m_pkRedMaterial->GetPass(0)->m_kTUs[0]->SetRes("text/fetfont.tga");
	m_pkRedMaterial->GetPass(0)->m_bLighting = 				false;		
	m_pkRedMaterial->GetPass(0)->m_bColorMaterial = 		true;
	m_pkRedMaterial->GetPass(0)->m_kVertexColor =			Vector3(1,0,0);
	m_pkRedMaterial->GetPass(0)->m_bFog = 					false;		
	m_pkRedMaterial->GetPass(0)->m_bAlphaTest =				true;		
	m_pkRedMaterial->GetPass(0)->m_bDepthTest = 			false;

	m_pkGreenMaterial = new ZMaterial;
	m_pkGreenMaterial->GetPass(0)->m_kTUs[0]->SetRes("text/fetfont.tga");
	m_pkGreenMaterial->GetPass(0)->m_bLighting = 				false;		
	m_pkGreenMaterial->GetPass(0)->m_bColorMaterial = 		true;
	m_pkGreenMaterial->GetPass(0)->m_kVertexColor =			Vector3(0,1,0);
	m_pkGreenMaterial->GetPass(0)->m_bFog = 					false;		
	m_pkGreenMaterial->GetPass(0)->m_bAlphaTest =				true;		
	m_pkGreenMaterial->GetPass(0)->m_bDepthTest = 			false;
	
	m_pkBlueMaterial = new ZMaterial;
	m_pkBlueMaterial->GetPass(0)->m_kTUs[0]->SetRes("text/fetfont.tga");
	m_pkBlueMaterial->GetPass(0)->m_bLighting = 				false;		
	m_pkBlueMaterial->GetPass(0)->m_bColorMaterial = 		true;
	m_pkBlueMaterial->GetPass(0)->m_kVertexColor =			Vector3(0,0,1);
	m_pkBlueMaterial->GetPass(0)->m_bFog = 					false;		
	m_pkBlueMaterial->GetPass(0)->m_bAlphaTest =				true;		
	m_pkBlueMaterial->GetPass(0)->m_bDepthTest = 			false;
	
	
	m_pkYellowMaterial = new ZMaterial;
	m_pkYellowMaterial->GetPass(0)->m_kTUs[0]->SetRes("text/fetfont.tga");
	m_pkYellowMaterial->GetPass(0)->m_bLighting = 			false;		
	m_pkYellowMaterial->GetPass(0)->m_bColorMaterial = 	true;
	m_pkYellowMaterial->GetPass(0)->m_kVertexColor =		Vector3(1,1,0);
	m_pkYellowMaterial->GetPass(0)->m_bFog = 					false;		
	m_pkYellowMaterial->GetPass(0)->m_bAlphaTest =			true;		
	m_pkYellowMaterial->GetPass(0)->m_bDepthTest = 			false;
			
	//setup font
	m_pkFont = new ZGuiFont("CharacterFont");
	m_pkFont->Create("/data/textures/text/fetfont.fnt",-1);	
}


void PointText::AddText(const string& strText,const Vector3& kPos,const Vector3& kVel,float fTTL,int iType)
{
// 	cout<<"adding new text "<<strText<<endl;

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

			switch(m_kText[i].m_iType)
			{
				case 0:			
					m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),m_kText[i].m_kPos,
														fScale,m_kText[i].m_strText,m_pkRedMaterial,m_pkFont,true);
					break;
				
				case 1:			
					m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),m_kText[i].m_kPos,
														fScale,m_kText[i].m_strText,m_pkGreenMaterial,m_pkFont,true);
					break;
				
				case 2:			
					m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),m_kText[i].m_kPos,
														fScale,m_kText[i].m_strText,m_pkBlueMaterial,m_pkFont,true);
					break;
					
				case 3:			
					m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),m_kText[i].m_kPos,
														fScale,m_kText[i].m_strText,m_pkYellowMaterial,m_pkFont,true);
					break;
					
			}
		}	
	}
}
