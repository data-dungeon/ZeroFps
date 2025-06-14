#include "mad_core.h"
#include "../../basic/quaternion.h"
#include "../../basic/zfassert.h"
#include "../../basic/zfsystem.h"
#include "../../basic/math.h"



Matrix4		g_Madkbonetransform[MAX_BONES];		// Bone transformation matrix
//Matrix4		g_FullBoneTransform[MAX_BONES];		// Bone transformation matrix.	// Vimrp2
//Quaternion	g_Madq[MAX_BONES];						// Quat angle for bone.
//Vector3		g_Madpos[MAX_BONES];						// Position for bone.
Vector3		g_TransformedVertex[10000];			// Transformed Skinvertex Position.
Vector3		g_TransformedNormal[10000];			// Transformed Skinvertex Normals.
BoneTransform	g_kBoneTransform[MAX_BONES];


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
	m_fSize				= 0;

	m_bLoaded			= false;
	m_bInterpolVertexFrames = false;
	
	//m_bSynced 			=false;	//enable asyncron loading of this resource
}

Mad_Core::~Mad_Core()
{

}

void Mad_Core::Save_SD(ZFVFile* pkZFile)
{
	int iNumOfBones = m_kSkelleton.size();
	pkZFile->Write(&iNumOfBones, sizeof(int),1);

	for(unsigned int i=0; i < m_kSkelleton.size(); i++) {
		pkZFile->Write(&m_kSkelleton[i],sizeof(Mad_CoreBone),1);
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
	
	Save_SD(&kZFile);
	kZFile.Close();
}

void Mad_Core::Save_AD(int iMeshId, const char* filename)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string(filename),0,true) ) {
		cout << "Failed to open " << filename << endl;
		return;
		}

	Save_AD(iMeshId, &kZFile);
	kZFile.Close();
}


void Mad_Core::Save_MD(int iMeshId, const char* filename)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string(filename),0,true) ) {
		cout << "Failed to open " << filename << endl;
		return;
		}

	Save_MD(iMeshId, &kZFile);
	kZFile.Close();
}

void Mad_Core::Save_MAD(const char* filename)
{
	unsigned int i;

	PrintCoreInfo();

	ZFVFile kZFile;

	if( !kZFile.Open(string(filename),0,true) ) 
	{
		cout << "Failed to open " << filename << endl;
		return;
	}
 
	Mad_Header m_kMadHeader;
	// Setup MAD Header.
	m_kMadHeader.m_iVersionNum		= MAD_VERSION;
	m_kMadHeader.m_iNumOfMeshes		= m_kMesh.size();
	m_kMadHeader.m_iNumOfAnimations = m_kBoneAnim.size();
		
	// Write MAD Header.
	kZFile.Write(&m_kMadHeader,sizeof(Mad_Header), 1);
	
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
}

 
/*
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
}*/


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
	if(iAnim < 0) // zeblar: l�ger till en check... fick ett negativt v�rde inskickat en g�ng som res i krash.
		iAnim = 0;

	int iNumOfKeyFrames = m_kBoneAnim[iAnim].m_kBoneKeyFrames.size();
	return (iNumOfKeyFrames * g_fMadFrameTime);
}

/*
int Mad_Core::GetAnimationTimeInFrames(int iAnim)
{
	return m_kMesh[0].akAnimation[iAnim].KeyFrame.size();
	return 0;
}*/

string Mad_Core::GetAnimationName(int iIndex)
{
	if( iIndex >= 0 && iIndex < m_kBoneAnim.size())
		return m_kBoneAnim[iIndex].m_szName;
	else
		return "error";
	
}

int Mad_Core::GetAnimIndex(const char* szName)
{
	for(unsigned int i=0; i<m_kBoneAnim.size(); i++) {
		if(strcmp(szName, m_kBoneAnim[i].m_szName) == 0)
			return i;
		}
	
	return MAD_NOANIMINDEX;
}
	
void Mad_Core::SetReplaceTexture(char* szFileName)
{
/*	char nisse[256];
	for(int i = 0; i< kHead.iNumOfTextures; i++) {
		sprintf(nisse, "../data/textures/%s.bmp", szFileName);
//		aiReplaceTextureIndex[i] = pkTextureManger->Load(nisse,0);
*/
}

