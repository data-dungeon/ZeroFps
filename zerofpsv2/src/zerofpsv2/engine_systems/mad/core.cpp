#include "mad_core.h"
#include "../../basic/quaternion.h"
#include "../../basic/zfassert.h"
//#include "../basic/zfassert.h"
//#include "globals.h"

Matrix4		g_Madkbonetransform[MAX_BONES];		// Bone transformation matrix
Matrix4		g_FullBoneTransform[MAX_BONES];		// Bone transformation matrix.
Quaternion	g_Madq[MAX_BONES];					// Quat angle for bone.
Vector3		g_Madpos[MAX_BONES];				// Position for bone.
Vector3		g_TransformedVertex[10000];			// Transformed Skinvertex Position.
Vector3		g_TransformedNormal[10000];			// Transformed Skinvertex Normals.

#define		MAD_FPS		15						// Def FPS for Mad's.
float const g_fMadFrameTime = 1.0 / MAD_FPS;	// Def time between frames in Mad.

Mad_CoreMesh* g_pkSelectedMesh;

Mad_Core::Mad_Core()
{
	iActiveFrame		= 0;
	iActiveKeyFrame	= 0;
	m_bDrawNormals		= false;
	iActiveAnimation	= 0;
	iBoneFrame			= 0;
	m_fBoundRadius		= 0;

	m_bInterpolVertexFrames = false;
}

Mad_Core::~Mad_Core()
{

}

void Mad_Core::Save_SD(ZFVFile* pkZFile)
{
	int iNumOfBones = m_kSkelleton.size();
	pkZFile->Write(&iNumOfBones, sizeof(int),1);
	//	fwrite(&iNumOfBones, sizeof(int),1,pkFp);

	for(unsigned int i=0; i < m_kSkelleton.size(); i++) {
		pkZFile->Write(&m_kSkelleton[i],sizeof(Mad_CoreBone),1);
	//	fwrite(&m_kSkelleton[i],sizeof(Mad_CoreBone),1,pkFp);
	}
}

void Mad_Core::Save_MD(int iMeshId, ZFVFile* pkZFile)
{
	m_kMesh[iMeshId].Save(pkZFile);
}

void Mad_Core::Save_AD(int iMeshId, ZFVFile* pkZFile)
{
	m_kBoneAnim[iMeshId].Save(pkZFile);
}

void Mad_Core::Save_SD(const char* filename)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string(filename),0,true) ) {
		cout << "Failed to open " << filename << endl;
		return;
		}
	
	//FILE* fp = fopen(filename, "wb");
	Save_SD(&kZFile);
	kZFile.Close();
	//fclose(fp);
}

void Mad_Core::Save_AD(int iMeshId, const char* filename)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string(filename),0,true) ) {
		cout << "Failed to open " << filename << endl;
		return;
		}

	//FILE* fp = fopen(filename, "wb");
	Save_AD(iMeshId, &kZFile);
	kZFile.Close();
//	fclose(fp);
}


void Mad_Core::Save_MD(int iMeshId, const char* filename)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string(filename),0,true) ) {
		cout << "Failed to open " << filename << endl;
		return;
		}

//	FILE* MadFp = fopen(filename, "wb");
	Save_MD(iMeshId, &kZFile);
	kZFile.Close();
//	fclose(MadFp);
	
}

void Mad_Core::Save_MAD(const char* filename)
{
	unsigned int i;

	PrintCoreInfo();

//	m_kMesh[0].CreateVertexNormals();

	ZFVFile kZFile;
	if( !kZFile.Open(string(filename),0,true) ) {
		cout << "Failed to open " << filename << endl;
		return;
		}
/*	FILE* MadFp = fopen(filename, "wb");
	if(!MadFp) {
		cout << "Failed to open '" << filename << "' for writing" << endl;
		return;
		}*/

	Mad_Header m_kMadHeader;
	// Setup MAD Header.
	m_kMadHeader.m_iVersionNum		= MAD_VERSION;
	m_kMadHeader.m_iNumOfMeshes		= m_kMesh.size();
	m_kMadHeader.m_iNumOfAnimations = m_kBoneAnim.size();
		
	// Write MAD Header.
	kZFile.Write(&m_kMadHeader,sizeof(Mad_Header), 1);
	//fwrite(&m_kMadHeader,sizeof(Mad_Header), 1, MadFp);
	
	// Write SD
	Save_SD(&kZFile);

	// Write AD
	for(i=0; i<m_kMadHeader.m_iNumOfAnimations; i++)
		Save_AD(i, &kZFile);

	// Write MD
	for(i=0; i<m_kMadHeader.m_iNumOfMeshes; i++)
		Save_MD(i, &kZFile);

	// Write MA

	kZFile.Close();
//	fclose(MadFp);

}

 

