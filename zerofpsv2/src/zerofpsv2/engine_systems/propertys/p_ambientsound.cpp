// p_ambientsound.h

#include "p_ambientsound.h"
#include "../../engine/entity.h"

P_AmbientSound::P_AmbientSound()
{
	strcpy(m_acName,"P_AmbientSound");

	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pEntityMan = static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));

	bNetwork = true;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;

	m_strSound = "";
	m_bSoundStarted = false;

	m_iAmbientAreaID = -1;

	m_kPrevPos = Vector3(0,0,0);
}

P_AmbientSound::~P_AmbientSound()
{
	if(m_iAmbientAreaID != -1)
		m_pkAudioSystem->RemoveAmbientArea(m_iAmbientAreaID);	
}

void P_AmbientSound::SetArea(vector<Vector2>& kPolygon) // OBS! Det är lokala coordinater som skall skickas in.
{
	m_kPolygon = kPolygon;

	vector<Vector2> kRealArea(m_kPolygon);

	Vector2 objpos(GetEntity()->GetWorldPosV().x, GetEntity()->GetWorldPosV().z);
	for(int i=0; i<kRealArea.size(); i++)
		kRealArea[i] += objpos;

	if(m_iAmbientAreaID != -1)
		m_pkAudioSystem->ChangePntsInAmbientArea(m_iAmbientAreaID, kRealArea);
}

void P_AmbientSound::SetSound(string strSound)
{
	m_strSound = strSound;

	if(m_iAmbientAreaID != -1)
		m_pkAudioSystem->ChangeAmbientAreaSound(m_iAmbientAreaID, m_strSound);

	printf("P_AmbientSound::SetSound %s\n", strSound.c_str());
}

void P_AmbientSound::GetArea(vector<Vector2>& kPolygon)
{
	kPolygon = m_kPolygon;
}

string P_AmbientSound::GetSound()
{
	return m_strSound;
}

void P_AmbientSound::Update()
{
	if(m_pEntityMan->IsUpdate(PROPERTY_SIDE_CLIENT))
	{
		if(m_bSoundStarted == false)
		{
			vector<Vector2> kRealArea(m_kPolygon);

			Vector2 objpos(GetEntity()->GetWorldPosV().x, GetEntity()->GetWorldPosV().z);
			for(int i=0; i<kRealArea.size(); i++)
			{
				kRealArea[i] += objpos;
			}

			m_iAmbientAreaID = m_pkAudioSystem->AddAmbientArea(m_strSound, kRealArea);
			m_bSoundStarted = true;
		}

		if(m_iAmbientAreaID != -1)
		{
			if(m_kPrevPos != GetEntity()->GetWorldPosV())
				SetArea(m_kPolygon);
		}
	}
}

void P_AmbientSound::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write_Str(m_strSound.c_str());

	int iNumPoints = m_kPolygon.size();
	pkNetPacket->Write( iNumPoints );

	for(int i=0; i<iNumPoints; i++)
	{
		pkNetPacket->Write(m_kPolygon[i].x);
		pkNetPacket->Write(m_kPolygon[i].y);
	}

	SetNetUpdateFlag(iConnectionID,false);
}

void P_AmbientSound::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	char temp[128];
	pkNetPacket->Read_Str(temp);
	m_strSound = temp;

	int iNumPoints;
	pkNetPacket->Read( iNumPoints );

	m_kPolygon.clear(); 

	for(int i=0; i<iNumPoints; i++)
	{
		float x, y;
		pkNetPacket->Read(x);
		pkNetPacket->Read(y);
		m_kPolygon.push_back(Vector2(x,y)); 
	}

	m_bSoundStarted = false;
}

vector<PropertyValues> P_AmbientSound::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	kReturn[0].kValueName = "filename";
	kReturn[0].iValueType = VALUETYPE_STRING; 
	kReturn[0].pkValue    = (void*)&m_strSound;

	SetNetUpdateFlag(true);

	SetSound(m_strSound);

	printf("P_AmbientSound::GetPropertyValues\n");
	
	return kReturn;
}

void P_AmbientSound::Save(ZFIoInterface* pkFile)
{
	char temp[128];
	strcpy(temp,m_strSound.c_str());

	pkFile->Write((void*)temp,128,1);

	int iNumPoints = m_kPolygon.size();
	pkFile->Write(&iNumPoints, sizeof(int), 1);

	for(int i=0; i<iNumPoints; i++)
	{
		pkFile->Write(&m_kPolygon[i].x, sizeof(float), 1);
		pkFile->Write(&m_kPolygon[i].y, sizeof(float), 1);
	}
}

void P_AmbientSound::Load(ZFIoInterface* pkFile,int iVersion)
{
	char temp[128];
	pkFile->Read((void*)temp,128,1);	
	m_strSound = temp;

	int iNumPoints;
	pkFile->Read(&iNumPoints, sizeof(int), 1);

	for(int i=0; i<iNumPoints; i++)
	{ 
		float x, y;
		pkFile->Read(&x, sizeof(float), 1);
		pkFile->Read(&y, sizeof(float), 1);
		m_kPolygon.push_back(Vector2(x,y));
	}

	m_bSoundStarted = false;

	SetNetUpdateFlag(true);

	m_kPrevPos = GetEntity()->GetWorldPosV();
}

Property* Create_AmbientSound()
{
	return new P_AmbientSound();
}