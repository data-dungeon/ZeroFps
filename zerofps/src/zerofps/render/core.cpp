#include "mad.h"
#include "../oglload/zfpsgl.h"

Core::Core()
{
	pakTextureCoo = NULL;
	pakFrames = NULL;
	pakFaces = NULL;

	pkTextureManger = NULL;
	
	iActiveFrame = 0;
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

	// Write triangles.
	pakFaces = new MadFace [kHead.iNumOfFaces];
	fread(pakFaces,sizeof(MadFace),kHead.iNumOfFaces,fp);

	fclose(fp);
}

void Core::SetFrameI(int iFrame)
{
	if(iFrame >= 0 && iFrame < kHead.iNumOfFrames)
		iActiveFrame = iFrame;
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
	
	pkTextureManger->BindTexture("file:../data/textures/bitch.bmp",0);
	glCullFace(GL_FRONT);

	glVertexPointer(3,GL_FLOAT,0,pakFrames[iActiveFrame].pVertex);
	glTexCoordPointer(2,GL_FLOAT,0,pakTextureCoo);
	
	glDrawElements(GL_TRIANGLES,kHead.iNumOfFaces*3,GL_UNSIGNED_INT,pakFaces[0].iIndex);
	glCullFace(GL_BACK);

	glPopMatrix();
}