void Mad_Core::SetFrameI(int iFrame)
{
	if(iFrame >= 0 && iFrame < m_kMesh[0].kHead.iNumOfFrames)
		iActiveFrame = iFrame;
}

void Mad_Core::LoopPlayAnim(int iAnim)
{
	iActiveKeyFrame++;
	if(iActiveKeyFrame >= int(m_kMesh[0].akAnimation[iAnim].KeyFrame.size()))
		iActiveKeyFrame = 0;

	iActiveFrame = m_kMesh[0].akAnimation[iAnim].KeyFrame[iActiveKeyFrame].iVertexFrame;
}


void Mad_Core::SetFrame_NormalizedTime(float fNormTime)
{
	
}

void Mad_Core::SetFrame_InSeconds(float fSeconds)
{

}

char* Mad_Core::GetName(void)
{
	return Name;
}


float Mad_Core::GetAnimationLengthInS(int iAnim)
{ 
	int iNumOfKeyFrames = m_kBoneAnim[iAnim].m_kBoneKeyFrames.size();
	return (iNumOfKeyFrames * g_fMadFrameTime);
}

int Mad_Core::GetAnimationTimeInFrames(int iAnim)
{
	return m_kMesh[0].akAnimation[iAnim].KeyFrame.size();
	return 0;
}

void Mad_Core::SetReplaceTexture(char* szFileName)
{
/*	char nisse[256];
	for(int i = 0; i< kHead.iNumOfTextures; i++) {
		sprintf(nisse, "../data/textures/%s.bmp", szFileName);
//		aiReplaceTextureIndex[i] = pkTextureManger->Load(nisse,0);
*/
}


/*
void InversTransformMatrix(Matrix4& pkIn, Matrix4& pkOut)
{
	pkOut = pkIn;
	pkOut = pkIn.Invert2();
}*/

void Mad_Core::SetUpBindPose()
{
	Matrix4		kMadkBoneMatrix;					
	
	unsigned int i;
	Vector3 Angles;

	for(i=0; i<m_kSkelleton.size(); i++) {
		Angles = m_kSkelleton[i].m_kRotation;
		g_Madq[i].AngleQuaternion(Angles);
		g_Madpos[i] = m_kSkelleton[i].m_kPosition;
		}

	Quaternion kTestQ;
	for (i = 0; i < m_kSkelleton.size(); i++) {
		kMadkBoneMatrix.Identity();
		kMadkBoneMatrix = g_Madq[i].Inverse();
		kMadkBoneMatrix.SetPos(g_Madpos[i]);

		if (m_kSkelleton[i].m_iParent == -1) {
			g_Madkbonetransform[i] = kMadkBoneMatrix;
		} 
	 	else {
			g_Madkbonetransform[i] = kMadkBoneMatrix * g_Madkbonetransform[m_kSkelleton[i].m_iParent];
		}
	}

	for (i = 0; i < m_kSkelleton.size(); i++) {
		m_MadkbonetransformI[i] = g_Madkbonetransform[i].Invert2();
		//InversTransformMatrix(g_Madkbonetransform[i],g_MadkbonetransformI[i]);
		}	
}

