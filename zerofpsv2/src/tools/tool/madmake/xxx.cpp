#include <iostream>
#include "xxx.h"
#include "../../../zerofpsv2/basic/simplescript.h"

float g_fExportScale;	
float g_fUnitsMeter;	// Num of units to be one meter in real scale.
float g_fTotalScale;	// 
extern string ucaOutFile;

bool IsATextureName(char* szName)
{
	char szTexName[256];
	strcpy(szTexName, szName);
	Gemens(szTexName);

	if(strstr(szTexName, ".bmp") || strstr(szTexName, ".tga"))
		return true;

	return false;

}


/*int	ModellXXX::AddTexture(char* ucpTextureName)
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
}*/
















void ModellXXX::ReadAnimationFrame(FILE* fp, int iNumTriangles)
{
/*	Vector3		kVertex;
	Vector3		kNormal;
	MadFace		kFace;

	Mad_VertexFrame kFrame;
	MadTextureCoo	kTextureCoo;

	for(int i=0; i< iNumTriangles; i++)
	{
		for(int v = 0; v < 3; v++)
		{
			fscanf(fp, " <%f,%f,%f,%f,%f,%f,%f,%f>",&kVertex.x,&kVertex.y,&kVertex.z, 
				&kNormal.x,&kNormal.y,&kNormal.z,
				&kTextureCoo.s,&kTextureCoo.t);
			kFace.iIndex[v] = kFrame.akVertex.size();
			kFrame.akVertex.push_back(kVertex);
			kFrame.akNormal.push_back(kNormal);
		}
	}

	m_akFrames.push_back(kFrame);*/
}

void ModellXXX::ReadVertexFrame(FILE* fp, int iNumTriangles)
{
/*	Vector3		kVertex;
	Vector3		kNormal;
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

	float diff_s, diff_t;

	for(int i=0; i< iNumTriangles; i++)
	{
		fscanf(fp, "%s", TextureName);
		iTexture = AddTexture(TextureName);

		for(int v = 0; v < 3; v++)
		{
			fscanf(fp, " <%f,%f,%f,%f,%f,%f,%f,%f>",&kVertex.x,&kVertex.y,&kVertex.z,
				&kNormal.x,&kNormal.y,&kNormal.z,
				&kTextureCoo.s,&kTextureCoo.t);
			kFace.iIndex[v] = kFrame.akVertex.size();
			kFrame.akVertex.push_back(kVertex);

			if(bMakeTextureCoo)
				m_akTextureCoo.push_back(kTextureCoo);
			else {
				diff_s = m_akTextureCoo[(i*3) + v].s - kTextureCoo.s;
				diff_t = m_akTextureCoo[(i*3) + v].t - kTextureCoo.t;
				if(diff_s != 0.0 || diff_t != 0.0 ) {
					cout << "Err: " << i << ", " << v << "  ";
					cout << "<" << diff_s << ","<< diff_t << ">" << endl;
					}
				}
		}

		if(bMakeTriangles) {
			m_akFace.push_back(kFace);
			
			// Skapa en submesh för varje polygon
			Mad_CoreSubMesh newsub;
			newsub.iFirstTriangle = i;
			newsub.iNumOfTriangles = 1;
			newsub.iTextureIndex = iTexture;
			m_akSubMesh.push_back(newsub);
			int smsize = m_akSubMesh.size();
			}
	}

	m_akFrames.push_back(kFrame);*/
}

void ModellXXX::ReadBaseFrame(const char* filename)
{
/*	cout << "Base: " << filename << " ";

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

			cout << "Frame: " << iFrame << endl;
			ReadVertexFrame(fp, iNumTris);
		}
		else 
			done = true;
	}

	cout << endl;

	fclose(fp);
	m_akFrames.clear();	*/
}

char ErrorMsg[512];

