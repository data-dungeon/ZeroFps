#include "mad_core.h"

Mad_CoreBoneAnimation::Mad_CoreBoneAnimation()
{
	Clear();
}

Mad_CoreBoneAnimation::~Mad_CoreBoneAnimation()
{

}

void Mad_CoreBoneAnimation::Clear(void)
{
	strcpy(m_szName, "");
	m_kBoneKeyFrames.clear();
}

void Mad_CoreBoneAnimation::operator=(const Mad_CoreBoneAnimation& kOther)
{
	strcpy(m_szName, kOther.m_szName);
	m_kBoneKeyFrames = kOther.m_kBoneKeyFrames;
}

int Mad_CoreBoneAnimation::Size()
{
	return m_kBoneKeyFrames.size();
}

void Mad_CoreBoneAnimation::Resize(int iNewSize)
{
	m_kBoneKeyFrames.resize(iNewSize);
}

void Mad_CoreBoneAnimation::PushBack(Mad_CoreBoneKeyFrame kBoneKeyFrame)
{
	m_kBoneKeyFrames.push_back(kBoneKeyFrame);
}

/*
	AnimName
	Num Of Frame
	<Frames>
*/


void Mad_CoreBoneAnimation::Save(FILE* fp)
{
	fwrite(m_szName,1,MAD_MAX_ANIMATIONNAME,fp);

	int iNumOfFrames = m_kBoneKeyFrames.size();
	fwrite(&iNumOfFrames,1,sizeof(int),fp);

	for(unsigned int i=0; i<m_kBoneKeyFrames.size(); i++) 
		m_kBoneKeyFrames[i].Save(fp);
}

void Mad_CoreBoneAnimation::Load(FILE* fp)
{
	cout << "Now loading: ";
	Mad_CoreBoneKeyFrame	NewBoneKeyFrame;
	fread(m_szName,1,MAD_MAX_ANIMATIONNAME,fp);
	cout << m_szName << endl;

	int iNumOfFrames;
	fread(&iNumOfFrames,1,sizeof(int),fp);
	cout << "Frames: " << iNumOfFrames<< endl;
	for(int i=0; i<iNumOfFrames; i++) 
	{
		NewBoneKeyFrame.Clear();
		NewBoneKeyFrame.Load(fp);
		m_kBoneKeyFrames.push_back(NewBoneKeyFrame);
	}
}



void Mad_CoreBoneKeyFrame::Save(FILE* fp)
{
	int iNumOfBones = m_kBonePose.size();
	fwrite(&iNumOfBones,1,sizeof(int),fp);

	for(unsigned int i=0; i<m_kBonePose.size(); i++)
		m_kBonePose[i].Save(fp);
}

void Mad_CoreBoneKeyFrame::Load(FILE* fp)
{
	Mad_CoreBoneKey	NewBoneKey;

	int iNumOfBones;
	fread(&iNumOfBones,1,sizeof(int),fp);
	
	for(int i=0; i<iNumOfBones; i++)
	{
		NewBoneKey.Clear();
		NewBoneKey.Load(fp);
		m_kBonePose.push_back(NewBoneKey);
	}
}

int Mad_CoreBoneKeyFrame::Size()
{
	return m_kBonePose.size();
}

void Mad_CoreBoneKeyFrame::Resize(int iNewSize)
{
	m_kBonePose.resize(iNewSize);
}

void Mad_CoreBoneKeyFrame::PushBack(Mad_CoreBoneKey kBoneKey)
{
	m_kBonePose.push_back(kBoneKey);
}






















void Mad_CoreBoneKey::Save(FILE* fp)
{
	fwrite(&m_kPosition, 1, sizeof(Vector3), fp);
	fwrite(&m_kRotation, 1, sizeof(Vector3), fp);
}

void Mad_CoreBoneKey::Load(FILE* fp)
{
	fread(&m_kPosition, 1, sizeof(Vector3), fp);
	fread(&m_kRotation, 1, sizeof(Vector3), fp);
}