void Mad_Core::SetUpBindPose()
{
	Matrix4		kMadkBoneMatrix;					
	
	unsigned int i;
	Vector3 Angles;

	for(i=0; i<m_kSkelleton.size(); i++) {
		Angles = m_kSkelleton[i].m_kRotation;
		g_kBoneTransform[i].m_kRot.AngleQuaternion(Angles);
		g_kBoneTransform[i].m_kPos = m_kSkelleton[i].m_kPosition;
		}

	Quaternion kTestQ;
	for (i = 0; i < m_kSkelleton.size(); i++) {
		kMadkBoneMatrix.Identity();
		kMadkBoneMatrix = g_kBoneTransform[i].m_kRot.Inverse();
		kMadkBoneMatrix.SetPos(g_kBoneTransform[i].m_kPos);

		if (m_kSkelleton[i].m_iParent == -1) {
			g_Madkbonetransform[i] = kMadkBoneMatrix;
		} 
	 	else {
			g_Madkbonetransform[i] = kMadkBoneMatrix * g_Madkbonetransform[m_kSkelleton[i].m_iParent];
		}
	}

	for (i = 0; i < m_kSkelleton.size(); i++) {
		m_MadkbonetransformI[i] = g_Madkbonetransform[i].Inverse();
		}	
}

void Mad_Core::SetupBonePose(BoneTransform* pkBones)
{
	unsigned int i;

//	for (i = 0; i < m_kSkelleton.size(); i++)	// pMad1
//		g_FullBoneTransform[i].Identity();

	if( iActiveAnimation == MAD_NOANIMINDEX) {
		return;
		}

	// N�TT CP CPCP CPCPPPPPPPPPPPP h�r...n�r en animation k�rs n�r maden sparas, och n�r man sedan laddar in den igen s� blir dessa tv� v�rden helt cp
	//DVOID FUL HACK...
	if(iStartFrame >= m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size())
		assert(0);
		//iStartFrame = 0;

	if(iEndFrame >= m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size())
		assert(0);
		//iEndFrame = 0;

	Vector3 Angles;

	Mad_CoreBoneKey* pkStartKey = &m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames[iStartFrame].m_kBonePose[0];
	Mad_CoreBoneKey* pkEndKey = &m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames[iEndFrame].m_kBonePose[0];
	
	ZFAssert(pkStartKey, "Mad_Core::SetupBonePose: No StartKey");
	ZFAssert(pkEndKey, "Mad_Core::SetupBonePose: No EndKey");

	Quaternion kStart, kEnd;

	float OneMinusFrameOffs = float(1.0) - fFrameOffs;

	int iBoneKeys = m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames[iStartFrame].m_kBonePose.size();

   //if ( iBoneKeys != m_kSkelleton.size() )
   //   return;
	
	for(i=0; i<m_kSkelleton.size(); i++) {
		// Get Start/End Keys
		//kStart.AngleQuaternion(pkStartKey[i].m_kRotation); 
		//kEnd.AngleQuaternion(pkEndKey[i].m_kRotation); 

		// Interp Keys
		//pkBones[i].m_kRot.QuaternionSlerp(&kStart, &kEnd, fFrameOffs);
		
		
		pkBones[i].m_kRot.QuaternionSlerp(&pkStartKey[i].m_kQuatRotation, &pkEndKey[i].m_kQuatRotation, fFrameOffs);
		pkBones[i].m_kPos = pkStartKey[i].m_kPosition * OneMinusFrameOffs + pkEndKey[i].m_kPosition * fFrameOffs;
		}
}

void Mad_Core::InterPolTransforms(BoneTransform* pkResultat, BoneTransform* pkFrom, BoneTransform* pkTo, float fDelta)
{
	Quaternion kStart, kEnd;
	float OneMinusFrameOffs = float(1.0) - fDelta;

	for(int i=0; i<m_kSkelleton.size(); i++) 
	{
		// Get Start/End Keys
		kStart = pkFrom[i].m_kRot;
		kEnd = pkTo[i].m_kRot;

		// Interp Keys
		pkResultat[i].m_kRot.QuaternionSlerp(&kStart, &kEnd, fDelta);
		pkResultat[i].m_kPos = pkFrom[i].m_kPos * OneMinusFrameOffs + pkTo[i].m_kPos * fDelta;
	}	
}

