#include "mad_core.h"


Mad_CoreMesh::Mad_CoreMesh()
{
	Clear();
}

Mad_CoreMesh::~Mad_CoreMesh()
{

}

void Mad_CoreMesh::Clear(void)
{
	strcpy(m_acName, "");
	kHead.iNumOfAnimation = 0;
	kHead.iNumOfFaces = 0;
	kHead.iNumOfFrames = 0;
	kHead.iNumOfSubMeshes = 0;
	kHead.iNumOfTextures = 0;
	kHead.iNumOfVertex = 0;
	kHead.iVersionNum = 0;

	bNotAnimated = false;
	iDisplayID = -1;

	akTextureCoo.clear();
	akFaces.clear();
	akFrames.clear();
	akSubMeshes.clear();
	akAnimation.clear();
	akBoneConnections.clear();
}

void Mad_CoreMesh::operator=(const Mad_CoreMesh& kOther)
{
	strcpy(m_acName, kOther.m_acName);
	kHead.iNumOfAnimation	= kOther.kHead.iNumOfAnimation;
	kHead.iNumOfFaces		= kOther.kHead.iNumOfFaces;
	kHead.iNumOfFrames		= kOther.kHead.iNumOfFrames;
	kHead.iNumOfSubMeshes	= kOther.kHead.iNumOfSubMeshes;
	kHead.iNumOfTextures	= kOther.kHead.iNumOfTextures;
	kHead.iNumOfVertex		= kOther.kHead.iNumOfVertex;
	kHead.iVersionNum		= kOther.kHead.iVersionNum;

	akTextureCoo = kOther.akTextureCoo;
	akFaces = kOther.akFaces;
	akFrames = kOther.akFrames;
	akSubMeshes = kOther.akSubMeshes;
	akAnimation = kOther.akAnimation;
	akBoneConnections = kOther.akBoneConnections;
}

void Mad_CoreMesh::CreateRigidBoneConnections(int iBoneId)
{
	akBoneConnections.resize(kHead.iNumOfVertex);
	for(unsigned int i=0; i<akBoneConnections.size(); i++)
		akBoneConnections[i] = iBoneId;

}


void Mad_CoreMesh::ShowInfo(void)
{
	cout << "Textures: " << kHead.iNumOfTextures << endl;
	cout << "Vertex: " << kHead.iNumOfVertex << endl;
	cout << "Triangles: " << kHead.iNumOfFaces << endl;
	cout << "Frames: " << kHead.iNumOfFrames << endl;
	cout << "SubMesh: " << kHead.iNumOfSubMeshes << endl;
	cout << "Animations: " << kHead.iNumOfAnimation << endl;

	for (unsigned int i=0; i<akAnimation.size(); i++) 
	{
		cout << " " << i << " : " << akAnimation[i].Name << " : Frames = " << akAnimation[i].KeyFrame.size() << endl;
	}
}


void Mad_CoreMesh::Save(FILE* fp)
{
	kHead.iNumOfAnimation = akAnimation.size();

	ShowInfo();

	// Write Head.
	fwrite(&kHead,sizeof(Mad_CoreMeshHeader),1,fp);

	// Write SubMesh
	fwrite(&akSubMeshes[0],sizeof(Mad_CoreSubMesh),kHead.iNumOfSubMeshes,fp);


	// Write Textures
	fwrite((void *)&akTextures[0],sizeof(Mad_CoreTexture),kHead.iNumOfTextures,fp);

	// Write Texture Coo
	fwrite((void *)&akTextureCoo[0],sizeof(Mad_TextureCoo),kHead.iNumOfVertex,fp);

	// Write Bone Vikter
	fwrite((void *)&this->akBoneConnections[0],sizeof(int),kHead.iNumOfVertex,fp);

	// Write Alla vertex Frames.
	int size;
	for(int i=0; i<kHead.iNumOfFrames; i++) {
		size = akFrames[i].akVertex.size();
		fwrite(&akFrames[i].akVertex[0],sizeof(Vector3),kHead.iNumOfVertex,fp);
		size = akFrames[i].akNormal.size();
		fwrite(&akFrames[i].akNormal[0],sizeof(Vector3),kHead.iNumOfVertex,fp);
		}

	// Write triangles.
	fwrite(&akFaces[0],sizeof(Mad_Face),kHead.iNumOfFaces,fp);

	// Write Animations.
	int iNumOfAnimations = this->akAnimation.size();
	fwrite(&iNumOfAnimations,sizeof(int), 1 ,fp);

	vector<Mad_CoreMeshAnimation>::iterator		itAnim;
	vector<Mad_CoreKeyFrame>::iterator		itKeyF;

	for(itAnim = akAnimation.begin(); itAnim != akAnimation.end(); itAnim++)
	{
		fwrite(itAnim->Name,sizeof(char), 64 ,fp);
		int iNumOfKeyFrames = itAnim->KeyFrame.size();
		fwrite(&iNumOfKeyFrames,sizeof(int), 1 ,fp);
		
		for(itKeyF = itAnim->KeyFrame.begin(); itKeyF != itAnim->KeyFrame.end(); itKeyF++)
		{
			fwrite(&itKeyF->iVertexFrame,sizeof(int), 1 ,fp);
		}
	}
	
}