/*
void Core::CreateFrame(void)
{
	Mad_CoreMesh* mesh = &m_kMesh[0];
	g_pkVertex	= &mesh->akFrames[iActiveFrame].akVertex[0];
	g_pkNormals = &mesh->akFrames[iActiveFrame].akNormal[0];

	if(!m_bInterpolVertexFrames)
		return;

	Vector3* pkStartFrame;
	Vector3* pkEndFrame;
	Vector3* pkFrame;
	int i;

	int iNumOfFrame = GetAnimationTimeInFrames(iActiveAnimation);
	int iStartFrame = int(fActiveAnimationTime / 0.1);
	int iEndFrame = iStartFrame + 1;
	if(iEndFrame >= iNumOfFrame) 
		iEndFrame = 0;
	
	int iStartVertexFrame = m_kMesh[0].akAnimation[iActiveAnimation].KeyFrame[iStartFrame].iVertexFrame;
	int iEndVertexFrame = m_kMesh[0].akAnimation[iActiveAnimation].KeyFrame[iEndFrame].iVertexFrame;

	pkStartFrame = &mesh->akFrames[iStartVertexFrame].akVertex[0];
	pkEndFrame = &mesh->akFrames[iEndVertexFrame].akVertex[0];
	pkFrame = g_akVertexBuffer;

	for(i=0; i< mesh->kHead.iNumOfVertex; i++) {
		Vector3 diff = pkEndFrame[i] - pkStartFrame[i];
		pkFrame[i] = pkStartFrame[i] + diff * fFrameOffs;
		}

	pkStartFrame = &mesh->akFrames[iStartVertexFrame].akNormal[0];
	pkEndFrame = &mesh->akFrames[iEndVertexFrame].akNormal[0];
	pkFrame = g_akNormalBuffer;

	for(i=0; i< mesh->kHead.iNumOfVertex; i++) {
		Vector3 diff = pkEndFrame[i] - pkStartFrame[i];
		pkFrame[i] = pkStartFrame[i] + diff * fFrameOffs;
		}

	g_pkVertex	= g_akVertexBuffer;
	g_pkNormals = g_akNormalBuffer;
}*/


void Mad_Core::SetupBonePose()
{
	Matrix4		kMadkBoneMatrix;					

	unsigned int i;
	Vector3 Angles;

	int iNumOfFrame = m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size();
	int iStartFrame = int(fActiveAnimationTime / g_fMadFrameTime);
	int iEndFrame = iStartFrame + 1;
	if(iStartFrame >= iNumOfFrame) 
		iStartFrame = 0;
	if(iEndFrame >= iNumOfFrame) 
		iEndFrame = 0;

	Mad_CoreBoneKey* pkStartKey = &m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames[iStartFrame].m_kBonePose[0];
	Mad_CoreBoneKey* pkEndKey = &m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames[iEndFrame].m_kBonePose[0];

	ZFAssert(pkStartKey, "Mad_Core::SetupBonePose: No StartKey");
	ZFAssert(pkEndKey, "Mad_Core::SetupBonePose: No EndKey");


	Quaternion kStart, kEnd;

	float OneMinusFrameOffs = float(1.0) - fFrameOffs;

	for(i=0; i<m_kSkelleton.size(); i++) {
		// Get Start/End Keys
		kStart.AngleQuaternion(pkStartKey[i].m_kRotation); 
		kEnd.AngleQuaternion(pkEndKey[i].m_kRotation); 

		// Interp Keys
		g_Madq[i].QuaternionSlerp(&kStart, &kEnd, fFrameOffs);

		if(m_kSkelleton[i].m_iParent != -1)
			g_Madpos[i] = pkStartKey[i].m_kPosition * OneMinusFrameOffs + pkEndKey[i].m_kPosition * fFrameOffs;
		else 
			g_Madpos[i].Set(0,0,0);
		}

	// Controllers
	
/*	Vector3	kBindAngle;
	Vector3 kBindPos;

	for(i=0; i<m_kControllers.size(); i++) {
		float fNew = m_kControllers[i].m_fMin + (m_kControllers[i].m_fMax - m_kControllers[i].m_fMin) * m_kControllers[i].m_fValue;
		cout << "fNew:  " << fNew << endl;
		
		kBindAngle = m_kSkelleton[m_kControllers[i].m_iJointID].m_kRotation;
		kBindPos = m_kSkelleton[m_kControllers[i].m_iJointID].m_kPosition;

		switch(m_kControllers[i].m_eAxis) {
			case CONTROLL_ANGLE_X:	kBindAngle.x = fNew;		break;
			case CONTROLL_ANGLE_Y:	kBindAngle.y = fNew;	 break;
			case CONTROLL_ANGLE_Z:	kBindAngle.z = fNew;	 break;
			case CONTROLL_MOVE_X:	kBindPos.x = fNew;		 break;
			case CONTROLL_MOVE_Y:	kBindPos.y = fNew;	 	 break;
			case CONTROLL_MOVE_Z:	kBindPos.z = fNew;		 break;
			}

		g_Madq[m_kControllers[i].m_iJointID].AngleQuaternion(kBindAngle);
		g_Madpos[m_kControllers[i].m_iJointID] = kBindPos;
		}*/
	

	for (i = 0; i < m_kSkelleton.size(); i++) {
		kMadkBoneMatrix.Identity();
		kMadkBoneMatrix = g_Madq[i].Inverse();
		kMadkBoneMatrix.SetPos(g_Madpos[i]);

		if (m_kSkelleton[i].m_iParent == -1) {
			g_Madkbonetransform[i] = kMadkBoneMatrix;
		} 
		else {
			g_Madkbonetransform[i] = kMadkBoneMatrix * g_Madkbonetransform[m_kSkelleton[i].m_iParent];
		}

		g_FullBoneTransform[i] = m_MadkbonetransformI[i] * g_Madkbonetransform[i];
	}
}