// VimRp2
void Mad_Core::GenerateBoneMatris(BoneTransform* pkBones, Matrix4* pkMatrix)
{
	Matrix4		kMadkBoneMatrix;					

	vector<int> kJointId = GetAllJointID();

	for (int i = 0; i < kJointId.size(); i++) 
	{
		int iId = kJointId[i];

		//kMadkBoneMatrix.Identity();	// pMad1
		kMadkBoneMatrix = pkBones[ iId ].m_kRot.Inverse();
		kMadkBoneMatrix.SetPos(pkBones[ iId ].m_kPos);

		if (m_kSkelleton[ iId ].m_iParent == -1) {
			g_Madkbonetransform[ iId ] = kMadkBoneMatrix;
		} 
		else {
			g_Madkbonetransform[ iId ] = kMadkBoneMatrix * g_Madkbonetransform[m_kSkelleton[ iId ].m_iParent];
		}

		pkMatrix[ iId ] = m_MadkbonetransformI[ iId ] * g_Madkbonetransform[ iId ];
	}

/*for (i = 0; i < m_kSkelleton.size(); i++) 
	{
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
*/
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

/*
void Mad_Core::SetAnimationTime( int iAnim, float fTime )
{
	fActiveAnimationTime = fTime;
	iActiveAnimation = iAnim;

	int iNumOfFrame = GetAnimationTimeInFrames(iActiveAnimation);
	int iFrame = int(fActiveAnimationTime / g_fMadFrameTime);
	fFrameOffs = (fActiveAnimationTime / g_fMadFrameTime) - iFrame;
	SetFrameI(m_kMesh[0].akAnimation[iActiveAnimation].KeyFrame[iFrame].iVertexFrame);
}*/

void Mad_Core::SetBoneAnimationTime(int iAnim, float fTime, bool bLoop)
{
	iActiveAnimation		= iAnim;
	fActiveAnimationTime = fTime;

	if(iActiveAnimation == -1) {
		iStartFrame = 0;
		iEndFrame = 0;
		fFrameOffs = 0.0;
		return;
		}

	int iNumOfFrame = m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size();
	
	// Calc Frame Anim is at as a float.
	float fFrame = fActiveAnimationTime / g_fMadFrameTime;

	// Get Real Frame before the one above.
	iStartFrame = int(fFrame);
	if(iStartFrame >= iNumOfFrame)
		iStartFrame = iNumOfFrame - 1;

	iEndFrame = iStartFrame + 1;
	if(iEndFrame >= iNumOfFrame) {
		if(bLoop) {
			iEndFrame = 0;
			}
		else {
			iEndFrame = iStartFrame;
			}
		}

	fFrameOffs = fFrame - iStartFrame;
}


void Mad_Core::LoadSkelleton(ZFVFile* pkZFile)
{
	Mad_CoreBone kNewBone;
	int iNumOfBones;
	pkZFile->Read(&iNumOfBones, 1, sizeof(int));

	for(int i=0; i<iNumOfBones; i++) {
		pkZFile->Read(&kNewBone, 1, sizeof(Mad_CoreBone));
		m_kSkelleton.push_back(kNewBone);
		}

	// fix for models without bones
	if ( !iNumOfBones )
	{
		kNewBone.Clear();
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
	kNewMesh.m_iMadVersion = kMadHeader.m_iVersionNum;
	kNewMesh.Load(pkZFile);
	m_kMesh.push_back(kNewMesh);
}


void Mad_Core::LoadSkelleton(const char* MadFileName)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string("test.sd"),0,true) ) {
		cout << "Failed to open " << "test.sd" << endl;
		return;
		}

	LoadSkelleton(&kZFile);
	kZFile.Close();
	SetUpBindPose();
}

void Mad_Core::LoadAnimation(const char* MadFileName)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string("test.sd"),0,true) ) {
		cout << "Failed to open " << "test.sd" << endl;
		return;
		}

	LoadAnimation(&kZFile);
	kZFile.Close();
}

void Mad_Core::LoadMesh(const char* MDFileName)
{
	ZFVFile kZFile;
	if( !kZFile.Open(string("test.sd"),0,true) ) {
		cout << "Failed to open " << "test.sd" << endl;
		return;
		}
	LoadMesh(&kZFile);
	kZFile.Close();
}