void Mad_CoreMesh::Load(FILE* fp)
{
	int i,j;
	// Read head
	fread(&kHead,sizeof(Mad_CoreMeshHeader),1,fp);

	// Read SubMesh
	for(i = 0; i<kHead.iNumOfSubMeshes; i++) {
		Mad_CoreSubMesh	kSubMesh;
		fread(&kSubMesh,sizeof(Mad_CoreSubMesh),1,fp);
		akSubMeshes.push_back(kSubMesh);

		}

	// Read textures
	for(i = 0; i<kHead.iNumOfTextures; i++) {
		Mad_CoreTexture	kTexture;
		fread(&kTexture,sizeof(Mad_CoreTexture),1,fp);
		akTextures.push_back(kTexture);
		}
//	fread((void *)akTextures,sizeof(Mad_Texture),kHead.iNumOfTextures,fp);

	// Read Texture Coo
	for(i = 0; i<kHead.iNumOfVertex; i++) {
		Mad_TextureCoo	kTexCoo;
		fread(&kTexCoo,sizeof(Mad_TextureCoo),1,fp);
		akTextureCoo.push_back(kTexCoo);
		}

	// Read Bone Vikter
	for(i = 0; i<kHead.iNumOfVertex; i++) {
		int iBoneVikt;
		fread(&iBoneVikt,sizeof(int),1,fp);
		akBoneConnections.push_back(iBoneVikt);
		}


	// Read Alla vertex Frames.
	Vector3* pkVector = new Vector3 [kHead.iNumOfVertex];
	for(i = 0; i<kHead.iNumOfFrames; i++) {
		Mad_CoreVertexFrame	kVertexFrame;

		fread(pkVector,sizeof(Vector3),kHead.iNumOfVertex,fp);
		for(j=0; j<kHead.iNumOfVertex; j++)
			kVertexFrame.akVertex.push_back(pkVector[j]);

		fread(pkVector,sizeof(Vector3),kHead.iNumOfVertex,fp);
		for(j=0; j<kHead.iNumOfVertex; j++)
			kVertexFrame.akNormal.push_back(pkVector[j]);

		akFrames.push_back(kVertexFrame);
	}

	delete [] pkVector;

	// Read triangles.
	for(i = 0; i<kHead.iNumOfFaces; i++) {
		Mad_Face	kFace;
		fread(&kFace, sizeof(Mad_Face),1,fp);
		akFaces.push_back(kFace);

		}

	// Read Animations.
	int iNumOfAnimations;
	fread(&iNumOfAnimations,sizeof(int), 1 ,fp);

	Mad_CoreMeshAnimation kNyAnim;
	Mad_CoreKeyFrame kNyKey;

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
//		cout << "Should Load: " << nisse << endl;
//		aiTextureIndex[i] = pkTextureManger->Load(nisse,0);
	}

}