void Mad_Core::ClearReplaceTexture(void)
{
//	for(int i = 0; i< kHead.iNumOfTextures; i++)
//		aiReplaceTextureIndex[i] = NO_TEXTURE;
}

int	Mad_Core::GetNumOfAnimations()
{
	return m_kBoneAnim.size();
}

void Mad_Core::SetAnimationTime( int iAnim, float fTime )
{
	fActiveAnimationTime = fTime;
	iActiveAnimation = iAnim;

	int iNumOfFrame = GetAnimationTimeInFrames(iActiveAnimation);
	int iFrame = int(fActiveAnimationTime / g_fMadFrameTime);
	fFrameOffs = (fActiveAnimationTime / g_fMadFrameTime) - iFrame;
	SetFrameI(m_kMesh[0].akAnimation[iActiveAnimation].KeyFrame[iFrame].iVertexFrame);
}

void Mad_Core::SetBoneAnimationTime(int iAnim, float fTime )
{
	fActiveAnimationTime = fTime;
	iActiveAnimation = iAnim;

	int iNumOfFrame = m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size();
	int iFrame = int(fActiveAnimationTime / g_fMadFrameTime);
	fFrameOffs = (fActiveAnimationTime / g_fMadFrameTime) - iFrame;
	iBoneFrame = iFrame;
	if(iBoneFrame >= int(m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size()) )
		iBoneFrame = 0;
}


void Mad_Core::LoadSkelleton(ZFVFile* pkZFile)
{
	Mad_CoreBone kNewBone;
	int iNumOfBones;
	pkZFile->Read(&iNumOfBones, 1, sizeof(int));
	//fread(&iNumOfBones, 1, sizeof(int), pkFp);

	for(int i=0; i<iNumOfBones; i++) {
		pkZFile->Read(&kNewBone, 1, sizeof(Mad_CoreBone));
		//fread(&kNewBone, 1, sizeof(Mad_CoreBone), pkFp);
		m_kSkelleton.push_back(kNewBone);
		}
}

void Mad_Core::LoadAnimation(ZFVFile* pkZFile)
{
	Mad_CoreBoneAnimation NewBoneAnim;
	NewBoneAnim.Clear();
	NewBoneAnim.Load(pkZFile);
	m_kBoneAnim.push_back(NewBoneAnim);
}

void Mad_Core::LoadMesh(ZFVFile* pkZFile)
{
	Mad_CoreMesh	kNewMesh;
	kNewMesh.Load(pkZFile);
	m_kMesh.push_back(kNewMesh);

//	Mad_CoreMesh* pkMesh = &m_kMesh[0];
}


void Mad_Core::LoadSkelleton(const char* MadFileName)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string("test.sd"),0,true) ) {
		cout << "Failed to open " << "test.sd" << endl;
		return;
		}

	//FILE*	fp = fopen("test.sd","rb");
	LoadSkelleton(&kZFile);
	kZFile.Close();
	//fclose(fp);
	SetUpBindPose();
}

