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
	Vector3		kVertex;
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

	m_akFrames.push_back(kFrame);
}

void ModellXXX::ReadVertexFrame(FILE* fp, int iNumTriangles)
{
	Vector3		kVertex;
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

			cout << "Frame: " << iFrame << endl;
			ReadVertexFrame(fp, iNumTris);
		}
		else 
			done = true;
	}

	cout << endl;

	fclose(fp);
	m_akFrames.clear();	
}

void ModellXXX::ReadCoreMesh(char* filename)
{
	Vector3		kVertex;
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

	cout << "CoreMesh: " << filename << " ";

	char tmpstr[256];

	FILE* fp = fopen(filename,"rt");

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
		iTexture = AddTexture(TextureName);

		for(int v = 0; v < 3; v++)
		{
			fscanf(fp, " <%d,%f,%f,%f,%f,%f,%f,%f,%f>",&iBoneLink, &kVertex.x,&kVertex.y,&kVertex.z,
				&kNormal.x,&kNormal.y,&kNormal.z,
				&kTextureCoo.s,&kTextureCoo.t);
			kFace.iIndex[v] = kFrame.akVertex.size();
			kFrame.akVertex.push_back(kVertex);
			kFrame.akNormal.push_back(kNormal);			

			
			m_iBoneLinks.push_back(iBoneLink);
			
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

	m_akFrames.push_back(kFrame);

	cout << endl;

	fclose(fp);
//	m_akFrames.clear();	
}



void ModellXXX::ReadAnimation(char* filename)
{
	cout << "Base: " << filename << " ";

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

	fclose(fp);
}


/*struct BoneData
{
	int			m_iParent;
	char		m_szName[64];
	Vector3		m_kPosition;
	Vector3		m_kRotation;
};*/

/*
 0 left_ankle -1  3.663815 -46.230229 -2.260499  -0.000000 0.000000 -0.000000 
 1 left_ball 0  0.000000 -1.410943 6.541643  0.000000 0.000000 0.000000 

*/
void ModellXXX::ReadExportSD(char* filename)
{
	cout << "Skelleton: " << filename << " ";
	char tmpstr[256];
	bool done = false;

	FILE* fp = fopen(filename,"rt");

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

void ModellXXX::ReadExportAD(char* filename)
{
	cout << "Animation: " << filename << " ";
	char tmpstr[256];
	bool done = false;

	FILE* fp = fopen(filename,"rt");

	int res;
	int iBoneId;
	int iFrameNum;
	iFrameNum = -1;

	Mad_BoneKey				kNewBoneKey;
	Mad_CoreBoneAnimation	kNewBoneAnim;
	Mad_BoneKeyFrame		kNewBoneKeyFrame;

	kNewBoneAnim.Clear();
	kNewBoneKeyFrame.Clear();
	kNewBoneAnim.Clear();

	while(!done) {
		res = fscanf(fp, "%s",tmpstr);
		if(res ==  EOF)
			break;
		
		if(strcmp(tmpstr, "Frame") == 0) {
			if(iFrameNum != -1) {
				kNewBoneAnim.m_kBoneKeyFrames.push_back(kNewBoneKeyFrame);
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
		kNewBoneKeyFrame.m_kBonePose.push_back(kNewBoneKey);
//		kNewTrack = kNewAnimation.GetTrackForBone(iBoneId);
//		kNewTrack->m_kKeyFrames.push_back(kNewBoneKey);
	}

	fclose(fp);

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
	ScriptFile kMMScipt;
	kMMScipt.LoadScript(filename);

	char* ucpToken;
	ucpToken = kMMScipt.GetToken();

	while(ucpToken)
	{
		if (!strcmp (ucpToken, "!filetype"))
			cout << "Command filetype" << endl;
		
		if (!strcmp (ucpToken, "!add-fd"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Command add-fd: " << ucpToken << endl;
			ReadBaseFrame(ucpToken);
		}

		if (!strcmp (ucpToken, "!add-md"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Command add-md: " << ucpToken << endl;
			ReadCoreMesh(ucpToken);
			//ReadBaseFrame(ucpToken);
		}

		if (!strcmp (ucpToken, "!add-meshanim"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Command add-meshanim: " << ucpToken << endl;
			ReadAnimation(ucpToken);
		}

		if (!strcmp (ucpToken, "!sd-export"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Command sd-export: " << ucpToken << endl;
			ReadExportSD(ucpToken);
		}

		if (!strcmp (ucpToken, "!ad-export"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Command ad-export: " << ucpToken << endl;
			ReadExportAD(ucpToken);
		}

		ucpToken = kMMScipt.GetToken();
	}
}

bool ModellXXX::Export(MadExporter* mad, const char* filename)
{
	int i;
	mad->m_akSkelleton = m_akSkelleton;
	mad->m_kBoneAnim = m_kBoneAnim;

	OptimizeSubMeshes();

	int iTotalNumOfVertex = m_akFrames[0].akVertex.size();
	int iTotalTriangles = m_akFace.size();
	int iAntalFrames = m_akFrames.size();


	Mad_CoreMesh* pkMesh = mad->GetMesh("mesh");
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


	return true;
}

void ModellXXX::OptimizeSubMeshes(void)
{
	if(m_akSubMesh.size() < 2)	return;
	
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

}
