// p_ambientsound.h

#include "p_ambientsound.h"
#include "../../engine/entity.h"

const float FADE_TIME_AMBIENT_AREA = 2.5f; // sekunder.

P_AmbientSound::P_AmbientSound()
{
	strcpy(m_acName,"P_AmbientSound");

	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pEntityMan = static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));

	bNetwork = true;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;

	m_bDotFileLoaded = false;
	m_iCurrentAmbientArea = -1;

	m_bChangeSound = false;
	m_strSound = "";
	m_iSoundID = -1;
	m_fGain = 0.0f;

	m_fFadeTimer = -1;

	m_bDotFileDontExist = false;
}

P_AmbientSound::~P_AmbientSound()
{

}

void P_AmbientSound::Update()
{
	if(m_pEntityMan->IsUpdate(PROPERTY_SIDE_SERVER))
	{
		if(m_bDotFileLoaded == false && m_bDotFileDontExist == false)
		{			
			if(LoadDotFile(m_pEntityMan->GetLoadDir() + string("/asa.dot")))
			{
				m_bDotFileLoaded = true;
			}
		}
		else
		{
			Vector2 pos(GetEntity()->GetIWorldPosV().x, GetEntity()->GetIWorldPosV().z);

			int area = -999;

			for(int i=0; i<m_kDotArray.size(); i++)
				if(PntInPolygon(&pos, m_kDotArray[i].m_kPolygon))
				{
					area = i;
					break;
				}

			if(m_iCurrentAmbientArea != area)
			{
				m_iCurrentAmbientArea = area;
				SetNetUpdateFlag(true);
			}
		}
	}
	else
	{
		if(m_bChangeSound == false && m_iSoundID != -1) // kvar i samma ambientarea, flytta ljudet
		{																// och ändra velocity.
			m_pkAudioSystem->MoveSound(m_iSoundID, GetEntity()->GetIWorldPosV(), Vector3(0,0,0), m_fGain);

			if(m_fGain < 1.0f)
			{
				FadeGain(false);
			}
			else
			{
				m_fGain = 1.0f;
				m_fFadeTimer = -1;
			}
		}
		else
		{
			if(m_iSoundID != -1)
			{
				if(m_fGain > 0)
				{
					FadeGain(true);
					m_pkAudioSystem->MoveSound(m_iSoundID, GetEntity()->GetIWorldPosV(), Vector3(0,0,0), m_fGain);
					return; // Starta inte det nya ljudet föränn det gamla har fadat ut..
				}
				else
				{
					m_pkAudioSystem->StopSound(m_iSoundID);
					m_fGain = 0.0f;
					m_fFadeTimer = -1;
				}
			}
			
			m_iSoundID = m_pkAudioSystem->StartSound(m_strSound, 
				GetEntity()->GetIWorldPosV(), Vector3(0,0,0), true, m_fGain); // och starta ett nytt.

			m_bChangeSound = false;
		}	
	}
}

void P_AmbientSound::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	if(m_iCurrentAmbientArea >= 0)
		pkNetPacket->Write_Str(  m_kDotArray[m_iCurrentAmbientArea].m_strFileName );
	else
	{
		string strNone = "";
		pkNetPacket->Write_Str(  strNone );
	}

	SetNetUpdateFlag(iConnectionID,false);
}

void P_AmbientSound::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	string strSound;
	pkNetPacket->Read_Str( strSound );
	m_strSound = strSound;
	m_bChangeSound = true;
}

bool P_AmbientSound::LoadDotFile(string strFileName)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string(strFileName),0,false) ) {
		cout << "Failed to load dot file: " << strFileName.c_str() << endl;
		m_bDotFileDontExist = true;
		return false;
		}
	
	int iNumAreas;
	kZFile.Read(&iNumAreas, sizeof(int), 1);

	for(int i=0; i<iNumAreas; i++)
	{
		DOTFILE_INFO di;
		kZFile.Read_Str(di.m_strAreaName);
		kZFile.Read_Str(di.m_strFileName);

		int iNumPoints;
		kZFile.Read(&iNumPoints, sizeof(int), 1);

		for(int i=0; i<iNumPoints; i++)
		{
			float x,y;
			kZFile.Read(&x, sizeof(float), 1);
			kZFile.Read(&y, sizeof(float), 1);
			di.m_kPolygon.push_back(new Vector2(x,y)); 
		}

		m_kDotArray.push_back(di); 
	}

	return true;
}

bool P_AmbientSound::PntInPolygon(Vector2 *pt, vector<Vector2*>& kPolygon)
{
	int wn = 0;

	vector<Vector2 *>::iterator it;

	// loop through all edges of the polygon
	for (it=kPolygon.begin(); it<kPolygon.end()-1; it++)
	{
		if ((*(it))->y <= pt->y)
		{         
			if ((*(it+1))->y > pt->y) 
				if (IsLeft( *it, *(it+1), pt) > 0)
					++wn;
		}
		else
		{                       
			if ((*(it+1))->y <= pt->y)
				if (IsLeft( *it, *(it+1), pt) < 0)
					--wn;
		}
	}
	if (wn==0)
		return false;

	return true;
}

void P_AmbientSound::FadeGain(bool bOut)
{
	float fTime = m_pEntityMan->GetSimTime();

	if(m_fFadeTimer == -1)
		m_fFadeTimer = fTime;

	float fTimeSinceLastFrame = fTime - m_fFadeTimer;
	float dif = fTimeSinceLastFrame / FADE_TIME_AMBIENT_AREA;

	if(bOut)
		m_fGain -= dif;
	else
		m_fGain += dif;

	m_fFadeTimer = fTime;
}

vector<PropertyValues> P_AmbientSound::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
	return kReturn;
}

void P_AmbientSound::Save(ZFIoInterface* pkFile)
{
}

void P_AmbientSound::Load(ZFIoInterface* pkFile,int iVersion)
{
}

Property* Create_AmbientSound()
{
	return new P_AmbientSound();
}


