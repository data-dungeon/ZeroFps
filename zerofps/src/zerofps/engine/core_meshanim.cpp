#include "mad_core.h"



void Mad_CoreKeyFrame::Clear(void)
{
	iVertexFrame = 0;
	fFrameTime = 0;
}

void Mad_CoreKeyFrame::operator=(const Mad_CoreKeyFrame& kOther)
{
	iVertexFrame = kOther.iVertexFrame;
	fFrameTime = kOther.fFrameTime;
}

void Mad_CoreMeshAnimation::Clear(void)
{
	strcpy(Name, "");
	KeyFrame.clear();
}

void Mad_CoreMeshAnimation::operator=(const Mad_CoreMeshAnimation& kOther)
{
	strcpy(Name, kOther.Name);
	KeyFrame = kOther.KeyFrame;
}



int Mad_CoreMeshAnimation::Size()
{
	return KeyFrame.size();
}

void Mad_CoreMeshAnimation::Resize(int iNewSize)
{
	KeyFrame.resize(iNewSize);
}

void Mad_CoreMeshAnimation::PushBack(Mad_CoreKeyFrame kKeyFrame)
{
	KeyFrame.push_back(kKeyFrame);
}
