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


void Mad_CoreBoneAnimation::Save(ZFVFile* pkZFile)
{
	pkZFile->Write(m_szName,1,MAD_MAX_ANIMATIONNAME);
	//fwrite(m_szName,1,MAD_MAX_ANIMATIONNAME,fp);

	int iNumOfFrames = m_kBoneKeyFrames.size();
	pkZFile->Write(&iNumOfFrames,1,sizeof(int));
	//fwrite(&iNumOfFrames,1,sizeof(int),fp);

	for(unsigned int i=0; i<m_kBoneKeyFrames.size(); i++) 
		m_kBoneKeyFrames[i].Save(pkZFile);
}

void Mad_CoreBoneAnimation::Load(ZFVFile* pkZFile)
{
	cout << "Now loading: ";
	Mad_CoreBoneKeyFrame	NewBoneKeyFrame;
	pkZFile->Read(m_szName,1,MAD_MAX_ANIMATIONNAME);
	//fread(m_szName,1,MAD_MAX_ANIMATIONNAME,fp);
	cout << m_szName << endl;

	int iNumOfFrames;
	pkZFile->Read(&iNumOfFrames,1,sizeof(int));
	//fread(&iNumOfFrames,1,sizeof(int),fp);
	cout << "Frames: " << iNumOfFrames<< endl;
	for(int i=0; i<iNumOfFrames; i++) 
	{
		NewBoneKeyFrame.Clear();
		NewBoneKeyFrame.Load(pkZFile);
		m_kBoneKeyFrames.push_back(NewBoneKeyFrame);
	}
}

int Mad_CoreBoneAnimation::GetSizeInBytes()
{
	int iSizeInBytes = 0;
	iSizeInBytes += sizeof(Mad_CoreBoneAnimation);
	for(int i=0; i<m_kBoneKeyFrames.size(); i++) 
		iSizeInBytes += m_kBoneKeyFrames[i].GetSizeInBytes();

	return iSizeInBytes;
}









void Mad_CoreBoneKeyFrame::Save(ZFVFile* pkZFile)
{
	int iNumOfBones = m_kBonePose.size();
	pkZFile->Write(&iNumOfBones,1,sizeof(int));
	//fwrite(&iNumOfBones,1,sizeof(int),fp);

	for(unsigned int i=0; i<m_kBonePose.size(); i++)
		m_kBonePose[i].Save(pkZFile);
}

void Mad_CoreBoneKeyFrame::Load(ZFVFile* pkZFile)
{
	Mad_CoreBoneKey	NewBoneKey;

	int iNumOfBones;
	pkZFile->Read(&iNumOfBones,1,sizeof(int));
	//fread(&iNumOfBones,1,sizeof(int),fp);
	
	for(int i=0; i<iNumOfBones; i++)
	{
		NewBoneKey.Clear();
		NewBoneKey.Load(pkZFile);
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

int  Mad_CoreBoneKeyFrame::GetSizeInBytes()
{
	int iSizeInBytes = 0;
	iSizeInBytes += sizeof(Mad_CoreBoneKeyFrame);
	iSizeInBytes += m_kBonePose.size() * sizeof(Mad_CoreBoneKey);
	return iSizeInBytes;
}




















void Mad_CoreBoneKey::Save(ZFVFile* pkZFile)
{
	pkZFile->Write(&m_kPosition, 1, sizeof(Vector3));
	pkZFile->Write(&m_kRotation, 1, sizeof(Vector3));
	//fwrite(&m_kPosition, 1, sizeof(Vector3), fp);
	//fwrite(&m_kRotation, 1, sizeof(Vector3), fp);
}

void Mad_CoreBoneKey::Load(ZFVFile* pkZFile)
{
	pkZFile->Read(&m_kPosition, 1, sizeof(Vector3));
	pkZFile->Read(&m_kRotation, 1, sizeof(Vector3));
/*		cout << m_kRotation.x << "," 
			 << m_kRotation.y << ","
			 << m_kRotation.z << endl;*/
	//fread(&m_kPosition, 1, sizeof(Vector3), fp);
	//fread(&m_kRotation, 1, sizeof(Vector3), fp);
}