Mad_CoreMeshAnimation*	Mad_CoreMesh::GetAnimation(char* ucaName)
{
	vector<Mad_CoreMeshAnimation>::iterator it;

	for(it = akAnimation.begin(); it != akAnimation.end(); it++)
	{
		if(strcmp(it->Name, ucaName) == 0)
			return &(*it);
	}

	// Finns ingen animation med det namnet så skapa den och returnera den.
	Mad_CoreMeshAnimation kNewAnim;
	kNewAnim.Clear();
	strcpy(kNewAnim.Name, ucaName);
	akAnimation.push_back(kNewAnim);
	return &akAnimation.back();
}

void Mad_CoreMesh::CreateVertexNormals()
{
	Vector3 kSurfNormal;
	Vector3 kA, kB;
	int v,f;

	vector<Vector3> kSurfNormals;
	kSurfNormals.resize(kHead.iNumOfFaces);

	for(unsigned int i=0; i<this->akFrames.size(); i++) {
		// Clear all normals.
		for( v = 0; v<kHead.iNumOfVertex; v++)
			akFrames[i].akNormal[v].Set(0,0,0);

		// Create Surface Normals
		for( f = 0; f<kHead.iNumOfFaces; f++) {
			kA = akFrames[i].akVertex[ akFaces[f].iIndex[0] ] - akFrames[i].akVertex[ akFaces[f].iIndex[1] ];
			kB = akFrames[i].akVertex[ akFaces[f].iIndex[2] ] - akFrames[i].akVertex[ akFaces[f].iIndex[1] ];
			
			kSurfNormal = kA.Cross(kB);
			kSurfNormals[f] = kSurfNormal;
			}

		// Add Surface to vertex normals
		for( f = 0; f<kHead.iNumOfFaces; f++) {
			akFrames[i].akNormal[ akFaces[f].iIndex[0] ] += kSurfNormals[f];
			akFrames[i].akNormal[ akFaces[f].iIndex[1] ] += kSurfNormals[f];
			akFrames[i].akNormal[ akFaces[f].iIndex[2] ] += kSurfNormals[f];
			}


		// Normalize Surface normals.
		for( v = 0; v<kHead.iNumOfVertex; v++)
			if(akFrames[i].akNormal[v].Length() > 0)
				akFrames[i].akNormal[v].Normalize();
		}
}

void Mad_CoreMesh::OptimizeSubMeshes()
{
	if(akSubMeshes.size() < 2)	return;

	cout << "OptimizeSubMeshes '" << m_acName  << "' " << akSubMeshes.size() << endl;

	vector<Mad_CoreSubMesh>	akOldSubMesh;
	akOldSubMesh = akSubMeshes;
	akSubMeshes.clear();

	Mad_CoreSubMesh newsub;
	newsub.iFirstTriangle	= 0;
	newsub.iNumOfTriangles	= 1;
	newsub.iTextureIndex	= akOldSubMesh[0].iTextureIndex;

	unsigned int i;

	for(i=1; i<akOldSubMesh.size(); i++) {
		if(newsub.iTextureIndex != akOldSubMesh[i].iTextureIndex) {
			cout << "/" << endl;
			akSubMeshes.push_back(newsub);
			newsub.iFirstTriangle	= i;
			newsub.iNumOfTriangles	= 1;
			newsub.iTextureIndex	= akOldSubMesh[i].iTextureIndex;
			}
		else {
			cout << ".";
			newsub.iNumOfTriangles++;
			}
		}

	akSubMeshes.push_back(newsub);
	cout << "End OptimizeSubMeshes: " << akSubMeshes.size() << endl;

	for(i=0; i<akSubMeshes.size(); i++) {
		cout << "SubMesh[" << i << "]:" << akSubMeshes[i].iFirstTriangle;
		cout << " / " << akSubMeshes[i].iNumOfTriangles;
		cout << " / " << akSubMeshes[i].iTextureIndex << endl;

		}
}




