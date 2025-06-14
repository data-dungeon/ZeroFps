#include <sstream>
#include "mad_core.h"

using namespace std;

//bool	g_iMadLOD;
ConVar g_kMadLOD("r_madlod","1");

Mad_RawMesh::Mad_RawMesh()
{
	Clear();
}

string Mad_RawMesh::ShowInfo(void)
{
	ostringstream	kOut;

	kOut << "Textures: " << kHead.iNumOfTextures << endl;
	kOut << "Vertex: " << kHead.iNumOfVertex << endl;
	kOut << "Triangles: " << kHead.iNumOfFaces << endl;
	kOut << "Frames: " << kHead.iNumOfFrames << endl;
	kOut << "SubMesh: " << kHead.iNumOfSubMeshes << endl;
	kOut << "Animations: " << kHead.iNumOfAnimation << endl;
	kOut << "m_fMaxDistance: " << m_fMaxDistance << endl;

	for (unsigned int i=0; i<akTextures.size(); i++) 
	{
		kOut << " Texture[" << i << "]: " << akTextures[i].ucTextureName << endl;
	}

	for (unsigned int i=0; i<akSubMeshes.size(); i++) 
	{
		kOut << " Submesh[" << i << "]: " << akSubMeshes[i].iFirstTriangle << " , " << akSubMeshes[i].iNumOfTriangles << "," << akSubMeshes[i].iTextureIndex << endl; 
	}

	for (unsigned int i=0; i<akAnimation.size(); i++) 
	{
		kOut << " " << i << " : " << akAnimation[i].Name << " : Frames = " << akAnimation[i].KeyFrame.size() << endl;
	}

	return kOut.str();
}

