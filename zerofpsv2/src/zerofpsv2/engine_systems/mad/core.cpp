#include "mad_core.h"
#include "../../basic/quaternion.h"
#include "../../basic/zfassert.h"

Matrix4		g_Madkbonetransform[MAX_BONES];		// Bone transformation matrix
Matrix4		g_FullBoneTransform[MAX_BONES];		// Bone transformation matrix.
Quaternion	g_Madq[MAX_BONES];					// Quat angle for bone.
Vector3		g_Madpos[MAX_BONES];				// Position for bone.
Vector3		g_TransformedVertex[10000];			// Transformed Skinvertex Position.
Vector3		g_TransformedNormal[10000];			// Transformed Skinvertex Normals.

#define		MAD_FPS		15						// Def FPS for Mad's.
float const g_fMadFrameTime = 1.0 / MAD_FPS;	// Def time between frames in Mad.

Mad_CoreMesh* g_pkSelectedMesh;
//Mad_RawMesh*  g_pkSelectedRawMesh;

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
		m_MadkbonetransformI[i] = g_Madkbonetransform[i].Inverse();
		}	
}

void Mad_Core::SetupBonePose()
{
	unsigned int i;

	if( iActiveAnimation == MAD_NOANIMINDEX) {
		for (i = 0; i < m_kSkelleton.size(); i++)
			g_FullBoneTransform[i].Identity();
	
		return;
		}

	// NÅTT CP CPCP CPCPPPPPPPPPPPP här...när en animation körs när maden sparas, och när man sedan laddar in den igen så blir dessa två värden helt cp
	//DVOID FUL HACK...
	if(iStartFrame >= m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size())
		iStartFrame = 0;

	if(iEndFrame >= m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size())
		iEndFrame = 0;
		
		
	Matrix4		kMadkBoneMatrix;					

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
		kStart.AngleQuaternion(pkStartKey[i].m_kRotation); 
		kEnd.AngleQuaternion(pkEndKey[i].m_kRotation); 

		// Interp Keys
		g_Madq[i].QuaternionSlerp(&kStart, &kEnd, fFrameOffs);

			g_Madpos[i] = pkStartKey[i].m_kPosition * OneMinusFrameOffs + pkEndKey[i].m_kPosition * fFrameOffs;
		}
	
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


bool Mad_Core::Create(string MadFileName)
{
	strcpy(Name,MadFileName.c_str());

	if(strcmp(Name, "/data/mad/goblin.mad") == 0) {
		int gam = 1;
	}

	ZFVFile kZFile;
	if( !kZFile.Open(MadFileName,0,false) ) {
		cout << "Failed to find" << MadFileName.c_str();
		cout << "\n";
		return false;
		}

	Mad_Header kMadHeader;
	kZFile.Read(&kMadHeader,sizeof(Mad_Header), 1);
	
	// Check Versions Num
	if(kMadHeader.m_iVersionNum != MAD_VERSION) {
		cout << "Wrong version " << kMadHeader.m_iVersionNum << " in " << MadFileName << endl;
		}	

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
	if(g_pkSelectedMesh->bNotAnimated)
		return &g_pkSelectedMesh->GetLODMesh(0)->akFrames[0].akVertex[0];

	return g_TransformedVertex;
}

Vector3* Mad_Core::GetNormalsPtr()
{
	if(g_pkSelectedMesh->bNotAnimated)
		return &g_pkSelectedMesh->GetLODMesh(0)->akFrames[0].akNormal[0];

	return g_TransformedNormal;
}


int* Mad_Core::GetFacesPtr(Mad_CoreMesh* pkMesh,Mad_CoreSubMesh* pkSubMesh)
{
	return (int*) &pkMesh->GetLODMesh(0)->akFaces[pkSubMesh->iFirstTriangle ].iIndex;	
		
}

Mad_TextureCoo* Mad_Core::GetTextureCooPtr(Mad_CoreMesh* pkMesh)
{
	return &pkMesh->GetLODMesh(0)->akTextureCoo[0];
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
	//g_pkSelectedRawMesh = pkMesh->GetLODMesh(0);

	if(pkMesh->bNotAnimated)
		return;
	
	int* piBoneConnection;
	Vector3* pkVertex = &g_pkSelectedMesh->GetLODMesh(0)->akFrames[0].akVertex[0];
	Vector3* pkNormal = &g_pkSelectedMesh->GetLODMesh(0)->akFrames[0].akNormal[0];
 
	pkVertexDst = g_TransformedVertex;
	pkNormalDst = g_TransformedNormal;
	piBoneConnection = &g_pkSelectedMesh->GetLODMesh(0)->akBoneConnections[0];

	for(int i = 0; i<g_pkSelectedMesh->GetLODMesh(0)->kHead.iNumOfVertex; i++) {
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

Matrix4 Mad_Core::GetBoneTransform(int iBone)
{
	return g_Madkbonetransform[iBone];
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
		cout << " [" << i << "]: " << m_kMesh[i].m_acName << endl;
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

	for(unsigned int i=0; i <pkMesh->GetLODMesh(0)->akFrames[0].akVertex.size(); i++) 
	{
		float newdist = pkVertex[i].Length();		
		
		if(newdist > fDist)
			fDist = newdist;


	}

	m_fBoundRadius = fDist;
	//cout << "Bound Radius for '" << GetName() << "' is = " << fDist << endl;
}

float Mad_Core::GetRadius()
{
	return m_fBoundRadius;	// * float(0.01);	// // BM-NOTE: Exporter scale stämmer inte ännu.
}

int Mad_Core::GetJointID(const char* szJointName)
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

			if(strcmp(m_kSkelleton[i].m_acName, szJointName) == 0) {
				return g_FullBoneTransform[i].GetPosVector();
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
		//cout << "Failed to add jointcontroller " << szName << "to joint " << szJoint << endl;
		}

	m_kControllers.push_back(kNewControll);
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