void Mad_Core::LoadAnimation(const char* MadFileName)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string("test.sd"),0,true) ) {
		cout << "Failed to open " << "test.sd" << endl;
		return;
		}

	//FILE*	fp = fopen("test.ad","rb");
	LoadAnimation(&kZFile);
	kZFile.Close();
	//fclose(fp);
}

void Mad_Core::LoadMesh(const char* MDFileName)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string("test.sd"),0,true) ) {
		cout << "Failed to open " << "test.sd" << endl;
		return;
		}
//	FILE* fp = fopen(MDFileName, "rb");
	LoadMesh(&kZFile);
	kZFile.Close();
//	fclose(fp);
}


bool Mad_Core::Create(string MadFileName)
//bool Mad_Core::LoadMad(const char* MadFileName)
{
	strcpy(Name,MadFileName.c_str());

	ZFVFile kZFile;
	if( !kZFile.Open(MadFileName,0,false) ) {
		cout << "Failed to find" << MadFileName.c_str();
		cout << "\n";
		return false;
		}

/*	FILE* MadFp = fopen(MadFileName, "rb");
	if(!MadFp) {
		cout << "Failed to find" << MadFileName << endl;
		return false;
		}
	
	*/

	Mad_Header kMadHeader;
	kZFile.Read(&kMadHeader,sizeof(Mad_Header), 1);
	//fread(&kMadHeader,sizeof(Mad_Header), 1, MadFp);
	
	// Check Versions Num
	if(kMadHeader.m_iVersionNum != MAD_VERSION) {
		cout << "Wrong version " << kMadHeader.m_iVersionNum << " in " << MadFileName << endl;
		}	

/*
	cout << "m_iVersionNum: "		<< kMadHeader.m_iVersionNum << endl;
	cout << "m_iNumOfMeshes: "		<< kMadHeader.m_iNumOfMeshes << endl;
	cout << "m_iNumOfAnimations: "	<< kMadHeader.m_iNumOfAnimations << endl;
*/
	

	LoadSkelleton(&kZFile);
//	LoadSkelleton(MadFp);
	SetUpBindPose();

	unsigned int i;
	for(i=0; i<kMadHeader.m_iNumOfAnimations; i++)
		LoadAnimation(&kZFile);
		//LoadAnimation(MadFp);

	for(i=0; i<kMadHeader.m_iNumOfMeshes; i++)
		LoadMesh(&kZFile);
		//LoadMesh(MadFp);

//	fclose(MadFp);
	kZFile.Close();
//	PrintCoreInfo();

	if(m_kSkelleton.size() == 1) {
		for(i=0; i<m_kMesh.size(); i++)
			m_kMesh[i].bNotAnimated = true;
		}

	CalculateRadius();

	return true;
}

Mad_CoreMesh* Mad_Core::GetMeshByID(int iMesh)
{
	if(iMesh < 0)	
		return NULL;
	if(iMesh >= int(m_kMesh.size()))
		return NULL;

	return &m_kMesh[iMesh];
}

Vector3*  Mad_Core::GetVerticesPtr()
{
	if(g_pkSelectedMesh->bNotAnimated)
		return &g_pkSelectedMesh->akFrames[0].akVertex[0];

	return g_TransformedVertex;
}

Vector3* Mad_Core::GetNormalsPtr()
{
	if(g_pkSelectedMesh->bNotAnimated)
		return &g_pkSelectedMesh->akFrames[0].akNormal[0];

	return g_TransformedNormal;
}


int* Mad_Core::GetFacesPtr(Mad_CoreMesh* pkMesh,Mad_CoreSubMesh* pkSubMesh)
{
	return (int*) &pkMesh->akFaces[pkSubMesh->iFirstTriangle ].iIndex;	
		
}

Mad_TextureCoo* Mad_Core::GetTextureCooPtr(Mad_CoreMesh* pkMesh)
{
	return &pkMesh->akTextureCoo[0];
}

int Mad_Core::GetTextureID()
{
	return 0;
}