bool Mad_Core::Create(const string& strName)
{
	string MadFileName = strName;

	//setup data path
	if(MadFileName.find("data/mad/") == -1)
		MadFileName = "data/mad/"+MadFileName;
//  	else
//  		cerr<<"Warning: old resource path:"<<MadFileName<<endl;
	
	strcpy(Name,MadFileName.c_str());


	ZFVFile kZFile;
	if( !kZFile.Open(MadFileName,0,false) ) {
		cerr << "Warning: Failed to find" << MadFileName.c_str();
		cout << "\n";
		return false;
		}

//	Mad_Header kMadHeader;
	kZFile.Read(&kMadHeader,sizeof(Mad_Header), 1);
	
	// Check Versions Num
// 	if(kMadHeader.m_iVersionNum != MAD_VERSION) {
// 		cout << "Wrong version " << kMadHeader.m_iVersionNum << " in " << MadFileName << endl;
// 		}	

	LoadSkelleton(&kZFile);
	SetUpBindPose();

	unsigned int i;
	for(i=0; i<kMadHeader.m_iNumOfAnimations; i++)
		LoadAnimation(&kZFile);
		//LoadAnimation(MadFp);

	for(i=0; i<kMadHeader.m_iNumOfMeshes; i++)
		LoadMesh(&kZFile);

	kZFile.Close();

	if(m_kSkelleton.size() == 1) {
		for(i=0; i<m_kMesh.size(); i++)
			m_kMesh[i].bNotAnimated = true;
		}

	CalculateRadius();
	
	//PrintCoreInfo();
	
	m_bLoaded = true;
	
	return true;
}

int Mad_Core::CalculateSize()
{
	int iSizeInBytes = 0;
	iSizeInBytes += sizeof(Mad_Core);
	
	iSizeInBytes += sizeof(Mad_CoreBone) * m_kSkelleton.size();
	
	for(unsigned int iBa=0; iBa<m_kBoneAnim.size(); iBa++)
		iSizeInBytes += m_kBoneAnim[iBa].GetSizeInBytes();
	
	for(unsigned int iM=0; iM<m_kMesh.size(); iM++)
		iSizeInBytes += m_kMesh[iM].GetSizeInBytes();

	return iSizeInBytes;
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
	return &g_pkSelectedRawMesh->akFrames[0].akVertex[0];	// VimRP

	if(g_pkSelectedMesh->bNotAnimated)
		return &g_pkSelectedRawMesh->akFrames[0].akVertex[0];

	//dvoid fix
	if(iActiveAnimation == MAD_NOANIMINDEX)
		return &g_pkSelectedRawMesh->akFrames[0].akVertex[0];
		
	return g_TransformedVertex;
}

Vector3* Mad_Core::GetNormalsPtr()
{
	return &g_pkSelectedRawMesh->akFrames[0].akNormal[0];	// VimRP

	if(g_pkSelectedMesh->bNotAnimated)
		//return &g_pkSelectedMesh->GetLODMesh(0)->akFrames[0].akNormal[0];
		return &g_pkSelectedRawMesh->akFrames[0].akNormal[0];

	if(iActiveAnimation == MAD_NOANIMINDEX)
		return &g_pkSelectedRawMesh->akFrames[0].akNormal[0];
	
	
	return g_TransformedNormal;
}


int* Mad_Core::GetFacesPtr(Mad_CoreMesh* pkMesh,Mad_CoreSubMesh* pkSubMesh)
{
	return (int*) &g_pkSelectedRawMesh->akFaces[pkSubMesh->iFirstTriangle ].iIndex;	
		
}

Mad_TextureCoo* Mad_Core::GetTextureCooPtr(Mad_CoreMesh* pkMesh)
{
	return &g_pkSelectedRawMesh->akTextureCoo[0];
}

int Mad_Core::GetTextureID()
{
	return 0;
}

