#include "../../zerofps/basic/basicmath.pkg"
#include "mad.h"



void Mad_KeyFrame::Clear(void)
{
	iVertexFrame = 0;
	fFrameTime = 0;
}

void Mad_KeyFrame::operator=(const Mad_KeyFrame& kOther)
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
