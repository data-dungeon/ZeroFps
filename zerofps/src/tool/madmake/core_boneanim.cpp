#include "../../zerofps/basic/basicmath.pkg"
#include "mad.h"

















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


/*
	AnimName
	Num Of Frame
	<Frames>
*/


void Mad_CoreBoneAnimation::Save(FILE* fp)
{

}

void Mad_CoreBoneAnimation::Load(FILE* fp)
{

}

