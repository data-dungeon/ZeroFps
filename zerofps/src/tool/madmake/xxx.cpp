#include <iostream>
#include "xxx.h"
#include "script.h"


int	ModellXXX::AddTexture(char* ucpTextureName)
{
	vector<Mad_Texture>::iterator itTexture;
	int iTextureIndex = 0;

	char* ext = strstr(ucpTextureName, ".");
	ext[0] = 0;


	for(itTexture = m_akTextureNames.begin(); itTexture != m_akTextureNames.end(); itTexture++)
	{
		if(strcmp(itTexture->ucTextureName, ucpTextureName) == 0)
			return iTextureIndex; 

		iTextureIndex++;
	}

	Mad_Texture madtex;
	strcpy(madtex.ucTextureName, ucpTextureName);
	m_akTextureNames.push_back(madtex);

	return iTextureIndex; 
}

void ModellXXX::ReadAnimationFrame(FILE* fp, int iNumTriangles)
{
	MadVertex	kVertex;
	MadFace		kFace;

	Mad_VertexFrame kFrame;
	MadTextureCoo	kTextureCoo;

	for(int i=0; i< iNumTriangles; i++)
	{
		for(int v = 0; v < 3; v++)
		{
			fscanf(fp, " <%f,%f,%f,%f,%f>",&kVertex.x,&kVertex.y,&kVertex.z,&kTextureCoo.s,&kTextureCoo.t);
			kFace.iIndex[v] = kFrame.akVertex.size();
			kFrame.akVertex.push_back(kVertex);
		}
	}

	m_akFrames.push_back(kFrame);
}

void ModellXXX::ReadVertexFrame(FILE* fp, int iNumTriangles)
{
	MadVertex	kVertex;
	MadFace		kFace;

	Mad_VertexFrame kFrame;
	MadTextureCoo	kTextureCoo;

	bool bMakeTextureCoo = true;
	bool bMakeTriangles = true;
	if(m_akTextureCoo.size() > 0)
		bMakeTextureCoo = false;
	if(m_akFace.size() > 0)
		bMakeTriangles = false;


	char TextureName[256];
	int iTexture;

	for(int i=0; i< iNumTriangles; i++)
	{
		fscanf(fp, "%s", TextureName);
		iTexture = AddTexture(TextureName);

		for(int v = 0; v < 3; v++)
		{
			fscanf(fp, " <%f,%f,%f,%f,%f>",&kVertex.x,&kVertex.y,&kVertex.z,&kTextureCoo.s,&kTextureCoo.t);
			kFace.iIndex[v] = kFrame.akVertex.size();
			kFrame.akVertex.push_back(kVertex);
			if(bMakeTextureCoo)
				m_akTextureCoo.push_back(kTextureCoo);
		}

		if(bMakeTriangles) {
			m_akFace.push_back(kFace);
			
			// Skapa en submesh för varje polygon
			Mad_SubMesh newsub;
			newsub.iFirstTriangle = i;
			newsub.iNumOfTriangles = 1;
			newsub.iTextureIndex = iTexture;
			m_akSubMesh.push_back(newsub);
			int smsize = m_akSubMesh.size();
			}
	}

	m_akFrames.push_back(kFrame);
}

void ModellXXX::ReadBaseFrame(char* filename)
{
	cout << "Base: " << filename << " ";

	char tmpstr[256];

	FILE* fp = fopen(filename,"rt");

	int iFrame = -1;
	int iNumTris = -1;

	bool done = false;

	while(!done)
	{
		fscanf(fp, "%s",tmpstr);
		if(strcmp(tmpstr, "Frame") == 0) {
			fscanf(fp, "%s",tmpstr);
			iFrame = atoi(tmpstr);

			fscanf(fp, "%s",tmpstr);
			fscanf(fp, "%s",tmpstr);
			iNumTris = atoi(tmpstr);

			ReadVertexFrame(fp, iNumTris);
			cout << ".";
		}
		else 
			done = true;
	}

	cout << endl;

	fclose(fp);
	m_akFrames.clear();	
}