void Mad_Core::PrepareMesh(Mad_CoreMesh* pkMesh)
{
	Vector3* pkVertexDst;
	Vector3* pkNormalDst;

	g_pkSelectedMesh = pkMesh;
	if(pkMesh->bNotAnimated)
		return;
	
	//Matrix4 kFullTransform;
	int* piBoneConnection;
	Vector3* pkVertex = &pkMesh->akFrames[0].akVertex[0];
	Vector3* pkNormal = &pkMesh->akFrames[0].akNormal[0];
 
	pkVertexDst = g_TransformedVertex;
	pkNormalDst = g_TransformedNormal;
	piBoneConnection = &pkMesh->akBoneConnections[0];

	for(int i = 0; i<pkMesh->kHead.iNumOfVertex; i++) {
		//iBoneConnection = pkMesh->akBoneConnections[i];
		*pkVertexDst = g_FullBoneTransform[*piBoneConnection].VectorTransform( *pkVertex );
		*pkNormalDst = g_FullBoneTransform[*piBoneConnection].VectorRotate( *pkNormal );

		pkVertexDst++;
		pkNormalDst++;
		piBoneConnection++;
		pkVertex++;
		pkNormal++;

		
		/*kFullTransform.Identity();
		kFullTransform = g_FullBoneTransform[ pkMesh->akBoneConnections[i] ];
		g_TransformedVertex[i] = kFullTransform.VectorTransform(pkMesh->akFrames[0].akVertex[i]);
		g_TransformedNormal[i] = kFullTransform.VectorRotate(pkMesh->akFrames[0].akNormal[i]);
		*/
		}
}


unsigned int Mad_Core::GetNumOfBones()
{
	return m_kSkelleton.size();
}

Vector3 Mad_Core::GetBonePosition(int iBone)
{
	return g_Madkbonetransform[iBone].GetPos();
}

int Mad_Core::GetBoneParent(int iBone)
{
	return m_kSkelleton[iBone].m_iParent;
}

int	Mad_Core::NumOfMeshes()
{
	return m_kMesh.size();

}

Mad_CoreMesh* Mad_Core::CreateCoreMesh(const char* szName)
{
	Mad_CoreMesh kNewMesh;
	kNewMesh.Clear();
	strcpy(kNewMesh.m_acName, szName);
	m_kMesh.push_back(kNewMesh);
	return &m_kMesh.back();

}

void Mad_Core::SetSkelleton(vector<Mad_CoreBone>	kSkelleton)
{
	m_kSkelleton = kSkelleton;
}

void Mad_Core::SetMesh(vector<Mad_CoreMesh>	kMesh)
{
	m_kMesh = kMesh;
}

void Mad_Core::SetAnimation(vector<Mad_CoreBoneAnimation>	kBoneAnim)
{
	m_kBoneAnim = kBoneAnim;
} 

void Mad_Core::PrintCoreInfo()
{
	unsigned int i;
	cout << "Num Of Animations: " << m_kBoneAnim.size() << endl;
	for(i=0; i<m_kBoneAnim.size(); i++)
		cout << " [" << i << "]: " << m_kBoneAnim[i].m_szName << endl;

	cout << "Num Of Mesh: " << m_kMesh.size() << endl;
	for(i=0; i<m_kMesh.size(); i++)
		cout << " [" << i << "]: " << m_kMesh[i].kHead.iNumOfSubMeshes << endl;

	cout << "Num Of Bones: " << m_kSkelleton.size() << endl;
}

