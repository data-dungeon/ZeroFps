#include "effectsystem.h"


EffectSystem* EffectSystem::m_pkInstance = new EffectSystem;		 



EffectSystem::EffectSystem()
{
	cout<<"effect system created"<<endl;

}

void EffectSystem::Setup(int iMaxParts)
{
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));			
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));				
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkLevelMan=static_cast<LevelManager*>(g_ZFObjSys.GetObjectPtr("LevelManager"));
	m_pkMap=m_pkLevelMan->GetHeightMap();

	
	m_iMaxParts = iMaxParts;
	
	particle temp;
	temp.iType = -1;
	temp.kPos.Set(0,0,0);
	
	for(int i=0;i<m_iMaxParts;i++)
		m_kParts.push_back(temp);
}

void EffectSystem::AddFX(int iType,Vector3 kPos)
{
	switch(iType)
	{
		case FX_88MMHIT:
		{
			for(int i= 0;i< 6;i++)
			{
				AddPart(PART_DEBRI,kPos);			
			}
			
			for(int j= 0;j< 2;j++)
			{
				AddPart(PART_SMOKE,kPos);			
			}
	
			break;
		}
		
		case FX_HEAVYMGHIT:
		{
			for(int i= 0;i< 2;i++)
			{
				AddPart(PART_SMALLDEBRI,kPos);			
			}
			
			break;
		}
		
		case FX_88MMFIRE:
		{
			for(int i= 0;i< 4;i++)
			{
				AddPart(PART_SMOKE,kPos);			
			}
	
			break;
		}
		
		case FX_EXPLOSION1:
		{
			for(int i= 0;i< 20;i++)
			{
				AddPart(PART_SMOKE,kPos);			
			}
			
			for(int i= 0;i< 40;i++)
			{
				AddPart(PART_DEBRI,kPos);			
			}
			
			for(int i= 0;i< 20;i++)
			{
				AddPart(PART_SMALLDEBRI,kPos);			
			}
	
			break;
		}
		
	}
}

void EffectSystem::Draw()
{
	glPushAttrib(GL_COLOR_BUFFER_BIT|GL_LIGHTING_BIT|GL_ENABLE_BIT);
	//glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	for(int i=0;i<m_iMaxParts;i++)
	{
		if(m_kParts[i].iType == -1)
			continue;

		switch(m_kParts[i].iType)
		{			
			case PART_DEBRI:
				m_pkRender->DrawBillboard(m_pkFps->GetCam()->GetModelViewMatrix(),
													m_kParts[i].kPos,
													1,
													m_pkTexMan->Load("../data/textures/fire.tga",0));
				break;
			
			case PART_SMALLDEBRI:
				m_pkRender->DrawBillboard(m_pkFps->GetCam()->GetModelViewMatrix(),
													m_kParts[i].kPos,
													0.4,
													m_pkTexMan->Load("../data/textures/fire.tga",0));
				break;
			
			case PART_SMOKE:
				m_pkRender->DrawBillboard(m_pkFps->GetCam()->GetModelViewMatrix(),
													m_kParts[i].kPos,
													0.5,
													m_pkTexMan->Load("../data/textures/smoke.tga",0));
				break;

		}
	}
	
	glPopAttrib();
}

void EffectSystem::Update()
{
	for(int i=0;i<m_iMaxParts;i++)
	{
		if(m_kParts[i].iType == -1)
			continue;
			
		if(m_pkFps->GetGameTime() - m_kParts[i].fCTime >m_kParts[i].fTTL)
		{	
			m_kParts[i].iType = -1;
			continue;
		}
			
			
		switch(m_kParts[i].iType)
		{			
		
			case PART_SMALLDEBRI:
			case PART_DEBRI:
				m_kParts[i].kVel += Vector3(0,-0.3,0) * m_pkFps->GetGameFrameTime();
				m_kParts[i].kPos += m_kParts[i].kVel * m_pkFps->GetGameFrameTime(); 			
				break;
					
			case PART_SMOKE:
				m_kParts[i].kVel += Vector3(0,0.5,0) * m_pkFps->GetGameFrameTime();
				m_kParts[i].kPos += m_kParts[i].kVel * m_pkFps->GetGameFrameTime(); 			
				break;
		
		}
	}
}

void EffectSystem::AddPart(int iType,Vector3 kPos)
{
	for(int i=0;i<m_kParts.size();i++)
	{
		if(m_kParts[i].iType == -1)
		{
			m_kParts[i].iType = iType;
			m_kParts[i].kPos = kPos;
			m_kParts[i].fCTime = m_pkFps->GetGameTime();
			m_kParts[i].fTTL = 1;
			
			switch(iType)			
			{
				case PART_DEBRI:
					m_kParts[i].kVel = Vector3((rand()% 100)-50,rand()% 100+1,(rand()% 100-50)).Unit();
					m_kParts[i].fTTL = 2;
					break;		
				
				case PART_SMALLDEBRI:
					m_kParts[i].kVel = Vector3((rand()% 100)-50,rand()% 100+1,(rand()% 100-50)).Unit();
					m_kParts[i].fTTL = 1;
					break;		

				case PART_SMOKE:
					m_kParts[i].kVel = Vector3((rand()% 3)-1.5,rand()% 2+1,(rand()% 3-1.5)).Unit();
					m_kParts[i].fTTL = 0.5;
					break;					
			}
			
			return;
		}
	}
}

void EffectSystem::StopPart(int iID)
{
	m_kParts[iID].iType = -1;
}