void Mad_RawMesh::Save(int iVersion, ZFVFile* pkZFile)
{
	kHead.iNumOfAnimation = akAnimation.size();

//	ShowInfo();

	if(iVersion >= 3)
		pkZFile->Write(&m_fMaxDistance,sizeof(float),1);
	

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

void Mad_RawMesh::Load(int iVersion,ZFVFile* pkZFile)
{
	int i,j;

	if(iVersion >= 3)
		pkZFile->Read(&m_fMaxDistance,sizeof(float),1);

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

	OptimizeSubMeshes();
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

	m_fMaxDistance = 250;

//	iDisplayID = -1;

	akTextureCoo.clear();
	akFaces.clear();
	akFrames.clear();
	akSubMeshes.clear();
	akAnimation.clear();
	akBoneConnections.clear();
}

Mad_RawMesh::Mad_RawMesh(const Mad_RawMesh& kOther)
{
	Clear();
	*this = kOther;
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

	akTextures			= kOther.akTextures; 
	akTextureCoo		= kOther.akTextureCoo;
	akFaces				= kOther.akFaces;
	akFrames				= kOther.akFrames;
	akSubMeshes			= kOther.akSubMeshes;
	akAnimation			= kOther.akAnimation;
	akBoneConnections = kOther.akBoneConnections;

	m_fMaxDistance		= kOther.m_fMaxDistance;
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
	iSizeInBytes += sizeof(Mad_RawMesh);

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

bool Mad_RawMesh::SetTextureHandle(int iTextureIndex, string strName)
{
	return akTexturesHandles[iTextureIndex].SetRes(strName);
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

	// Finns ingen animation med det namnet s� skapa den och returnera den.
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
	// Verify we need to optimize.
	if(akTextures.size() == akSubMeshes.size())	
		return;


	vector<Mad_TextureCoo>			akNewTextureCoo;
	vector<Mad_Face>					akNewFaces;
	vector<Mad_CoreVertexFrame>	akNewFrames;	
	vector<Mad_CoreSubMesh>			akNewSubMeshes;
	vector<int>							akNewBoneConnections;	
	Mad_CoreVertexFrame				kVertexFrame;
	//int iVIndex;
	Mad_CoreSubMesh					kSubMesh;

//    cout << ShowInfo();
// 	cout << "*** SUBMESH OPTIMIZE ***" << endl;

	for(int iTexture=0; iTexture<akTextures.size(); iTexture++)
	{
		for(int iSubMesh=0; iSubMesh<akSubMeshes.size(); iSubMesh++)
		{
			if(akSubMeshes[iSubMesh].iTextureIndex == iTexture)
			{
				if(akNewSubMeshes.size() == 0)
				{
					kSubMesh.iFirstTriangle = 0;
					kSubMesh.iNumOfTriangles = akSubMeshes[iSubMesh].iNumOfTriangles;
					kSubMesh.iTextureIndex = iTexture;
					akNewSubMeshes.push_back(kSubMesh);
				}
				else
				{
					// If this is the same as last submesh add to it else create a new one.
					if(akNewSubMeshes.back().iTextureIndex == iTexture)
					{
						akNewSubMeshes.back().iNumOfTriangles += akSubMeshes[iSubMesh].iNumOfTriangles;
					}
					else
					{
						kSubMesh.iFirstTriangle = akNewFaces.size();
						kSubMesh.iNumOfTriangles = akSubMeshes[iSubMesh].iNumOfTriangles;
						kSubMesh.iTextureIndex = iTexture;
						akNewSubMeshes.push_back(kSubMesh);
					}
				}

				for(int iFace=akSubMeshes[iSubMesh].iFirstTriangle; iFace<(akSubMeshes[iSubMesh].iFirstTriangle+akSubMeshes[iSubMesh].iNumOfTriangles);iFace++)
				{
					Mad_Face kNewFace;
               
					for(int i=0; i<3; i++)
					{
						int iNewVIndex = akNewTextureCoo.size();
						int iOldVIndex = akFaces[iFace].iIndex[i];
						kNewFace.iIndex[i] = iNewVIndex;

						akNewTextureCoo.push_back( akTextureCoo[ iOldVIndex ] );
						akNewBoneConnections.push_back( akBoneConnections[ iOldVIndex ] );
						kVertexFrame.PushBack(akFrames[0].akVertex[iOldVIndex],akFrames[0].akNormal[iOldVIndex] );
					}

					akNewFaces.push_back( kNewFace );
				}
			}
		}
	}
	
	akTextureCoo = akNewTextureCoo;
	akFaces = akNewFaces;
	akFrames.clear();
	akFrames.push_back(kVertexFrame);
	akBoneConnections = akNewBoneConnections;
	akSubMeshes = akNewSubMeshes;
	kHead.iNumOfSubMeshes = akSubMeshes.size();

// 	cout << ShowInfo();
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








Mad_CoreMesh::Mad_CoreMesh()
{
	Clear();
}

Mad_CoreMesh::~Mad_CoreMesh()
{

}

Mad_CoreMesh::Mad_CoreMesh(const Mad_CoreMesh& kOther)
{
	Clear();
	*this = kOther;
}

void Mad_CoreMesh::operator=(const Mad_CoreMesh& kOther)
{
	m_iMadVersion = kOther.m_iMadVersion;
	strcpy(m_acName, kOther.m_acName);
	m_kLodMesh = kOther.m_kLodMesh;
   bNotAnimated = kOther.bNotAnimated;
}

void Mad_CoreMesh::Clear(void)
{
	m_iMadVersion = -1;
	strcpy(m_acName, "");

	bNotAnimated = false;
	m_kLodMesh.clear();
}

Mad_RawMesh* Mad_CoreMesh::GetLODMesh(int iId)
{
	if(iId >= m_kLodMesh.size())
		return &m_kLodMesh[m_kLodMesh.size()-1];

	return &m_kLodMesh[iId];
}

Mad_RawMesh* Mad_CoreMesh::GetLODRender(float fDistance)
{
	if(!g_kMadLOD.GetBool())
		return &m_kLodMesh[0];

	Mad_RawMesh* pkRawMesh = NULL;

	for(int i=0; i<m_kLodMesh.size(); i++)
	{
		//cout << "Lod distance [" << i << "]" << m_kLodMesh[i].m_fMaxDistance << endl;
		if(fDistance > m_kLodMesh[i].m_fMaxDistance)
			continue;

		return &m_kLodMesh[i];
	}

	//return max lodlevel
	return &m_kLodMesh[m_kLodMesh.size()-1];

// 	return NULL;
}

int Mad_CoreMesh::GetLODRenderIndex(float fDistance)
{
	if(!g_kMadLOD.GetBool())
		return 0;

	Mad_RawMesh* pkRawMesh = NULL;

	for(int i=0; i<m_kLodMesh.size(); i++)
	{
		//cout << "Lod distance [" << i << "]" << m_kLodMesh[i].m_fMaxDistance << endl;
		if(fDistance > m_kLodMesh[i].m_fMaxDistance)
			continue;

		return i;
	}

	//return max lodlevel
	return m_kLodMesh.size()-1;
//	return -1;
}

void Mad_CoreMesh::Save(ZFVFile* pkZFile)
{
	if(m_iMadVersion >= 2)
	{
		pkZFile->Write(m_acName,MAD_MAX_NAME,1);
	}
	if(m_iMadVersion >= 3)
	{
		int iNumRawMesh = m_kLodMesh.size();
      pkZFile->Write(&iNumRawMesh,sizeof(int),1);
		for(int i=0; i<m_kLodMesh.size(); i++) 
		{
			m_kLodMesh[i].Save(m_iMadVersion, pkZFile);
		}
	}
	else
	{
		m_kLodMesh[0].Save(m_iMadVersion, pkZFile);
	}
}

void Mad_CoreMesh::Load(ZFVFile* pkZFile)
{
	if(m_iMadVersion >= 2)
	{
		pkZFile->Read(m_acName, MAD_MAX_NAME, 1);
	}
	//cout << "Reading MAD:Mesh: " << m_acName << endl;

	if(m_iMadVersion >= 3)
	{
		int iNumRawMesh;
      pkZFile->Read(&iNumRawMesh,sizeof(int),1);
		for(int i=0; i<iNumRawMesh; i++) 
		{
			Mad_RawMesh kRMesh;
			kRMesh.Clear();
			kRMesh.Load(m_iMadVersion, pkZFile);
			m_kLodMesh.push_back(kRMesh);
		}
	}
	else
	{
		Mad_RawMesh kRMesh;
		m_kLodMesh.push_back(kRMesh);
		m_kLodMesh[0].Load(m_iMadVersion, pkZFile);
	}
}

void Mad_CoreMesh::ShowInfo(void)
{
	cout << "Mad_CoreMesh::ShowInfo" << endl;
	cout << "Num Of RawMeshes: " << m_kLodMesh.size() << endl;
	for(int i=0; i<m_kLodMesh.size(); i++)
		cout << m_kLodMesh[i].ShowInfo();
}

int Mad_CoreMesh::GetSizeInBytes()
{
	int iBytes = sizeof( Mad_CoreMesh );
	for(int i=0; i<m_kLodMesh.size(); i++)
		iBytes += m_kLodMesh[i].GetSizeInBytes();

	return iBytes;
}