int	Mad_CoreMesh::AddTexture(char* ucpTextureName)
{
	vector<Mad_CoreTexture>::iterator itTexture;
	int iTextureIndex = 0;

	char* ext = strstr(ucpTextureName, ".");
	ext[0] = 0;


	for(itTexture = akTextures.begin(); itTexture != akTextures.end(); itTexture++)
	{
		if(strcmp(itTexture->ucTextureName, ucpTextureName) == 0)
			return iTextureIndex; 

		iTextureIndex++;
	}

	Mad_CoreTexture madtex;
	strcpy(madtex.ucTextureName, ucpTextureName);
	akTextures.push_back(madtex);

	return iTextureIndex; 
}

void Mad_CoreMesh::ResizeTextures(int iNewSize)
{
	akTextures.resize(iNewSize);
}

void Mad_CoreMesh::ResizeTexturesCoo(int iNewSize)
{
	akTextureCoo.resize(iNewSize);
}

void Mad_CoreMesh::ResizeFaces(int iNewSize)
{
	akFaces.resize(iNewSize);
}

void Mad_CoreMesh::ResizeFrames(int iNewSize)
{
	akFrames.resize(iNewSize);
}

void Mad_CoreMesh::ResizeSubMesh(int iNewSize)
{
	akSubMeshes.resize(iNewSize);
}

void Mad_CoreMesh::ResizeAnimations(int iNewSize)
{
	akAnimation.resize(iNewSize);
}

void Mad_CoreMesh::ResizeBoneConnections(int iNewSize)
{
	akBoneConnections.resize(iNewSize);
}

int Mad_CoreMesh::SizeTextures()
{
	return akTextures.size();
}

int Mad_CoreMesh::SizeTexturesCoo()
{
	return akTextureCoo.size();
}

int Mad_CoreMesh::SizeFaces()
{
	return akFaces.size();
}
int Mad_CoreMesh::SizeFrames()
{
	return akFrames.size();
}

int Mad_CoreMesh::SizeSubMesh()
{
	return akSubMeshes.size();
}

int Mad_CoreMesh::SizeAnimations()
{
	return akAnimation.size();
}

int Mad_CoreMesh::SizeBoneConnections()
{
	return akBoneConnections.size();
}

void Mad_CoreMesh::PushBackTexture(Mad_CoreTexture kTexture)
{
	akTextures.push_back(kTexture);
}

void Mad_CoreMesh::PushBackTextureCoo(Mad_TextureCoo kTextureCoo)
{
	akTextureCoo.push_back(kTextureCoo);
}

void Mad_CoreMesh::PushBackFaces(Mad_Face kFace)
{
	akFaces.push_back(kFace);
}

void Mad_CoreMesh::PushBackFrames(Mad_CoreVertexFrame kFrames)
{
	akFrames.push_back(kFrames);

}

void Mad_CoreMesh::PushBackSubMeshes(Mad_CoreSubMesh kSubMesh)
{
	akSubMeshes.push_back(kSubMesh);

}

void Mad_CoreMesh::PushBackAnimation(Mad_CoreMeshAnimation kAnimation)
{
	akAnimation.push_back(kAnimation);

}

void Mad_CoreMesh::PushBackBoneConnection(int iBoneConnection)
{
	akBoneConnections.push_back(iBoneConnection);

}

int Mad_CoreMesh::NumOfVertexPerFrame()
{
	return akFrames[0].akVertex.size();
}

Mad_TextureCoo* Mad_CoreMesh::GetTexCoo()
{
	return &akTextureCoo[0];

}

Mad_CoreSubMesh* Mad_CoreMesh::GetSubMesh(int iSubMesh)
{
	return &akSubMeshes[iSubMesh];
}

Mad_Face* Mad_CoreMesh::GetFace(int iFace)
{
	return &akFaces[iFace];
}

char* Mad_CoreMesh::GetTextureName(int iTextureIndex)
{
	return akTextures[iTextureIndex].ucTextureName;
}

Mad_CoreTexture* Mad_CoreMesh::GetTextureInfo(int iTextureIndex)
{
	return &akTextures[iTextureIndex];
}

void Mad_CoreMesh::SetTextureID(int iTextureIndex, int iID)
{
	iTextureID[iTextureIndex] = iID;
}

int Mad_CoreMesh::GetTextureID(int iTextureIndex)
{
	return iTextureID[iTextureIndex];
}