void ModellXXX::ReadCoreMesh(const char* filename, const char* szName)
{
	FILE* fp = fopen(filename, "rt");
	if(fp == NULL) {
		sprintf(ErrorMsg, "Failed to find file %s.\n",filename);
		Error(ErrorMsg);
		}

	Vector3					kVertex;
	Vector3					kNormal;
	Mad_Face				kFace;
	Mad_CoreVertexFrame		kFrame;
	Mad_TextureCoo			kTextureCoo;

	Mad_CoreMesh* pkMesh = GetMesh(szName);


	bool bMakeTextureCoo = true;
	bool bMakeTriangles = true;
	if(pkMesh->SizeTexturesCoo() > 0)
		bMakeTextureCoo = false;
	if(pkMesh->SizeFaces() > 0)
		bMakeTriangles = false;

	char TextureName[256];
	int iTexture;

	float diff_s, diff_t;

	char tmpstr[256];
	int iFrame = -1;
	int iNumTris = -1;

	bool done = false;

	fscanf(fp, "%s",tmpstr);
	fscanf(fp, "%s",tmpstr);
	iNumTris = atoi(tmpstr);
	int iBoneLink;

	for(int i=0; i < iNumTris; i++)
	{
		fscanf(fp, "%s", TextureName);
		if(! IsATextureName(TextureName)) {
			sprintf(ErrorMsg, "One (or more) triangles have no texture in file %s",filename);
			Error(ErrorMsg);
			}

		iTexture = pkMesh->AddTexture(TextureName);

		for(int v = 0; v < 3; v++)
		{
			fscanf(fp, " <%d,%f,%f,%f,%f,%f,%f,%f,%f>",&iBoneLink, &kVertex.x,&kVertex.y,&kVertex.z,
				&kNormal.x,&kNormal.y,&kNormal.z,
				&kTextureCoo.s,&kTextureCoo.t);
			kVertex *= g_fTotalScale;
			kFace.iIndex[v] = kFrame.Size();
			kFrame.PushBack(kVertex, kNormal);
			//kFrame.akVertex.push_back(kVertex);
			//kFrame.akNormal.push_back(kNormal);			

			
//			pkMesh->akBoneConnections.push_back(iBoneLink);
			pkMesh->PushBackBoneConnection(iBoneLink);
			
			if(bMakeTextureCoo)
//				pkMesh->akTextureCoo.push_back(kTextureCoo);
				pkMesh->PushBackTextureCoo(kTextureCoo);
			else {
				/*diff_s = pkMesh->akTextureCoo[(i*3) + v].s - kTextureCoo.s;
				diff_t = pkMesh->akTextureCoo[(i*3) + v].t - kTextureCoo.t;
				if(diff_s != 0.0 || diff_t != 0.0 ) {
					cout << "Err: " << i << ", " << v << "  ";
					cout << "<" << diff_s << ","<< diff_t << ">" << endl;
					}*/
				}
		}

		if(bMakeTriangles) {
			//pkMesh->akFaces.push_back(kFace);
			pkMesh->PushBackFaces(kFace);
			
			// Skapa en submesh för varje polygon
			Mad_CoreSubMesh newsub;
			newsub.iFirstTriangle = i;
			newsub.iNumOfTriangles = 1;
			newsub.iTextureIndex = iTexture;
			//pkMesh->akSubMeshes.push_back(newsub);
			pkMesh->PushBackSubMeshes(newsub);
			//int smsize = pkMesh->akSubMeshes.size();
			}
	}

//	pkMesh->akFrames.push_back(kFrame);
	pkMesh->SetTextureFlags();
	pkMesh->PushBackFrames(kFrame);

	cout << "Done Reading Mesh: " <<	pkMesh->m_acName << endl;



	fclose(fp);
//	m_akFrames.clear();	
}



void ModellXXX::ReadAnimation(const char* filename)
{
/*	cout << "Base: " << filename << " ";

	char tmpstr[256];

	FILE* fp = fopen(filename,"rt");

	int iFrame		= -1;
	int iNumTris	= -1;

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

	Mad_CoreMeshAnimation newanim;
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

	fclose(fp);*/
}