/*
	Get radius of object (All mesh object).
*/
void Mad_Core::CalculateRadius()
{
	if(m_kMesh.size() <= 0) {
		m_fBoundRadius = 0;
		return;
		}

	Vector3 CenterPos = GetJointPosition(NULL);

	Mad_CoreMesh* pkMesh = &m_kMesh[0];		
	Vector3* pkVertex = &pkMesh->akFrames[0].akVertex[0];		

	float fDist=0;

	for(unsigned int i=0; i <pkMesh->akFrames[0].akVertex.size(); i++) 
	{
//		float newdist = (pkVertex[i] - CenterPos).Length();
		float newdist = pkVertex[i].Length();		
		
		if(newdist > fDist)
			fDist = newdist;


	}

	m_fBoundRadius = fDist;
	
	
/*
	Vector3 kMin = pkVertex[0] - CenterPos;
	Vector3 kMax = kMin;
	Vector3 kDiff;

	for(unsigned int i=0; i <pkMesh->akFrames[0].akVertex.size(); i++) {
		kDiff = pkVertex[i] - CenterPos;
		
		if(kDiff.x < kMin.x)
			kMin.x = kDiff.x;
		else if (kDiff.x > kMax.x)
			kMax.x = kDiff.x;

		if(kDiff.y < kMin.y)
			kMin.y = kDiff.y;
		else if (kDiff.y > kMax.y)
			kMax.y = kDiff.y;

		if(kDiff.z < kMin.z)
			kMin.z = kDiff.z;
		else if (kDiff.z > kMax.z)
			kMax.z = kDiff.z;
		}


	Vector3 kDiagonal = (kMax - kMin) / 2;
	float fRadius = kDiagonal.Length();
	cout << "fRadius: " << fRadius << endl;

	kMin.Abs();
	kMax.Abs();
	float fMaxDist = kMin.x;
	if(kMin.y > fMaxDist)	fMaxDist = kMin.y;
	if(kMin.z > fMaxDist)	fMaxDist = kMin.z;

	if(kMax.x > fMaxDist)	fMaxDist = kMax.x;
	if(kMax.y > fMaxDist)	fMaxDist = kMax.y;
	if(kMax.z > fMaxDist)	fMaxDist = kMax.z;

	m_fBoundRadius = fMaxDist;	// * 1.42;
	
	cout << "m_fBoundRadius: " << fMaxDist << endl;
//	return fRadius;*/
}

float Mad_Core::GetRadius()
{
	return m_fBoundRadius;	// * float(0.01);	// // BM-NOTE: Exporter scale stämmer inte ännu.
}

int Mad_Core::GetJointID(char* szJointName)
{
	for(unsigned int i=0; i<m_kSkelleton.size(); i++) {
		if(strcmp(m_kSkelleton[i].m_acName, szJointName) == 0) 
			return i;
		}

	return -1;
}

Vector3 Mad_Core::GetJointPosition(char* szJointName)
{
	unsigned int i;

	if(szJointName) {
		for(i=0; i<m_kSkelleton.size(); i++) {
			if(strcmp(m_kSkelleton[i].m_acName, szJointName) == 0) 
				return m_kSkelleton[i].m_kPosition;
			}
		}
	else {
		for(i=0; i<m_kSkelleton.size(); i++) {
			if(m_kSkelleton[i].m_iParent == -1) 
				return m_kSkelleton[i].m_kPosition;
			}
		}

	return Vector3::ZERO;
}

void Mad_Core::CreateController(char* szName, char* szJoint, ControllAxis eAxis, float fMin, float fMax)
{
	for(unsigned int i=0; i<m_kControllers.size(); i++) {
		if(strcmp(szName,m_kControllers[i].m_szName) == 0) {
			return;
			}
		}


	Controller	kNewControll;
	
	strcpy(kNewControll.m_szName, szName);
	strcpy(kNewControll.m_szJointName, szJoint);
	kNewControll.m_iJointID = GetJointID(szJoint);
	kNewControll.m_eAxis = eAxis;
	kNewControll.m_fMin = DegToRad(fMin);
	kNewControll.m_fMax = DegToRad(fMax);
	kNewControll.m_fValue = 0.0;

	if(kNewControll.m_iJointID == -1) {
		cout << "Failed to add jointcontroller " << szName << "to joint " << szJoint << endl;
		}

	m_kControllers.push_back(kNewControll);

	cout << "Controller createt for bone" << kNewControll.m_iJointID << endl;
}

void Mad_Core::SetControll(char* szName, float fValue)
{
	if(fValue < 0.0)
		fValue = 0.0;
	if(fValue > 1.0)
		fValue = 1.0;

	for(unsigned int i=0; i<m_kControllers.size(); i++) {
		if(strcmp(szName,m_kControllers[i].m_szName) == 0) {
			m_kControllers[i].m_fValue = fValue;
			return;
			}
		}

	cout << "Controller not found" << endl;
}

int	Mad_Core::GetMeshIDByName(char* szName)
{
	for(unsigned int i=0; i<m_kMesh.size(); i++) {
		if(strcmp(m_kMesh[i].m_acName, szName) == 0)
			return i;
		}

	return -1;
}



ZFResource* Create__Mad_Core()
{
	return new Mad_Core;
}
