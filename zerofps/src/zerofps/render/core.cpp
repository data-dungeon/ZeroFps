#include "mad.h"
#include "../oglload/zfpsgl.h"

Core::Core()
{
	pakTextureCoo = NULL;
	pakFrames = NULL;
	pakFaces = NULL;

	pkTextureManger = NULL;
	
	iActiveFrame = 0;
	iActiveKeyFrame = 0;
}

Core::~Core()
{
	if(pakTextureCoo)
		delete [] pakTextureCoo;
	if(pakFrames)
		delete [] pakFrames;
	if(pakFaces)
		delete [] pakFaces;
}

void Core::Load(char* MadFileName)
{
	strcpy(Name, MadFileName);

	FILE* fp = fopen(MadFileName, "rb");

	// Read head
	fread(&kHead,sizeof(Mad_Header),1,fp);

	// Read textures
	fread((void *)akTextures,sizeof(Mad_Texture),kHead.iNumOfTextures,fp);

	// Read Texture Coo
	pakTextureCoo = new MadTextureCoo [kHead.iNumOfVertex];
	fread((void *)pakTextureCoo,sizeof(MadTextureCoo),kHead.iNumOfVertex,fp);

	// Write Alla vertex Frames.
	pakFrames = new Mad_VertexFrame [kHead.iNumOfFrames];
	for(int i=0; i<kHead.iNumOfFrames; i++)
	{
		pakFrames[i].pVertex = new MadVertex [kHead.iNumOfVertex];
		fread(pakFrames[i].pVertex,sizeof(MadVertex),kHead.iNumOfVertex,fp);
	}

	// Read triangles.
	pakFaces = new MadFace [kHead.iNumOfFaces];
	fread(pakFaces,sizeof(MadFace),kHead.iNumOfFaces,fp);

	// Read Animations.
	int iNumOfAnimations;
	fread(&iNumOfAnimations,sizeof(int), 1 ,fp);
	cout << "iNumOfAnimations " << iNumOfAnimations << endl;	

	Mad_Animation kNyAnim;
	Mad_KeyFrame kNyKey;

	for(int iA = 0; iA < iNumOfAnimations; iA++)
	{
		kNyAnim.Clear();
		fread(kNyAnim.Name,sizeof(char), 64 ,fp);

		int iNumOfKeyFrames;
		fread(&iNumOfKeyFrames,sizeof(int), 1 ,fp);
	
		for(int iK = 0; iK < iNumOfKeyFrames; iK++ )
		{
			kNyKey.Clear();
			fread(&kNyKey.iVertexFrame,sizeof(int), 1 ,fp);
			kNyAnim.KeyFrame.push_back(kNyKey);
			cout << "VertexIndex " << kNyKey.iVertexFrame << endl;
		}

		akAnimation.push_back(kNyAnim);
	}

	fclose(fp);
}

void Core::SetFrameI(int iFrame)
{
	if(iFrame >= 0 && iFrame < kHead.iNumOfFrames)
		iActiveFrame = iFrame;
}

void Core::LoopPlayAnim(int iAnim)
{
	iActiveKeyFrame++;
	if(iActiveKeyFrame >= akAnimation[iAnim].KeyFrame.size())
		iActiveKeyFrame = 0;

//	cout << akAnimation[iAnim].KeyFrame[iActiveKeyFrame].iVertexFrame;
	iActiveFrame = akAnimation[iAnim].KeyFrame[iActiveKeyFrame].iVertexFrame;

	cout << iActiveKeyFrame << "/" << iActiveFrame << endl;
}


void Core::SetFrame_NormalizedTime(float fNormTime)
{
	
}

void Core::SetFrame_InSeconds(float fSeconds)
{

}

char* Core::GetName(void)
{
	return NULL;
}

void Core::draw()
{
	glColor3f(1,1,1);
	glDisable(GL_LIGHTING);

	glPushMatrix();
	int iVIndex;
	
	char nisse[256];
	sprintf(nisse, "%s.bmp", akTextures[0].ucTextureName);
	pkTextureManger->BindTexture(nisse,0);
//	pkTextureManger->BindTexture("file:../data/textures/bitch.bmp",0);
	glCullFace(GL_FRONT);

	glVertexPointer(3,GL_FLOAT,0,pakFrames[iActiveFrame].pVertex);
	glTexCoordPointer(2,GL_FLOAT,0,pakTextureCoo);
	
	glDrawElements(GL_TRIANGLES,kHead.iNumOfFaces*3,GL_UNSIGNED_INT,pakFaces[0].iIndex);
	glCullFace(GL_BACK);

	glPopMatrix();
}

float Core::GetAnimationLengthInS(int iAnim)
{
	int iNumOfKeyFrames = akAnimation[iAnim].KeyFrame.size();
	return (iNumOfKeyFrames * 0.1);
}

int Core::GetAnimationTimeInFrames(int iAnim)
{
	return akAnimation[iAnim].KeyFrame.size();
}



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

void Mad_Animation::Clear(void)
{
	strcpy(Name, "");
	KeyFrame.clear();
}

void Mad_Animation::operator=(const Mad_Animation& kOther)
{
	strcpy(Name, kOther.Name);
	KeyFrame = kOther.KeyFrame;
}