void ModellXXX::ReadExportSD(const char* filename)
{
	FILE* fp = fopen(filename, "rt");
	if(fp == NULL) {
		sprintf(ErrorMsg, "Failed to find file %s.\n",filename);
		Error(ErrorMsg);
		}

	char tmpstr[256];
	bool done = false;

//	FILE* fp = fopen(filename,"rt");

//	vector<Mad_CoreBone>	Bones;
	Mad_CoreBone NewBone;
	int res;
	int iBoneId;

	while(!done) {
		res = fscanf(fp, "%s",tmpstr);
		iBoneId = atoi(tmpstr);

		if(res ==  EOF)
			break;

		fscanf(fp, "%s",NewBone.m_acName);
		fscanf(fp, "%s",tmpstr);
		NewBone.m_iParent = atoi(tmpstr);

		fscanf(fp, "%s",tmpstr);
		NewBone.m_kPosition.x = atof(tmpstr);
		fscanf(fp, "%s",tmpstr);
		NewBone.m_kPosition.y = atof(tmpstr);
		fscanf(fp, "%s",tmpstr);
		NewBone.m_kPosition.z = atof(tmpstr);

		fscanf(fp, "%s",tmpstr);
		NewBone.m_kRotation.x = atof(tmpstr);
		fscanf(fp, "%s",tmpstr);
		NewBone.m_kRotation.y = atof(tmpstr);
		fscanf(fp, "%s",tmpstr);
		NewBone.m_kRotation.z = atof(tmpstr);

		// Scale
		NewBone.m_kPosition *= g_fTotalScale;
//		NewBone.m_kRotation *= g_fTotalScale;

		m_akSkelleton.push_back(NewBone);
	}

	fclose(fp);


	// Write Down SD
/*	fp = fopen("test.sd","wb");
	int iNumOfBones = Bones.size();

	fwrite(&iNumOfBones,1,sizeof(int),fp);
	for(int i=0; i<Bones.size(); i++) {
		fwrite(&Bones[i],1,sizeof(Mad_CoreBone),fp);
		}

	fclose(fp);*/


}

/*
class Mad_BoneKeyFrame
{
public:
	Vector3		m_kPosition;
	Vector3		m_kRotation;
};

class Mad_CoreTrack
{
public:
	int							m_iBoneID;
	vector<Mad_BoneKeyFrame>	m_kKeyFrames;
};

class Mad_CoreAnimation
{
public:
	vector<Mad_CoreTrack>	m_kTracks;
	Mad_CoreTrack* GetTrackForBone(int iBoneId);
	void PrintAnimation(void);
};

Mad_CoreTrack* Mad_CoreAnimation::GetTrackForBone(int iBoneId) {
	for(int i=0; i<m_kTracks.size(); i++) {
		if(m_kTracks[i].m_iBoneID == iBoneId)
			return &m_kTracks[i];
		}

	// Bone not found. Create it and add it.
	Mad_CoreTrack kNewTrack;
	kNewTrack.m_iBoneID = iBoneId;
	m_kTracks.push_back(kNewTrack);
	return &m_kTracks[m_kTracks.size() - 1];
}

void Mad_CoreAnimation::PrintAnimation(void)
{
	cout << "Animation has " << m_kTracks.size() << "tracks." << endl;

	for(int a=0; a < m_kTracks.size(); a++) {
		cout << "Track for bone " << m_kTracks[a].m_iBoneID << " has " << m_kTracks[a].m_kKeyFrames.size() << " frames. " << endl;
		
		}
}
*/