void Mad_Core::PrepareMesh(Mad_CoreMesh* pkMesh,Mad_RawMesh* pkRawMesh)
{
	g_pkSelectedMesh = pkMesh;
	g_pkSelectedRawMesh = pkRawMesh;

	if(iActiveAnimation == MAD_NOANIMINDEX)
		return;
	if(pkMesh->bNotAnimated)
		return;
	
	return;		// VimRp - Read below.
	/*
		The rest of the things in this function is what needs to be done in a render plugin.
		It should be possible to more or less copy this and plugin the pointers in the variable names.
	*/


/*	Vector3* pkVertexDst = g_TransformedVertex;
	Vector3* pkNormalDst = g_TransformedNormal;
		
	Vector3* pkVertex = &pkRawMesh->akFrames[0].akVertex[0];
	Vector3* pkNormal = &pkRawMesh->akFrames[0].akNormal[0];
 
	int* piBoneConnection = &pkRawMesh->akBoneConnections[0];
	Matrix4* pkMat;

	for(int i = 0; i<pkRawMesh->kHead.iNumOfVertex; i++) 
	{
		// p Mad 2
		// *pkVertexDst = g_FullBoneTransform[*piBoneConnection].VectorTransform( *pkVertex );
		// *pkNormalDst = g_FullBoneTransform[*piBoneConnection].VectorRotate( *pkNormal ); 

		// Calc vertex position.
		pkMat = &g_FullBoneTransform[*piBoneConnection];
		pkVertexDst->x = pkVertex->x * pkMat->RowCol[0][0] + pkVertex->y * pkMat->RowCol[1][0] + pkVertex->z * pkMat->RowCol[2][0] + pkMat->RowCol[3][0];
		pkVertexDst->y = pkVertex->x * pkMat->RowCol[0][1] + pkVertex->y * pkMat->RowCol[1][1] + pkVertex->z * pkMat->RowCol[2][1] + pkMat->RowCol[3][1];
		pkVertexDst->z = pkVertex->x * pkMat->RowCol[0][2] + pkVertex->y * pkMat->RowCol[1][2] + pkVertex->z * pkMat->RowCol[2][2] + pkMat->RowCol[3][2];
		
		// Calc normal for vertex.
		pkNormalDst->x = pkNormal->x * pkMat->RowCol[0][0] + pkNormal->y * pkMat->RowCol[1][0] + pkNormal->z * pkMat->RowCol[2][0];
		pkNormalDst->y = pkNormal->x * pkMat->RowCol[0][1] + pkNormal->y * pkMat->RowCol[1][1] + pkNormal->z * pkMat->RowCol[2][1];
		pkNormalDst->z = pkNormal->x * pkMat->RowCol[0][2] + pkNormal->y * pkMat->RowCol[1][2] + pkNormal->z * pkMat->RowCol[2][2];


		pkVertexDst++;
		pkNormalDst++;
		piBoneConnection++;
		pkVertex++;
		pkNormal++;
	}*/
}


unsigned int Mad_Core::GetNumOfBones()
{
	return m_kSkelleton.size();
}

Vector3 Mad_Core::GetBonePosition(int iBone)
{
	return g_Madkbonetransform[iBone].GetPos();
}

Matrix4 Mad_Core::GetBoneTransform(int iBone)
{
	return g_Madkbonetransform[iBone];
}


int Mad_Core::GetBoneParent(int iBone)
{
	return m_kSkelleton[iBone].m_iParent;
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
	cout << "Copy Mesh: " << kMesh.begin()->m_acName << endl;
	m_kMesh = kMesh;
	cout << "Copy Mesh: " << m_kMesh.begin()->m_acName << endl;
}

void Mad_Core::SetAnimation(vector<Mad_CoreBoneAnimation>	kBoneAnim)
{
	m_kBoneAnim = kBoneAnim;
} 