void ModellXXX::ReadAnimation(char* filename)
{
	cout << "Base: " << filename << " ";

	char tmpstr[256];

	FILE* fp = fopen(filename,"rt");

	int iFrame = -1;
	int iNumTris = -1;

	int iStartFrame = m_akFrames.size();
	int iNumOfFrames = 0;

	bool done = false;
	
	while(!done)
	{
		fscanf(fp, "%s",tmpstr);
		if(strcmp(tmpstr, "Frame") == 0) {
			fscanf(fp, "%s",tmpstr);
			iFrame = atoi(tmpstr);

			fscanf(fp, "%s",tmpstr);
			fscanf(fp, "%s",tmpstr);
			iNumTris = atoi(tmpstr);

			ReadAnimationFrame(fp, iNumTris);
			iNumOfFrames++;
			cout << ".";
		}
		else 
			done = true;
	}

	cout << endl;

	Mad_Animation newanim;
	Mad_KeyFrame newkey;
	newanim.Clear();
	strcpy(newanim.Name, filename);
	for(int i=0; i<iNumOfFrames; i++)
	{
		newkey.Clear();
		newkey.iVertexFrame = iStartFrame + i; 
		newanim.KeyFrame.push_back(newkey);
	}

	akAnimation.push_back(newanim);

	fclose(fp);
}

void ModellXXX::Read( char* filename )
{
	ScriptFile kMMScipt;
	kMMScipt.LoadScript(filename);

	char* ucpToken;
	ucpToken = kMMScipt.GetToken();

	while(ucpToken)
	{
		if (!strcmp (ucpToken, "!filetype"))
			cout << "Command filetype" << endl;
		
		if (!strcmp (ucpToken, "!add-md"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Command add-md: " << ucpToken << endl;
			ReadBaseFrame(ucpToken);
		}

		if (!strcmp (ucpToken, "!add-meshanim"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Command add-meshanim: " << ucpToken << endl;
			ReadAnimation(ucpToken);
		}


		ucpToken = kMMScipt.GetToken();
	}
}

bool ModellXXX::Export(MadExporter* mad)
{
	int iTotalNumOfVertex = m_akFrames[0].akVertex.size();
	int iTotalTriangles = m_akFace.size();
	int iAntalFrames = m_akFrames.size();

	int i;

	mad->kHead.iVersionNum		= 1;
	mad->kHead.iNumOfTextures	= m_akTextureNames.size();
	mad->kHead.iNumOfVertex		= iTotalNumOfVertex;
	mad->kHead.iNumOfFaces		= iTotalTriangles;
	mad->kHead.iNumOfFrames		= iAntalFrames;
	mad->kHead.iNumOfSubMeshes	= m_akSubMesh.size();
	mad->kHead.iNumOfAnimation	= akAnimation.size();

	for( i=0; i < m_akTextureNames.size(); i++) 
	{
		cout << "Texture " << i << ": " << m_akTextureNames[i].ucTextureName << endl;
		strcpy(mad->akTextures[i].ucTextureName, m_akTextureNames[i].ucTextureName);
	}

	mad->akFrames.resize(mad->kHead.iNumOfFrames);
	mad->akFaces.resize(mad->kHead.iNumOfFaces);
	mad->akSubMeshes.resize(mad->kHead.iNumOfSubMeshes);
	
	// Copy Submeshes.
	mad->akSubMeshes = m_akSubMesh;

	// Copy Faces
	for(i=0; i<mad->kHead.iNumOfFaces; i++) 
	{
		mad->akFaces[i].iIndex[0] = m_akFace[i].iIndex[0];
		mad->akFaces[i].iIndex[1] = m_akFace[i].iIndex[1];
		mad->akFaces[i].iIndex[2] = m_akFace[i].iIndex[2];
	}

	mad->akFrames		= m_akFrames;
	mad->akTextureCoo	= m_akTextureCoo;
	mad->akAnimation	= akAnimation;


	return true;
}

