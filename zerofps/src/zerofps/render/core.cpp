#include "mad.h"
#include "../ogl/zfpsgl.h"

Core::Core()
{
	pakTextureCoo	= NULL;
	pakFrames		= NULL;
	pakFaces		= NULL;
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
	int i;

	strcpy(Name, MadFileName);

	FILE* fp = fopen(MadFileName, "rb");

	// Read head
	fread(&kHead,sizeof(Mad_Header),1,fp);

	// Read SubMesh
	pkSubMesh = new Mad_SubMesh [kHead.iNumOfSubMeshes];
	fread((void *)pkSubMesh,sizeof(Mad_SubMesh),kHead.iNumOfSubMeshes,fp);

	// Read textures
	fread((void *)akTextures,sizeof(Mad_Texture),kHead.iNumOfTextures,fp);

	// Read Texture Coo
	pakTextureCoo = new MadTextureCoo [kHead.iNumOfVertex];
	fread((void *)pakTextureCoo,sizeof(MadTextureCoo),kHead.iNumOfVertex,fp);

	// Write Alla vertex Frames.
	pakFrames = new Mad_VertexFrame [kHead.iNumOfFrames];
	for(i=0; i<kHead.iNumOfFrames; i++)
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
		}

		akAnimation.push_back(kNyAnim);
	}

	// Load Textures
	char nisse[256];
	for(i = 0; i< kHead.iNumOfTextures; i++) {
		sprintf(nisse, "../data/textures/%s.bmp", akTextures[i].ucTextureName);
		aiTextureIndex[i] = pkTextureManger->Load(nisse,0);
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

	iActiveFrame = akAnimation[iAnim].KeyFrame[iActiveKeyFrame].iVertexFrame;
}


void Core::SetFrame_NormalizedTime(float fNormTime)
{
	
}

void Core::SetFrame_InSeconds(float fSeconds)
{

}

char* Core::GetName(void)
{
	return Name;
}

void Core::draw()
{
	glColor3f(1,1,1);
	
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );

	glDisable(GL_LIGHTING);

	glVertexPointer(3,GL_FLOAT,0,pakFrames[iActiveFrame].pVertex);
	glTexCoordPointer(2,GL_FLOAT,0,pakTextureCoo);
	
	int iMADTextureNum;
	int iTextureId = aiTextureIndex[0];

	for(int i=0; i<this->kHead.iNumOfSubMeshes; i++)
	{
		iMADTextureNum = pkSubMesh[i].iTextureIndex;
		iTextureId = aiTextureIndex[ iMADTextureNum ];
		if(aiReplaceTextureIndex[ iMADTextureNum ] != NO_TEXTURE)
			iTextureId = aiReplaceTextureIndex[ iMADTextureNum ];

		pkTextureManger->BindTexture(iTextureId);
		glDrawElements(GL_TRIANGLES,
			pkSubMesh[i].iNumOfTriangles * 3,
			GL_UNSIGNED_INT,
			pakFaces[ pkSubMesh[i].iFirstTriangle ].iIndex);
	}

	/*	
	if(aiReplaceTextureIndex[0] != NO_TEXTURE)
		iTextureId = aiReplaceTextureIndex[0];
	pkTextureManger->BindTexture(iTextureId);
	glDrawElements(GL_TRIANGLES,kHead.iNumOfFaces*3,GL_UNSIGNED_INT,pakFaces[0].iIndex);
	*/

	glPopAttrib();
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

void Core::SetReplaceTexture(char* szFileName)
{
	char nisse[256];
	for(int i = 0; i< kHead.iNumOfTextures; i++) {
		sprintf(nisse, "../data/textures/%s.bmp", szFileName);
		aiReplaceTextureIndex[i] = pkTextureManger->Load(nisse,0);
	}

}

void Core::ClearReplaceTexture(void)
{
	for(int i = 0; i< kHead.iNumOfTextures; i++)
		aiReplaceTextureIndex[i] = NO_TEXTURE;
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