void ModellXXX::ReadExportAD(const char* filename,	const char* szName)
{
	FILE* fp = fopen(filename, "rt");
	if(fp == NULL) {
		sprintf(ErrorMsg, "Failed to find file %s.\n",filename);
		Error(ErrorMsg);
		}

	char tmpstr[256];
	bool done = false;

	int res;
	int iBoneId;
	int iFrameNum;
	iFrameNum = -1;

	Mad_CoreBoneKey			kNewBoneKey;
	Mad_CoreBoneKeyFrame	kNewBoneKeyFrame;
	Mad_CoreBoneAnimation	kNewBoneAnim;

	kNewBoneKey.Clear();
	kNewBoneKeyFrame.Clear();
	kNewBoneAnim.Clear();

	while(!done) {
		res = fscanf(fp, "%s",tmpstr);
		if(res ==  EOF)
			break;
		
		if(strcmp(tmpstr, "Frame") == 0) {
			if(iFrameNum != -1) {
//				NewBoneAnim.m_kBoneKeyFrames.push_back(kNewBoneKeyFrame);
				kNewBoneAnim.PushBack(kNewBoneKeyFrame);
				kNewBoneKeyFrame.Clear();
				}

			res = fscanf(fp, "%s",tmpstr);
			iFrameNum = atoi(tmpstr);
			res = fscanf(fp, "%s",tmpstr);
			}

		iBoneId = atoi(tmpstr);
	
		fscanf(fp, "%s",tmpstr);
		kNewBoneKey.m_kPosition.x = atof(tmpstr);
		fscanf(fp, "%s",tmpstr);
		kNewBoneKey.m_kPosition.y = atof(tmpstr);
		fscanf(fp, "%s",tmpstr);
		kNewBoneKey.m_kPosition.z = atof(tmpstr);

		fscanf(fp, "%s",tmpstr);
		kNewBoneKey.m_kRotation.x = atof(tmpstr);
		fscanf(fp, "%s",tmpstr);
		kNewBoneKey.m_kRotation.y = atof(tmpstr);
		fscanf(fp, "%s",tmpstr);
		kNewBoneKey.m_kRotation.z = atof(tmpstr);

		kNewBoneKey.m_kPosition *= g_fTotalScale;
//		kNewBoneKey.m_kRotation *= g_fTotalScale;

//		kNewBoneKeyFrame.m_kBonePose.push_back(kNewBoneKey);
		kNewBoneKeyFrame.PushBack(kNewBoneKey);
//		kNewTrack = kNewAnimation.GetTrackForBone(iBoneId);
//		kNewTrack->m_kKeyFrames.push_back(kNewBoneKey);
	}

	fclose(fp);

	kNewBoneAnim.PushBack(kNewBoneKeyFrame);

	strcpy(kNewBoneAnim.m_szName, szName);
	cout << "Anim Size: " << kNewBoneAnim.Size();
	m_kBoneAnim.push_back(kNewBoneAnim);

/*	kNewAnimation.PrintAnimation();

	// Write Down SD
	fp = fopen("test.ad","wb");
	int iNumOfTracks = kNewAnimation.m_kTracks.size();
	int iNumOfKeyFrames = kNewAnimation.m_kTracks[0].m_kKeyFrames.size();

	fwrite(&iNumOfTracks,1,sizeof(int),fp);
	fwrite(&iNumOfTracks,1,sizeof(int),fp);
	for(int i=0; i<kNewAnimation.m_kTracks.size(); i++) {
		fwrite(&kNewAnimation.m_kTracks[i].m_iBoneID, 1, sizeof(int), fp);
		for(int t=0; t<iNumOfKeyFrames; t++) 
			fwrite(&kNewAnimation.m_kTracks[i].m_kKeyFrames[t].m_kPosition,1,sizeof(Vector3),fp);
			fwrite(&kNewAnimation.m_kTracks[i].m_kKeyFrames[t].m_kRotation,1,sizeof(Vector3),fp);
		}

	fclose(fp);*/

}



