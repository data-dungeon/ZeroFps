#include "mad_core.h"

Mad_RawMesh::Mad_RawMesh()
{
	Clear();
}

void Mad_RawMesh::Save(ZFVFile* pkZFile)
{
	kHead.iNumOfAnimation = akAnimation.size();

//	ShowInfo();

	// Write Head.
	pkZFile->Write(&kHead,sizeof(Mad_CoreMeshHeader),1);
	//fwrite(&kHead,sizeof(Mad_CoreMeshHeader),1,fp);

	// Write SubMesh
	pkZFile->Write(&akSubMeshes[0],sizeof(Mad_CoreSubMesh),kHead.iNumOfSubMeshes);
	//fwrite(&akSubMeshes[0],sizeof(Mad_CoreSubMesh),kHead.iNumOfSubMeshes,fp);


	// Write Textures
	pkZFile->Write((void *)&akTextures[0],sizeof(Mad_CoreTexture),kHead.iNumOfTextures);
	//fwrite((void *)&akTextures[0],sizeof(Mad_CoreTexture),kHead.iNumOfTextures,fp);

	// Write Texture Coo
	pkZFile->Write((void *)&akTextureCoo[0],sizeof(Mad_TextureCoo),kHead.iNumOfVertex);
	//fwrite((void *)&akTextureCoo[0],sizeof(Mad_TextureCoo),kHead.iNumOfVertex,fp);

	// Write Bone Vikter
	pkZFile->Write((void *)&this->akBoneConnections[0],sizeof(int),kHead.iNumOfVertex);
	//fwrite((void *)&this->akBoneConnections[0],sizeof(int),kHead.iNumOfVertex,fp);

	// Write Alla vertex Frames.
	int size;
	for(int i=0; i<kHead.iNumOfFrames; i++) {
		size = akFrames[i].akVertex.size();
		pkZFile->Write(&akFrames[i].akVertex[0],sizeof(Vector3),kHead.iNumOfVertex);
		//fwrite(&akFrames[i].akVertex[0],sizeof(Vector3),kHead.iNumOfVertex,fp);
		size = akFrames[i].akNormal.size();
		pkZFile->Write(&akFrames[i].akNormal[0],sizeof(Vector3),kHead.iNumOfVertex);
		//fwrite(&akFrames[i].akNormal[0],sizeof(Vector3),kHead.iNumOfVertex,fp);
		}

	// Write triangles.
	pkZFile->Write(&akFaces[0],sizeof(Mad_Face),kHead.iNumOfFaces);
	//fwrite(&akFaces[0],sizeof(Mad_Face),kHead.iNumOfFaces,fp);

	// Write Animations.
	int iNumOfAnimations = this->akAnimation.size();
	pkZFile->Write(&iNumOfAnimations,sizeof(int), 1 );
	//fwrite(&iNumOfAnimations,sizeof(int), 1 ,fp);

	vector<Mad_CoreMeshAnimation>::iterator		itAnim;
	vector<Mad_CoreKeyFrame>::iterator		itKeyF;

	for(itAnim = akAnimation.begin(); itAnim != akAnimation.end(); itAnim++)
	{
		pkZFile->Write(itAnim->Name,sizeof(char), 64);
		//fwrite(itAnim->Name,sizeof(char), 64 ,fp);
		int iNumOfKeyFrames = itAnim->KeyFrame.size();
		pkZFile->Write(&iNumOfKeyFrames,sizeof(int), 1);
		//fwrite(&iNumOfKeyFrames,sizeof(int), 1 ,fp);
		
		for(itKeyF = itAnim->KeyFrame.begin(); itKeyF != itAnim->KeyFrame.end(); itKeyF++)
		{
			pkZFile->Write(&itKeyF->iVertexFrame,sizeof(int), 1);
			//fwrite(&itKeyF->iVertexFrame,sizeof(int), 1 ,fp);
		}
	}
	
}