// Print All forms of info on this MAD.
void Mad_Core::PrintCoreInfo()
{
	// Skelleton
	cout << "Num Of Bones: " << m_kSkelleton.size() << endl;

	// Animation
	unsigned int i;
	cout << "Num Of Animations: " << m_kBoneAnim.size() << endl;
	for(i=0; i<m_kBoneAnim.size(); i++)
		cout << " [" << i << "]: " << m_kBoneAnim[i].m_szName << endl;

	// Mesh
	cout << "Num Of Mesh: " << m_kMesh.size() << endl;
	for(i=0; i<m_kMesh.size(); i++)
	{
		cout << " [" << i << "]: " << m_kMesh[i].m_acName << endl;
		m_kMesh[i].ShowInfo();
	}
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
	Vector3* pkVertex = &pkMesh->GetLODMesh(0)->akFrames[0].akVertex[0];		

	float fDist=0;

	Vector3 kMax(-999999,-999999,-999999);
	Vector3 kMin(999999,999999,999999);

	for(unsigned int i=0; i <pkMesh->GetLODMesh(0)->akFrames[0].akVertex.size(); i++) 
	{
		if(kMax.x < pkVertex[i].x) kMax.x = pkVertex[i].x;
		if(kMax.y < pkVertex[i].y) kMax.y = pkVertex[i].y;
		if(kMax.z < pkVertex[i].z) kMax.z = pkVertex[i].z;
		if(kMin.x > pkVertex[i].x) kMin.x = pkVertex[i].x;
		if(kMin.y > pkVertex[i].y) kMin.y = pkVertex[i].y;
		if(kMin.z > pkVertex[i].z) kMin.z = pkVertex[i].z;
		
		float newdist = pkVertex[i].Length();		
		
		if(newdist > fDist)
			fDist = newdist;
	}

	m_fSize = kMax.DistanceTo(kMin);
	m_fBoundRadius = fDist;
	//cout << "Bound Radius for '" << GetName() << "' is = " << fDist << endl;
}

float Mad_Core::GetRadius()
{
	return m_fBoundRadius;	// * float(0.01);	// // BM-NOTE: Exporter scale st�mmer inte �nnu.
}

int Mad_Core::GetJointID(const char* szJointName)
{
	for(unsigned int i=0; i<m_kSkelleton.size(); i++) {
		if(strcmp(m_kSkelleton[i].m_acName, szJointName) == 0) 
			return i;
		}

	return -1;
}

vector<int> Mad_Core::GetJointAndChildID(const char* szJointName)
{
	vector<int>	kRes;

	int iTopID = GetJointID(szJointName);
	if(iTopID == -1)
		return kRes;

	vector<int>	kCheck;
	kCheck.push_back(iTopID);

	while(kCheck.size())
	{
		int iCurrParent = kCheck.back();
		kCheck.pop_back();
		kRes.push_back(iCurrParent);
		g_ZFObjSys.Printf("%s", m_kSkelleton[iCurrParent].m_acName);

		for(unsigned int i=0; i<m_kSkelleton.size(); i++) 
		{
			if(m_kSkelleton[i].m_iParent == iCurrParent) 
			{
				kCheck.push_back(i);
			}
		}
	}

	return kRes;
}

vector<int> Mad_Core::GetJointSelection(const char* szJointNames)
{
	vector<int> kResultat;
	vector<int> kAdd;


	CmdArgument kCommand;
	kCommand.Set(szJointNames);

	for(int i=0; i<kCommand.m_kSplitCommand.size(); i++)
	{
		
		kAdd = GetJointAndChildID(kCommand.m_kSplitCommand[i].c_str());
		kResultat.insert(kResultat.end(), kAdd.begin(), kAdd.end());
	}

	return kResultat;
}

vector<int> Mad_Core::GetAllJointID()
{
	vector<int>	kRes;
	for(unsigned int i=0; i<m_kSkelleton.size(); i++) 
			kRes.push_back(i);
	return kRes;
}



Vector3 Mad_Core::GetJointPosition(char* szJointName)
{
	unsigned int i;

	if(szJointName) {
		for(i=0; i<m_kSkelleton.size(); i++) {

			if(strcmp(m_kSkelleton[i].m_acName, szJointName) == 0) {
				return Vector3::ZERO;	//g_FullBoneTransform[i].GetPos();
				}
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
/*	for(unsigned int i=0; i<m_kControllers.size(); i++) {
		if(strcmp(szName,m_kControllers[i].m_szName) == 0) {
			return;
			}
		}


	Controller	kNewControll;
	
	strcpy(kNewControll.m_szName, szName);
	strcpy(kNewControll.m_szJointName, szJoint);
	kNewControll.m_iJointID = GetJointID(szJoint);
	kNewControll.m_eAxis = eAxis;
	kNewControll.m_fMin = Math::DegToRad(fMin);
	kNewControll.m_fMax = Math::DegToRad(fMax);
	kNewControll.m_fValue = 0.0;

	if(kNewControll.m_iJointID == -1) {
		//cout << "Failed to add jointcontroller " << szName << "to joint " << szJoint << endl;
		}

	m_kControllers.push_back(kNewControll);
*/
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