void ModellXXX::Read( const char* filename )
{
	g_fExportScale	= 1.0;
	g_fUnitsMeter	= 100;	 
	g_fTotalScale	= (1.0 / g_fUnitsMeter) * g_fExportScale;

	LogIt("g_fTotalScale = %f\n", g_fTotalScale);

	SimpleScriptFile kMMScipt;
	if(kMMScipt.LoadScript(filename) == false) {
		sprintf(ErrorMsg, "Failed to load script file %s.\n",filename);
		Error(ErrorMsg);
		}

	char* ucpToken;
	ucpToken = kMMScipt.GetToken();
	string strFileName;
	string strName;

	while(ucpToken)
	{
		if (!strcmp (ucpToken, "!scale"))
		{
			ucpToken = kMMScipt.GetToken();
			g_fExportScale = atof(ucpToken);
			cout << "Setting Scale to: " << g_fExportScale << endl;
			g_fTotalScale	= (1.0 / g_fUnitsMeter) * g_fExportScale;
		}

		if (!strcmp (ucpToken, "!filetype")) {
			cout << "Command filetype" << endl;
			}
		
		if (!strcmp (ucpToken, "!target"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Target file is: " << ucpToken << endl;
			ucaOutFile = ucpToken;
		}

		if (!strcmp (ucpToken, "!sd-export"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Setting Bones" << ucpToken << endl;
			ReadExportSD(ucpToken);
		}

		if (!strcmp (ucpToken, "!ad-export"))
		{
			strFileName = kMMScipt.GetToken();
			strName = kMMScipt.GetToken();
			cout << "Add Animation '" << strName.c_str() << "' from " << strFileName.c_str() << endl;
			ReadExportAD(strFileName.c_str(),strName.c_str());
		}

		if (!strcmp (ucpToken, "!add-md"))
		{
			strFileName = kMMScipt.GetToken();
			strName = kMMScipt.GetToken();
			cout << "Add Mesh '" << strName.c_str() << "' from " << strFileName.c_str() << endl;
			ReadCoreMesh(strFileName.c_str(),strName.c_str());
		}

		if (!strcmp (ucpToken, "!add-fd"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Command add-fd: " << ucpToken << endl;
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

bool ModellXXX::Export(MadExporter* mad, const char* filename)
{
	int i;
	cout << m_kBoneAnim.size() << endl;
	mad->SetSkelleton(m_akSkelleton);
	mad->SetAnimation(m_kBoneAnim);
	
//	mad->m_akSkelleton = m_akSkelleton;
//	mad->m_kBoneAnim = m_kBoneAnim;

/*	cout << "m_kBoneAnim.size: " << m_kBoneAnim.size() << endl;
	cout << "mad->m_kBoneAnim.size: " << mad->m_kBoneAnim.size() << endl;

	for(i=0; i<m_kBoneAnim.size(); i++) {
		cout << m_kBoneAnim[i].m_szName << endl;
		}

	for(i=0; i<mad->m_kBoneAnim.size(); i++) {
		cout << mad->m_kBoneAnim[i].m_szName << endl;
		}*/


//	OptimizeSubMeshes();
//	int imeshSize = m_kMesh.size();

	vector<Mad_CoreMesh>::iterator it;
	for(it = m_kMesh.begin(); it != m_kMesh.end(); it++)
		it->OptimizeSubMeshes();

	for(it = m_kMesh.begin(); it != m_kMesh.end(); it++)
	{
//		int iTotalNumOfVertex = it->akFrames[0].akVertex.size();
//		int iTotalTriangles = it->akFaces.size();
//		int iAntalFrames = it->akFrames.size();
		int iTotalNumOfVertex = it->NumOfVertexPerFrame();
		int iTotalTriangles = it->SizeFaces();
		int iAntalFrames = it->SizeFrames();
		
		it->kHead.iVersionNum		= 1;
//		it->kHead.iNumOfTextures	= it->akTextures.size();
		it->kHead.iNumOfTextures	= it->SizeTextures();
		it->kHead.iNumOfVertex		= iTotalNumOfVertex;
		it->kHead.iNumOfFaces		= iTotalTriangles;
		it->kHead.iNumOfFrames		= iAntalFrames;
//		it->kHead.iNumOfSubMeshes	= it->akSubMeshes.size();
//		it->kHead.iNumOfAnimation	= it->akAnimation.size();
		it->kHead.iNumOfSubMeshes	= it->SizeSubMesh();
		it->kHead.iNumOfAnimation	= it->SizeAnimations();
	}

//	mad->m_kMesh = m_kMesh;		
	mad->SetMesh(m_kMesh);

/*	Mad_CoreMesh* pkMesh = mad->GetMesh("mesh");
	pkMesh->kHead.iVersionNum		= 1;
	pkMesh->kHead.iNumOfTextures	= m_akTextureNames.size();
	pkMesh->kHead.iNumOfVertex		= iTotalNumOfVertex;
	pkMesh->kHead.iNumOfFaces		= iTotalTriangles;
	pkMesh->kHead.iNumOfFrames		= iAntalFrames;
	pkMesh->kHead.iNumOfSubMeshes	= m_akSubMesh.size();
	pkMesh->kHead.iNumOfAnimation	= akAnimation.size();

	for( i=0; i < m_akTextureNames.size(); i++) 
	{
		cout << "Texture " << i << ": " << m_akTextureNames[i].ucTextureName << endl;
		strcpy(pkMesh->akTextures[i].ucTextureName, m_akTextureNames[i].ucTextureName);
	}

	pkMesh->akFrames.resize(pkMesh->kHead.iNumOfFrames);
	pkMesh->akFaces.resize(pkMesh->kHead.iNumOfFaces);
	pkMesh->akSubMeshes.resize(pkMesh->kHead.iNumOfSubMeshes);
	
	// Copy Submeshes.
	pkMesh->akSubMeshes = m_akSubMesh;

	// Copy Faces
	for(i=0; i<pkMesh->kHead.iNumOfFaces; i++) 
	{
		pkMesh->akFaces[i].iIndex[0] = m_akFace[i].iIndex[0];
		pkMesh->akFaces[i].iIndex[1] = m_akFace[i].iIndex[1];
		pkMesh->akFaces[i].iIndex[2] = m_akFace[i].iIndex[2];
	}

	int framesize = m_akFrames.size();
	pkMesh->akFrames		= m_akFrames;
	pkMesh->akTextureCoo	= m_akTextureCoo;
	pkMesh->akAnimation		= akAnimation;
	pkMesh->akBoneConnections = m_iBoneLinks;
	*/

	return true;
}

void ModellXXX::OptimizeSubMeshes(void)
{
/*	if(m_akSubMesh.size() < 2)	return;
	
	cout << "Start OptimizeSubMeshes: " << m_akSubMesh.size() << endl;

	vector<Mad_CoreSubMesh>	akOldSubMesh;
	akOldSubMesh = m_akSubMesh;
	m_akSubMesh.clear();

	Mad_CoreSubMesh newsub;
	newsub.iFirstTriangle	= 0;
	newsub.iNumOfTriangles	= 1;
	newsub.iTextureIndex	= akOldSubMesh[0].iTextureIndex;

	int i;

	for(i=1; i<akOldSubMesh.size(); i++) {
		if(newsub.iTextureIndex != akOldSubMesh[i].iTextureIndex) {
			cout << "/" << endl;
			m_akSubMesh.push_back(newsub);
			newsub.iFirstTriangle	= i;
			newsub.iNumOfTriangles	= 1;
			newsub.iTextureIndex	= akOldSubMesh[i].iTextureIndex;
			}
		else {
			cout << ".";
			newsub.iNumOfTriangles++;
			}
		}

	m_akSubMesh.push_back(newsub);
	cout << "End OptimizeSubMeshes: " << m_akSubMesh.size() << endl;

	for(i=0; i<m_akSubMesh.size(); i++) {
		cout << "SubMesh[" << i << "]:" << m_akSubMesh[i].iFirstTriangle;
		cout << " / " << m_akSubMesh[i].iNumOfTriangles;
		cout << " / " << m_akSubMesh[i].iTextureIndex << endl;

		}
*/
}


Mad_CoreMesh* ModellXXX::GetMesh(const char* ucaName)
{
	vector<Mad_CoreMesh>::iterator it;

	for(it = m_kMesh.begin(); it != m_kMesh.end(); it++)
	{
		if(strcmp(it->m_acName, ucaName) == 0)
			return it;
	}

	// Finns ingen mesh med det namnet så skapa den och returnera den.
	Mad_CoreMesh kNewMesh;
	kNewMesh.Clear();
	strcpy(kNewMesh.m_acName, ucaName);
	m_kMesh.push_back(kNewMesh);
	return &m_kMesh.back();
}