void Mad_RawMesh::Load(ZFVFile* pkZFile)
{
	int i,j;
	// Read head
	pkZFile->Read(&kHead,sizeof(Mad_CoreMeshHeader),1);
	//fread(&kHead,sizeof(Mad_CoreMeshHeader),1,fp);

	// Read SubMesh
	for(i = 0; i<kHead.iNumOfSubMeshes; i++) {
		Mad_CoreSubMesh	kSubMesh;
		pkZFile->Read(&kSubMesh,sizeof(Mad_CoreSubMesh),1);
		//fread(&kSubMesh,sizeof(Mad_CoreSubMesh),1,fp);
		akSubMeshes.push_back(kSubMesh);

		}

	// Read textures
	for(i = 0; i<kHead.iNumOfTextures; i++) {
		Mad_CoreTexture	kTexture;
		pkZFile->Read(&kTexture,sizeof(Mad_CoreTexture),1);
		//fread(&kTexture,sizeof(Mad_CoreTexture),1,fp);
		akTextures.push_back(kTexture);
		}
//	fread((void *)akTextures,sizeof(Mad_Texture),kHead.iNumOfTextures,fp);

	// Read Texture Coo
	for(i = 0; i<kHead.iNumOfVertex; i++) {
		Mad_TextureCoo	kTexCoo;
		pkZFile->Read(&kTexCoo,sizeof(Mad_TextureCoo),1);
		//fread(&kTexCoo,sizeof(Mad_TextureCoo),1,fp);
		akTextureCoo.push_back(kTexCoo);
		}

	// Read Bone Vikter
	for(i = 0; i<kHead.iNumOfVertex; i++) {
		int iBoneVikt;
		pkZFile->Read(&iBoneVikt,sizeof(int),1);
		//fread(&iBoneVikt,sizeof(int),1,fp);
		akBoneConnections.push_back(iBoneVikt);
		}


	// Read Alla vertex Frames.
	Vector3* pkVector = new Vector3 [kHead.iNumOfVertex];
	for(i = 0; i<kHead.iNumOfFrames; i++) {
		Mad_CoreVertexFrame	kVertexFrame;

		pkZFile->Read(pkVector,sizeof(Vector3),kHead.iNumOfVertex);
		//fread(pkVector,sizeof(Vector3),kHead.iNumOfVertex,fp);
		for(j=0; j<kHead.iNumOfVertex; j++)
			kVertexFrame.akVertex.push_back(pkVector[j]);

		pkZFile->Read(pkVector,sizeof(Vector3),kHead.iNumOfVertex);
		//fread(pkVector,sizeof(Vector3),kHead.iNumOfVertex,fp);
		for(j=0; j<kHead.iNumOfVertex; j++)
			kVertexFrame.akNormal.push_back(pkVector[j]);

		akFrames.push_back(kVertexFrame);
	}

	delete [] pkVector;

	// Read triangles.
	for(i = 0; i<kHead.iNumOfFaces; i++) {
		Mad_Face	kFace;
		pkZFile->Read(&kFace, sizeof(Mad_Face),1);
		//fread(&kFace, sizeof(Mad_Face),1,fp);
		akFaces.push_back(kFace);

		}

	// Read Animations.
	int iNumOfAnimations;
	pkZFile->Read(&iNumOfAnimations,sizeof(int), 1);
	//fread(&iNumOfAnimations,sizeof(int), 1 ,fp);

	Mad_CoreMeshAnimation kNyAnim;
	Mad_CoreKeyFrame kNyKey;

	for(int iA = 0; iA < iNumOfAnimations; iA++)
	{
		kNyAnim.Clear();
		pkZFile->Read(kNyAnim.Name,sizeof(char), 64);
		//fread(kNyAnim.Name,sizeof(char), 64 ,fp);

		int iNumOfKeyFrames;
		pkZFile->Read(&iNumOfKeyFrames,sizeof(int), 1);
		//fread(&iNumOfKeyFrames,sizeof(int), 1 ,fp);
	
		for(int iK = 0; iK < iNumOfKeyFrames; iK++ )
		{
			kNyKey.Clear();
			pkZFile->Read(&kNyKey.iVertexFrame,sizeof(int), 1);
			//fread(&kNyKey.iVertexFrame,sizeof(int), 1 ,fp);
			kNyAnim.KeyFrame.push_back(kNyKey);
		}

		akAnimation.push_back(kNyAnim);
	}

/*	// Load Textures
	char nisse[256];
	for(i = 0; i< kHead.iNumOfTextures; i++) {
		sprintf(nisse, "../data/textures/%s.bmp", akTextures[i].ucTextureName);
//		cout << "Should Load: " << nisse << endl;
//		aiTextureIndex[i] = pkTextureManger->Load(nisse,0);
	}*/

}


