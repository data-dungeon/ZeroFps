#include "ambientsoundareas.h"

AmbientSoundAreas::AmbientSoundAreas()
{
	m_bAddPointsToSoundArea = false;
	m_iPlayingSoundID = -1;
}

AmbientSoundAreas::~AmbientSoundAreas()
{
	RemoveAllAmbientAreas();
}

void AmbientSoundAreas::Draw(Render* pkRender)
{
	Vector3 kColor;

	for(int i=0; i<m_kAmbientAreas.size(); i++)
	{
		vector<Vector3> kPoints;
		GetAllPointsInAmbientArea(m_kAmbientAreas[i].m_strAreaName, kPoints);

		int size = kPoints.size();
		if(size > 1)
		{
			if(m_strAmbientAreaEdited == m_kAmbientAreas[i].m_strAreaName)
				kColor = Vector3(0.5f,0,0.25f);
			else
				kColor = Vector3(1,1,0);

			for(int i=0; i<size; i++)
			{
				if(i+1==size)
				{ 
					pkRender->Line(
						Vector3(kPoints[i].x,0,kPoints[i].z), 
						Vector3(kPoints[0].x,0,kPoints[0].z), kColor);
				}
				else
					pkRender->Line(
						Vector3(kPoints[i].x,0,kPoints[i].z), 
						Vector3(kPoints[i+1].x,0,kPoints[i+1].z), kColor);
			}
		}
	}

}

bool AmbientSoundAreas::Save(string strFileName)
{
	return 1;

	ZFVFile kZFile;
	if( !kZFile.Open(string(strFileName),0,true) ) {
		cout << "Failed to save " << strFileName.c_str() << endl;
		return false;
		}

	int iNumAreas = m_kAmbientAreas.size();
	kZFile.Write(&iNumAreas, sizeof(int), 1);
	
	vector<Vector3> kPoints;

	for(int i=0; i<m_kAmbientAreas.size(); i++)
	{
		kZFile.Write_Str(m_kAmbientAreas[i].m_strAreaName);
		kZFile.Write_Str(GetAmbientSound(m_kAmbientAreas[i].m_strAreaName));
 
		GetAllPointsInAmbientArea(m_kAmbientAreas[i].m_strAreaName, kPoints);

		int iNumPoints = kPoints.size();
		kZFile.Write(&iNumPoints, sizeof(int), 1);

		for(int i=0; i<kPoints.size(); i++)
		{
			kZFile.Write(&kPoints[i].x, sizeof(float), 1);
			kZFile.Write(&kPoints[i].z, sizeof(float), 1);
		}

		kPoints.clear();
	}

	return true;
}

bool AmbientSoundAreas::Load(string strFileName, ZGuiListbox* pkList)
{
	return 1;

	ZFVFile kZFile;
	if( !kZFile.Open(string(strFileName),0,false) ) {
		cout << "Failed to load " << strFileName.c_str() << endl;
		return false;
		}

	RemoveAllAmbientAreas();
	pkList->RemoveAllItems(); 
	
	int iNumAreas;
	kZFile.Read(&iNumAreas, sizeof(int), 1);

	for(int i=0; i<iNumAreas; i++)
	{
		string strArea, strFile;
		kZFile.Read_Str(strArea);
		kZFile.Read_Str(strFile);

		CreateNewAmbientArea(strArea);
		SetAmbientSound(strArea, strFile);
		pkList->AddItem((char*)strArea.c_str(), i, false);

		int iNumPoints;
		kZFile.Read(&iNumPoints, sizeof(int), 1);

		for(int i=0; i<iNumPoints; i++)
		{
			float x,y;
			kZFile.Read(&x, sizeof(float), 1);
			kZFile.Read(&y, sizeof(float), 1);
			AddPointToAmbientArea(strArea, Vector2(x,y));
		}
	}

	return true;
}

void AmbientSoundAreas::CreateNewAmbientArea(string strArea)
{
	AmbientAreaInfo kNewArea;
	kNewArea.m_strAreaName = strArea;
	kNewArea.m_strFileName = "";
	m_kAmbientAreas.push_back(kNewArea);
}

void AmbientSoundAreas::RemoveAllAmbientAreas()
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
		for(int j=0; j<m_kAmbientAreas[i].m_kPolygon.size(); j++)
			delete m_kAmbientAreas[i].m_kPolygon[j];

	m_kAmbientAreas.clear();
}

void AmbientSoundAreas::RemoveAmbientArea(string strArea)
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
		if(m_kAmbientAreas[i].m_strAreaName == strArea)
		{
			for(int j=0; j<m_kAmbientAreas[i].m_kPolygon.size(); j++)
				delete m_kAmbientAreas[i].m_kPolygon[j];

			m_kAmbientAreas.erase( m_kAmbientAreas.begin() + i );
			break;
		}
}

void AmbientSoundAreas::SetAmbientSound(string strArea, string strFile)
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
		if(m_kAmbientAreas[i].m_strAreaName == strArea)
		{
			m_kAmbientAreas[i].m_strFileName = strFile;
			break;
		}
}

bool AmbientSoundAreas::AddPointToAmbientArea(string strArea, Vector2 point)
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
		if(m_kAmbientAreas[i].m_strAreaName == strArea)
		{
			m_kAmbientAreas[i].m_kPolygon.push_back( new Vector2(point) );
			return true;
		}

	return false;
}

bool AmbientSoundAreas::MoveAmbientAreaPoint(string strArea, Vector2 old_pos, Vector2 new_pos)
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
		if(m_kAmbientAreas[i].m_strAreaName == strArea)
		{
			for(int j=0; j<m_kAmbientAreas[i].m_kPolygon.size(); j++)
			{
				if(*(m_kAmbientAreas[i].m_kPolygon[j]) == old_pos)
				{
					m_kAmbientAreas[i].m_kPolygon[j]->x = new_pos.x;
					m_kAmbientAreas[i].m_kPolygon[j]->y = new_pos.y;
					return true;
				}
			}
		}

	return false;
}

void AmbientSoundAreas::ClearAllPointsInAmbientArea(string strArea)
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
	{
		if(m_kAmbientAreas[i].m_strAreaName == strArea)
		{
			for(int j=0; j<m_kAmbientAreas[i].m_kPolygon.size(); j++)
				delete m_kAmbientAreas[i].m_kPolygon[j];

			m_kAmbientAreas[i].m_kPolygon.clear();
		}
	}
}

void AmbientSoundAreas::GetAllPointsInAmbientArea(string strAreaName, vector<Vector3>& kPoints)
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
	{
		if(m_kAmbientAreas[i].m_strAreaName == strAreaName)
		{
			for(int j=0; j<m_kAmbientAreas[i].m_kPolygon.size(); j++)
			{
				kPoints.push_back(Vector3(m_kAmbientAreas[i].m_kPolygon[j]->x,0,
					m_kAmbientAreas[i].m_kPolygon[j]->y));
			}
			break;
		}
	}
}

string AmbientSoundAreas::GetAmbientSound(string strArea)
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
		if(m_kAmbientAreas[i].m_strAreaName == strArea)
			return m_kAmbientAreas[i].m_strFileName;

	return "";
}

void AmbientSoundAreas::Update(ZFAudioSystem* pkAudioSys)
{
	if(m_iPlayingSoundID)
		pkAudioSys->MoveSound(m_iPlayingSoundID,
			pkAudioSys->GetListnerPos(), Vector3(0,0,0), 1.0f); 
}