void Mad_RawMesh::Clear(void)
{
	kHead.iNumOfAnimation = 0;
	kHead.iNumOfFaces = 0;
	kHead.iNumOfFrames = 0;
	kHead.iNumOfSubMeshes = 0;
	kHead.iNumOfTextures = 0;
	kHead.iNumOfVertex = 0;
	kHead.iVersionNum = 0;

//	iDisplayID = -1;

	akTextureCoo.clear();
	akFaces.clear();
	akFrames.clear();
	akSubMeshes.clear();
	akAnimation.clear();
	akBoneConnections.clear();
}

void Mad_RawMesh::operator=(const Mad_RawMesh& kOther)
{
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

void Mad_RawMesh::ResizeTextures(int iNewSize)
{
	akTextures.resize(iNewSize);
}

void Mad_RawMesh::ResizeTexturesCoo(int iNewSize)
{
	akTextureCoo.resize(iNewSize);
}

void Mad_RawMesh::ResizeFaces(int iNewSize)
{
	akFaces.resize(iNewSize);
}

void Mad_RawMesh::ResizeFrames(int iNewSize)
{
	akFrames.resize(iNewSize);
}

void Mad_RawMesh::ResizeSubMesh(int iNewSize)
{
	akSubMeshes.resize(iNewSize);
}

void Mad_RawMesh::ResizeAnimations(int iNewSize)
{
	akAnimation.resize(iNewSize);
}

void Mad_RawMesh::ResizeBoneConnections(int iNewSize)
{
	akBoneConnections.resize(iNewSize);
}

int Mad_RawMesh::SizeTextures()
{
	return akTextures.size();
}

int Mad_RawMesh::SizeTexturesCoo()
{
	return akTextureCoo.size();
}

int Mad_RawMesh::SizeFaces()
{
	return akFaces.size();
}
int Mad_RawMesh::SizeFrames()
{
	return akFrames.size();
}

int Mad_RawMesh::SizeSubMesh()
{
	return akSubMeshes.size();
}

int Mad_RawMesh::SizeAnimations()
{
	return akAnimation.size();
}

int Mad_RawMesh::SizeBoneConnections()
{
	return akBoneConnections.size();
}

void Mad_RawMesh::PushBackTexture(Mad_CoreTexture kTexture)
{
	akTextures.push_back(kTexture);
}

void Mad_RawMesh::PushBackTextureCoo(Mad_TextureCoo kTextureCoo)
{
	akTextureCoo.push_back(kTextureCoo);
}

void Mad_RawMesh::PushBackFaces(Mad_Face kFace)
{
	akFaces.push_back(kFace);
}

void Mad_RawMesh::PushBackFrames(Mad_CoreVertexFrame kFrames)
{
	akFrames.push_back(kFrames);

}

void Mad_RawMesh::PushBackSubMeshes(Mad_CoreSubMesh kSubMesh)
{
	akSubMeshes.push_back(kSubMesh);

}

void Mad_RawMesh::PushBackAnimation(Mad_CoreMeshAnimation kAnimation)
{
	akAnimation.push_back(kAnimation);

}

void Mad_RawMesh::PushBackBoneConnection(int iBoneConnection)
{
	akBoneConnections.push_back(iBoneConnection);

}

int Mad_RawMesh::NumOfVertexPerFrame()
{
	return akFrames[0].akVertex.size();
}

void Mad_RawMesh::FlipFaces()
{
	int i0,i1,i2;

	for(unsigned int i =0; i < akFaces.size(); i++) {
		i0 = akFaces[i].iIndex[0];
		i1 = akFaces[i].iIndex[1];
		i2 = akFaces[i].iIndex[2];

		akFaces[i].iIndex[0] = i2;
		akFaces[i].iIndex[1] = i1;
		akFaces[i].iIndex[2] = i0;
		}
}



int Mad_RawMesh::GetSizeInBytes()
{
	int iSizeInBytes = 0;
	iSizeInBytes += sizeof(Mad_CoreMesh);

	iSizeInBytes += sizeof(Mad_CoreTexture) * akTextures.size();
	iSizeInBytes += sizeof(Mad_TextureCoo)  * akTextureCoo.size();
	iSizeInBytes += sizeof(Mad_Face)			 * akFaces.size();
	
	for(unsigned int iVf = 0; iVf < akFrames.size(); iVf++)
		iSizeInBytes += akFrames[iVf].GetSizeInBytes();

	iSizeInBytes += sizeof(Mad_CoreSubMesh) * akSubMeshes.size();

	for(unsigned int iMa = 0; iMa < akAnimation.size(); iMa++)
		iSizeInBytes += akAnimation[iMa].GetSizeInBytes();

	iSizeInBytes += sizeof(int) * akBoneConnections.size();

	return iSizeInBytes;
}



Mad_TextureCoo* Mad_RawMesh::GetTexCoo()
{
	return &akTextureCoo[0];

}

Mad_CoreSubMesh* Mad_RawMesh::GetSubMesh(int iSubMesh)
{
	return &akSubMeshes[iSubMesh];
}

Mad_Face* Mad_RawMesh::GetFace(int iFace)
{
	return &akFaces[iFace];
}

char* Mad_RawMesh::GetTextureName(int iTextureIndex)
{
	return akTextures[iTextureIndex].ucTextureName;
}

Mad_CoreTexture* Mad_RawMesh::GetTextureInfo(int iTextureIndex)
{
	return &akTextures[iTextureIndex];
}

void Mad_RawMesh::SetTextureID(int iTextureIndex, int iID)
{
	iTextureID[iTextureIndex] = iID;
}

int Mad_RawMesh::GetTextureID(int iTextureIndex)
{
	return iTextureID[iTextureIndex];
}

void Mad_RawMesh::SetTextureHandle(int iTextureIndex, string strName)
{
	akTexturesHandles[iTextureIndex].SetRes(strName);
}

ZFResourceHandle*	Mad_RawMesh::GetTextureHandle(int iTextureIndex)
{
	return &	akTexturesHandles[iTextureIndex];
}

void Mad_RawMesh::CreateRigidBoneConnections(int iBoneId)
{
	akBoneConnections.resize(kHead.iNumOfVertex);
	for(unsigned int i=0; i<akBoneConnections.size(); i++)
		akBoneConnections[i] = iBoneId;

}

Mad_CoreMeshAnimation*	Mad_RawMesh::GetAnimation(char* ucaName)
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


int	Mad_RawMesh::AddTexture(char* ucpTextureName)
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

void Mad_RawMesh::CreateVertexNormals()
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

void Mad_RawMesh::OptimizeSubMeshes()
{
	if(akSubMeshes.size() < 2)	return;

//	cout << "OptimizeSubMeshes '" << m_acName  << "' " << akSubMeshes.size() << endl;

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

void Mad_RawMesh::SetTextureFlags(void)
{
	char* pkChar;

	for(unsigned int i=0; i<akTextures.size(); i++) {				
		char* pszFlags = strstr(akTextures[i].ucTextureName, "-");		
		
		if(pszFlags == NULL)
			continue;

		pkChar = akTextures[i].ucTextureName;
		while(pkChar < pszFlags) {
			if(pkChar[0] == 'a') {
				akTextures[i].bIsAlphaTest = true;
				}
			if(pkChar[0] == 'c') {
				akTextures[i].bClampTexture = true;
				}

			pkChar++;
			}
		}
}










Mad_RawMesh* Mad_CoreMesh::GetLODMesh(int iId)
{
	return &m_kLodMesh[0];
}

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
/*	kHead.iNumOfAnimation = 0;
	kHead.iNumOfFaces = 0;
	kHead.iNumOfFrames = 0;
	kHead.iNumOfSubMeshes = 0;
	kHead.iNumOfTextures = 0;
	kHead.iNumOfVertex = 0;
	kHead.iVersionNum = 0;*/

	bNotAnimated = false;
//	iDisplayID = -1;
	m_kLodMesh.clear();

/*	akTextureCoo.clear();
	akFaces.clear();
	akFrames.clear();
	akSubMeshes.clear();
	akAnimation.clear();
	akBoneConnections.clear();*/
}

void Mad_CoreMesh::operator=(const Mad_CoreMesh& kOther)
{
	strcpy(m_acName, kOther.m_acName);
	m_kLodMesh = kOther.m_kLodMesh;

/*	kHead.iNumOfAnimation	= kOther.kHead.iNumOfAnimation;
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
	akBoneConnections = kOther.akBoneConnections;*/
}



void Mad_CoreMesh::ShowInfo(void)
{
	cout << "Mad_CoreMesh::ShowInfo" << endl;

/*	cout << "Textures: " << kHead.iNumOfTextures << endl;
	cout << "Vertex: " << kHead.iNumOfVertex << endl;
	cout << "Triangles: " << kHead.iNumOfFaces << endl;
	cout << "Frames: " << kHead.iNumOfFrames << endl;
	cout << "SubMesh: " << kHead.iNumOfSubMeshes << endl;
	cout << "Animations: " << kHead.iNumOfAnimation << endl;

	for (unsigned int i=0; i<akAnimation.size(); i++) 
	{
		cout << " " << i << " : " << akAnimation[i].Name << " : Frames = " << akAnimation[i].KeyFrame.size() << endl;
	}*/
}


void Mad_CoreMesh::Save(ZFVFile* pkZFile)
{
/*	for(int i=0; i<m_kLodMesh.size(); i++) {
		m_kLodMesh[i].Save(pkZFile);
		}*/
	
	m_kLodMesh[0].Save(pkZFile);

/*	kHead.iNumOfAnimation = akAnimation.size();

	ShowInfo();

	// Write Head.
	pkZFile->Write(&kHead,sizeof(Mad_CoreMeshHeader),1);
	//fwrite(&kHead,sizeof(Mad_CoreMeshHeader),1,fp);

	// Write SubMesh
	pkZFile->Write(&akSubMeshes[0],sizeof(Mad_CoreSubMesh),kHead.iNumOfSubMeshes);
	//fwrite(&akSubMeshes[0],sizeof(Mad_CoreSubMesh),kHead.iNumOfSubMeshes,fp);


	// Write Textures
	pkZFile->Write((void *)&akTextures[0],sizeof(Mad_CoreTexture),kHead.iNumOfTextures);
	//fwrite((void *)&akTextures[0],sizeof(Mad_CoreTexture),kHead.iNumOfTextures,fp);

	// Write Texture Coo
	pkZFile->Write((void *)&akTextureCoo[0],sizeof(Mad_TextureCoo),kHead.iNumOfVertex);
	//fwrite((void *)&akTextureCoo[0],sizeof(Mad_TextureCoo),kHead.iNumOfVertex,fp);

	// Write Bone Vikter
	pkZFile->Write((void *)&this->akBoneConnections[0],sizeof(int),kHead.iNumOfVertex);
	//fwrite((void *)&this->akBoneConnections[0],sizeof(int),kHead.iNumOfVertex,fp);

	// Write Alla vertex Frames.
	int size;
	for(int i=0; i<kHead.iNumOfFrames; i++) {
		size = akFrames[i].akVertex.size();
		pkZFile->Write(&akFrames[i].akVertex[0],sizeof(Vector3),kHead.iNumOfVertex);
		//fwrite(&akFrames[i].akVertex[0],sizeof(Vector3),kHead.iNumOfVertex,fp);
		size = akFrames[i].akNormal.size();
		pkZFile->Write(&akFrames[i].akNormal[0],sizeof(Vector3),kHead.iNumOfVertex);
		//fwrite(&akFrames[i].akNormal[0],sizeof(Vector3),kHead.iNumOfVertex,fp);
		}

	// Write triangles.
	pkZFile->Write(&akFaces[0],sizeof(Mad_Face),kHead.iNumOfFaces);
	//fwrite(&akFaces[0],sizeof(Mad_Face),kHead.iNumOfFaces,fp);

	// Write Animations.
	int iNumOfAnimations = this->akAnimation.size();
	pkZFile->Write(&iNumOfAnimations,sizeof(int), 1 );
	//fwrite(&iNumOfAnimations,sizeof(int), 1 ,fp);

	vector<Mad_CoreMeshAnimation>::iterator		itAnim;
	vector<Mad_CoreKeyFrame>::iterator		itKeyF;

	for(itAnim = akAnimation.begin(); itAnim != akAnimation.end(); itAnim++)
	{
		pkZFile->Write(itAnim->Name,sizeof(char), 64);
		//fwrite(itAnim->Name,sizeof(char), 64 ,fp);
		int iNumOfKeyFrames = itAnim->KeyFrame.size();
		pkZFile->Write(&iNumOfKeyFrames,sizeof(int), 1);
		//fwrite(&iNumOfKeyFrames,sizeof(int), 1 ,fp);
		
		for(itKeyF = itAnim->KeyFrame.begin(); itKeyF != itAnim->KeyFrame.end(); itKeyF++)
		{
			pkZFile->Write(&itKeyF->iVertexFrame,sizeof(int), 1);
			//fwrite(&itKeyF->iVertexFrame,sizeof(int), 1 ,fp);
		}
	}*/
	
}

void Mad_CoreMesh::Load(ZFVFile* pkZFile)
{
	Mad_RawMesh kRMesh;
	m_kLodMesh.push_back(kRMesh);
	m_kLodMesh[0].Load(pkZFile);


/*	int i,j;
	// Read head
	pkZFile->Read(&kHead,sizeof(Mad_CoreMeshHeader),1);
	//fread(&kHead,sizeof(Mad_CoreMeshHeader),1,fp);

	// Read SubMesh
	for(i = 0; i<kHead.iNumOfSubMeshes; i++) {
		Mad_CoreSubMesh	kSubMesh;
		pkZFile->Read(&kSubMesh,sizeof(Mad_CoreSubMesh),1);
		//fread(&kSubMesh,sizeof(Mad_CoreSubMesh),1,fp);
		akSubMeshes.push_back(kSubMesh);

		}

	// Read textures
	for(i = 0; i<kHead.iNumOfTextures; i++) {
		Mad_CoreTexture	kTexture;
		pkZFile->Read(&kTexture,sizeof(Mad_CoreTexture),1);
		//fread(&kTexture,sizeof(Mad_CoreTexture),1,fp);
		akTextures.push_back(kTexture);
		}
//	fread((void *)akTextures,sizeof(Mad_Texture),kHead.iNumOfTextures,fp);

	// Read Texture Coo
	for(i = 0; i<kHead.iNumOfVertex; i++) {
		Mad_TextureCoo	kTexCoo;
		pkZFile->Read(&kTexCoo,sizeof(Mad_TextureCoo),1);
		//fread(&kTexCoo,sizeof(Mad_TextureCoo),1,fp);
		akTextureCoo.push_back(kTexCoo);
		}

	// Read Bone Vikter
	for(i = 0; i<kHead.iNumOfVertex; i++) {
		int iBoneVikt;
		pkZFile->Read(&iBoneVikt,sizeof(int),1);
		//fread(&iBoneVikt,sizeof(int),1,fp);
		akBoneConnections.push_back(iBoneVikt);
		}


	// Read Alla vertex Frames.
	Vector3* pkVector = new Vector3 [kHead.iNumOfVertex];
	for(i = 0; i<kHead.iNumOfFrames; i++) {
		Mad_CoreVertexFrame	kVertexFrame;

		pkZFile->Read(pkVector,sizeof(Vector3),kHead.iNumOfVertex);
		//fread(pkVector,sizeof(Vector3),kHead.iNumOfVertex,fp);
		for(j=0; j<kHead.iNumOfVertex; j++)
			kVertexFrame.akVertex.push_back(pkVector[j]);

		pkZFile->Read(pkVector,sizeof(Vector3),kHead.iNumOfVertex);
		//fread(pkVector,sizeof(Vector3),kHead.iNumOfVertex,fp);
		for(j=0; j<kHead.iNumOfVertex; j++)
			kVertexFrame.akNormal.push_back(pkVector[j]);

		akFrames.push_back(kVertexFrame);
	}

	delete [] pkVector;

	// Read triangles.
	for(i = 0; i<kHead.iNumOfFaces; i++) {
		Mad_Face	kFace;
		pkZFile->Read(&kFace, sizeof(Mad_Face),1);
		//fread(&kFace, sizeof(Mad_Face),1,fp);
		akFaces.push_back(kFace);

		}

	// Read Animations.
	int iNumOfAnimations;
	pkZFile->Read(&iNumOfAnimations,sizeof(int), 1);
	//fread(&iNumOfAnimations,sizeof(int), 1 ,fp);

	Mad_CoreMeshAnimation kNyAnim;
	Mad_CoreKeyFrame kNyKey;

	for(int iA = 0; iA < iNumOfAnimations; iA++)
	{
		kNyAnim.Clear();
		pkZFile->Read(kNyAnim.Name,sizeof(char), 64);
		//fread(kNyAnim.Name,sizeof(char), 64 ,fp);

		int iNumOfKeyFrames;
		pkZFile->Read(&iNumOfKeyFrames,sizeof(int), 1);
		//fread(&iNumOfKeyFrames,sizeof(int), 1 ,fp);
	
		for(int iK = 0; iK < iNumOfKeyFrames; iK++ )
		{
			kNyKey.Clear();
			pkZFile->Read(&kNyKey.iVertexFrame,sizeof(int), 1);
			//fread(&kNyKey.iVertexFrame,sizeof(int), 1 ,fp);
			kNyAnim.KeyFrame.push_back(kNyKey);
		}

		akAnimation.push_back(kNyAnim);
	}

/*	// Load Textures
	char nisse[256];
	for(i = 0; i< kHead.iNumOfTextures; i++) {
		sprintf(nisse, "../data/textures/%s.bmp", akTextures[i].ucTextureName);
//		cout << "Should Load: " << nisse << endl;
//		aiTextureIndex[i] = pkTextureManger->Load(nisse,0);
	}
*/
}

























int Mad_CoreMesh::GetSizeInBytes()
{
	cout << "Mad_CoreMesh::GetSizeInBytes - NOT DONE" << endl;
	return 0;

/*	int iSizeInBytes = 0;
	iSizeInBytes += sizeof(Mad_CoreMesh);

	iSizeInBytes += sizeof(Mad_CoreTexture) * akTextures.size();
	iSizeInBytes += sizeof(Mad_TextureCoo)  * akTextureCoo.size();
	iSizeInBytes += sizeof(Mad_Face)			 * akFaces.size();
	
	for(unsigned int iVf = 0; iVf < akFrames.size(); iVf++)
		iSizeInBytes += akFrames[iVf].GetSizeInBytes();

	iSizeInBytes += sizeof(Mad_CoreSubMesh) * akSubMeshes.size();

	for(unsigned int iMa = 0; iMa < akAnimation.size(); iMa++)
		iSizeInBytes += akAnimation[iMa].GetSizeInBytes();

	iSizeInBytes += sizeof(int) * akBoneConnections.size();

	return iSizeInBytes;*/
}

