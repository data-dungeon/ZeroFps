#include "mad_core.h"

#define		MAX_BONES	256
Matrix4		g_Madkbonetransform[MAX_BONES];		// bone transformation matrix
Matrix4		g_MadkbonetransformI[MAX_BONES];	// Inverse bone transformation matrix for bind pose
Matrix4		g_FullBoneTransform[MAX_BONES];		
Vector3		g_Madpos[MAX_BONES];
Matrix4		g_MadkBoneMatrix;
Quaternion	g_Madq[MAX_BONES];

Vector3		g_akVertexBuffer[10000];
Vector3		g_akNormalBuffer[10000];
Vector3		g_TransformedVertex[10000];
Vector3		g_TransformedNormal[10000];

Mad_CoreMesh* g_pkSelectedMesh;

Mad_Core::Mad_Core()
{
	iActiveFrame = 0;
	iActiveKeyFrame = 0;
	m_bInterpolVertexFrames = false;
	m_bDrawNormals = false;
	iActiveAnimation = 0;
	iBoneFrame = 0;

}

Mad_Core::~Mad_Core()
{

}


void Mad_Core::Save_SD(FILE* pkFp)
{
	int iNumOfBones = m_kSkelleton.size();
	fwrite(&iNumOfBones, sizeof(int),1,pkFp);
	for(unsigned int i=0; i < m_kSkelleton.size(); i++) {
		fwrite(&m_kSkelleton[i],sizeof(Mad_CoreBone),1,pkFp);
	}
}

void Mad_Core::Save_MD(int iMeshId, FILE* pkFp)
{
	m_kMesh[iMeshId].Save(pkFp);
}

void Mad_Core::Save_AD(int iMeshId, FILE* pkFp)
{
	m_kBoneAnim[iMeshId].Save(pkFp);
}

void Mad_Core::Save_SD(const char* filename)
{
	FILE* fp = fopen(filename, "wb");
	Save_SD(fp);
	fclose(fp);
}

void Mad_Core::Save_AD(int iMeshId, const char* filename)
{
	FILE* fp = fopen(filename, "wb");
	Save_AD(iMeshId, fp);
	fclose(fp);
}


void Mad_Core::Save_MD(int iMeshId, const char* filename)
{
	FILE* MadFp = fopen(filename, "wb");
	Save_MD(iMeshId, MadFp);
	fclose(MadFp);
	
}

void Mad_Core::Save_MAD(const char* filename)
{
	int i;

	PrintCoreInfo();

//	m_kMesh[0].CreateVertexNormals();

	FILE* MadFp = fopen(filename, "wb");
	if(!MadFp) {
		cout << "Failed to open '" << filename << "' for writing" << endl;
		return;
		}

	Mad_Header m_kMadHeader;
	// Setup MAD Header.
	m_kMadHeader.m_iVersionNum		= MAD_VERSION;
	m_kMadHeader.m_iNumOfMeshes		= m_kMesh.size();
	m_kMadHeader.m_iNumOfAnimations = m_kBoneAnim.size();
		
	// Write MAD Header.
	fwrite(&m_kMadHeader,sizeof(Mad_Header), 1, MadFp);
	
	// Write SD
	Save_SD(MadFp);

	// Write AD
	for(i=0; i<m_kMadHeader.m_iNumOfAnimations; i++)
		Save_AD(i, MadFp);

	// Write MD
	for(i=0; i<m_kMadHeader.m_iNumOfMeshes; i++)
		Save_MD(i, MadFp);

	// Write MA

	fclose(MadFp);

}

 

void Mad_Core::SetFrameI(int iFrame)
{
	if(iFrame >= 0 && iFrame < m_kMesh[0].kHead.iNumOfFrames)
		iActiveFrame = iFrame;
}

void Mad_Core::LoopPlayAnim(int iAnim)
{
	iActiveKeyFrame++;
	if(iActiveKeyFrame >= m_kMesh[0].akAnimation[iAnim].KeyFrame.size())
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
	return (iNumOfKeyFrames * 0.1);
/*	int iNumOfKeyFrames = m_kMesh[0].akAnimation[iAnim].KeyFrame.size();
	return (iNumOfKeyFrames * 0.1);
	return 0;*/
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
	}*/

}



void InversTransformMatrix(Matrix4& pkIn, Matrix4& pkOut)
{
	pkOut = pkIn;
	pkOut = pkIn.Invert2();

//	pkOut = pkIn;

/*	swap(pkOut.RowCol[0][1], pkOut.RowCol[1][0]);
	swap(pkOut.RowCol[0][2], pkOut.RowCol[2][0]);
	swap(pkOut.RowCol[1][2], pkOut.RowCol[2][1]);

	Vector3 Pos = pkIn.GetPos();
	Pos = -Pos;
	pkOut.SetPos(Pos);*/

/*	cout << "Inverse: "<< endl;
	pkIn.Print();
	pkOut.Print();
	Matrix4 res = pkOut;
	res *= pkIn;
	res.SetZeroDelta(0.001 );
	res.Print(); 
	cout << "End Inverse: "<< endl;*/
}

void Mad_Core::SetUpBindPose()
{
	cout << "Setup bind pose" << endl;

	unsigned int i;
	Vector3 Angles;

	for(i=0; i<m_kSkelleton.size(); i++) {
		Angles = m_kSkelleton[i].m_kRotation;
		g_Madq[i].AngleQuaternion(Angles);
		g_Madpos[i] = m_kSkelleton[i].m_kPosition;
		}

	Quaternion kTestQ;
	for (i = 0; i < m_kSkelleton.size(); i++) {
		g_MadkBoneMatrix.Identity();
		g_MadkBoneMatrix = g_Madq[i].Inverse();
		g_MadkBoneMatrix.SetPos(g_Madpos[i]);

		if (m_kSkelleton[i].m_iParent == -1) {
			g_Madkbonetransform[i] = g_MadkBoneMatrix;
		} 
		else {
			g_Madkbonetransform[i] = g_MadkBoneMatrix * g_Madkbonetransform[m_kSkelleton[i].m_iParent];
		}
	}

	for (i = 0; i < m_kSkelleton.size(); i++) {
		cout << "Invert " << m_kSkelleton[i].m_acName << endl ;
		InversTransformMatrix(g_Madkbonetransform[i],g_MadkbonetransformI[i]);
		}	

/*	Matrix4 a;
	Matrix4 b;
	a.Identity();
	a.Translate(456.4643455469,345346.654654,234346.564654);
	InversTransformMatrix(a,b);*/
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
	unsigned int i;
	Vector3 Angles;

//	cout << "Size: " << m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size() << endl;

	int iNumOfFrame = m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size();
	int iStartFrame = int(fActiveAnimationTime / 0.1);
	int iEndFrame = iStartFrame + 1;
	if(iEndFrame >= iNumOfFrame) 
		iEndFrame = 0;

	//if(iBoneFrame >= m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size() )
	//	iBoneFrame = 0;

	Mad_CoreBoneKey* pkStartKey = &m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames[iStartFrame].m_kBonePose[0];
	Mad_CoreBoneKey* pkEndKey = &m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames[iEndFrame].m_kBonePose[0];
	
	Quaternion kStart, kEnd;

	float OneMinusFrameOffs = 1.0 - fFrameOffs;

	for(i=0; i<m_kSkelleton.size(); i++) {
//		Angles = m_kSkelleton[i].m_kRotation;
//		g_Madq[i].AngleQuaternion(Angles);
//		g_Madpos[i] = m_kSkelleton[i].m_kPosition;
	
		//g_Madq[i].AngleQuaternion( pkStartKey[i].m_kRotation);
		kStart.AngleQuaternion(pkStartKey[i].m_kRotation); 
		kEnd.AngleQuaternion(pkEndKey[i].m_kRotation); 
		g_Madq[i].QuaternionSlerp(&kStart, &kEnd, fFrameOffs);
		 
		//g_Madpos[i] = pkStartKey[i].m_kPosition;
		g_Madpos[i] = pkStartKey[i].m_kPosition * OneMinusFrameOffs +
			pkEndKey[i].m_kPosition * fFrameOffs;
		}



	Quaternion kTestQ;
	for (i = 0; i < m_kSkelleton.size(); i++) {
		g_MadkBoneMatrix.Identity();
		g_MadkBoneMatrix = g_Madq[i].Inverse();
		g_MadkBoneMatrix.SetPos(g_Madpos[i]);

		if (m_kSkelleton[i].m_iParent == -1) {
			g_Madkbonetransform[i] = g_MadkBoneMatrix;
		} 
		else {
			g_Madkbonetransform[i] = g_MadkBoneMatrix * g_Madkbonetransform[m_kSkelleton[i].m_iParent];
		}
	}

	Matrix4 kRes;

	for (i = 0; i < m_kSkelleton.size(); i++) {
		kRes = g_MadkbonetransformI[i];
		kRes = kRes * g_Madkbonetransform[i];
//		kRes *= g_Madkbonetransform[i];
		//kRes.Print();
//		kRes.Identity();
		kRes.SetZeroDelta(0.001 );
		g_FullBoneTransform[i] = kRes;
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

//	return m_kMesh[0].akAnimation.size();
}

void Mad_Core::SetAnimationTime( int iAnim, float fTime )
{
	fActiveAnimationTime = fTime;
	iActiveAnimation = iAnim;

	int iNumOfFrame = GetAnimationTimeInFrames(iActiveAnimation);
	int iFrame = int(fActiveAnimationTime / 0.1);
	fFrameOffs = (fActiveAnimationTime / 0.1) - iFrame;
	SetFrameI(m_kMesh[0].akAnimation[iActiveAnimation].KeyFrame[iFrame].iVertexFrame);

}

void Mad_Core::SetBoneAnimationTime(int iAnim, float fTime )
{
	fActiveAnimationTime = fTime;
	iActiveAnimation = iAnim;

	int iNumOfFrame = m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size();
	int iFrame = int(fActiveAnimationTime / 0.1);
	fFrameOffs = (fActiveAnimationTime / 0.1) - iFrame;
	iBoneFrame = iFrame;
	if(iBoneFrame >= m_kBoneAnim[iActiveAnimation].m_kBoneKeyFrames.size() )
		iBoneFrame = 0;
}


void Mad_Core::LoadSkelleton(FILE* pkFp)
{
	Mad_CoreBone kNewBone;
	int iNumOfBones;
	fread(&iNumOfBones, 1, sizeof(int), pkFp);

	for(int i=0; i<iNumOfBones; i++) {
		fread(&kNewBone, 1, sizeof(Mad_CoreBone), pkFp);
		m_kSkelleton.push_back(kNewBone);
		}
}

void Mad_Core::LoadAnimation(FILE* pkFp)
{
	Mad_CoreBoneAnimation NewBoneAnim;
	NewBoneAnim.Clear();
	NewBoneAnim.Load(pkFp);
	m_kBoneAnim.push_back(NewBoneAnim);
}

void Mad_Core::LoadMesh(FILE* pkFp)
{
	Mad_CoreMesh	kNewMesh;
	kNewMesh.Load(pkFp);
	m_kMesh.push_back(kNewMesh);

	// Load Textures
//	char nisse[256];
	Mad_CoreMesh* pkMesh = &m_kMesh[0];

/*		bool	bIsAlphaTest;				// True if needs alpha test.
	bool	bTwoSided;					// True if two sided.
	bool	bClampTexture;
*/

/*	for(int i=0; i<pkMesh->kHead.iNumOfTextures; i++) {
		pkMesh->akTextures[i].bIsAlphaTest = false;
		pkMesh->akTextures[i].bTwoSided = false;
		pkMesh->akTextures[i].bClampTexture = false;
		glGenTextures(1, &TexturesID[i]);	
		 
		img.free();
		sprintf(nisse, "%s.tga", kNewMesh.akTextures[i].ucTextureName);
		img.load(nisse);
		if(img.bHasAlpha) {
			pkMesh->akTextures[i].bIsAlphaTest = true;
			pkMesh->akTextures[i].bTwoSided = true;
			pkMesh->akTextures[i].bClampTexture = true;
			}

		img.MapColorToAlpha(0.9,0.9,0.9,1.0);
		img.save("alptest.tga", true);
		glBindTexture( GL_TEXTURE_2D, TexturesID[i] );		

		glTexImage2D(GL_TEXTURE_2D, 0, 4, img.width, img.height, 0, GL_RGBA, 
			GL_UNSIGNED_BYTE, img.pixels );

		GLenum daerror = glGetError();
		if(daerror != GL_NO_ERROR) {
			cout << "Error is" << GetGLErrorString(daerror) << endl;
			}
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if(pkMesh->akTextures[i].bClampTexture) {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_CLAMP);
			}
		
		}		*/
}


/*		bool	bIsAlphaTest;				// True if needs alpha test.
	bool	bTwoSided;					// True if two sided.
	bool	bClampTexture;
*/

void Mad_Core::LoadSkelleton(const char* MadFileName)
{
	FILE*	fp = fopen("test.sd","rb");
	LoadSkelleton(fp);
	fclose(fp);
	SetUpBindPose();
}

void Mad_Core::LoadAnimation(const char* MadFileName)
{
	FILE*	fp = fopen("test.ad","rb");
	LoadAnimation(fp);
	fclose(fp);
}

void Mad_Core::LoadMesh(const char* MDFileName)
{
	FILE* fp = fopen(MDFileName, "rb");
	LoadMesh(fp);
	fclose(fp);
}



void Mad_Core::LoadMad(const char* MadFileName)
{
	strcpy(Name,MadFileName);

	cout << "LoadMad: " << MadFileName << endl;
	FILE* MadFp = fopen(MadFileName, "rb");

	Mad_Header kMadHeader;
	fread(&kMadHeader,sizeof(Mad_Header), 1, MadFp);

	cout << "m_iVersionNum: " << kMadHeader.m_iVersionNum << endl;
	cout << "m_iNumOfMeshes: " << kMadHeader.m_iNumOfMeshes << endl;
	cout << "m_iNumOfAnimations: " << kMadHeader.m_iNumOfAnimations << endl;

	LoadSkelleton(MadFp);
	SetUpBindPose();

	int i;
	for(i=0; i<kMadHeader.m_iNumOfAnimations; i++)
		LoadAnimation(MadFp);

	for(i=0; i<kMadHeader.m_iNumOfMeshes; i++)
		LoadMesh(MadFp);

	fclose(MadFp);
	PrintCoreInfo();

	if(m_kSkelleton.size() == 1) {
		for(i=0; i<m_kMesh.size(); i++)
			m_kMesh[i].bNotAnimated = true;
		}
}

Mad_CoreMesh* Mad_Core::GetMeshByID(int iMesh)
{
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
	g_pkSelectedMesh = pkMesh;
	if(pkMesh->bNotAnimated)
		return;
	
	//	return;
	Matrix4 kFullTransform;

	for(int i = 0; i<pkMesh->kHead.iNumOfVertex; i++) {
		/*kFullTransform = g_MadkbonetransformI[0];
		kFullTransform = g_Madkbonetransform[pkMesh->akBoneConnections[i]] 
		g_MadkbonetransformI[pkMesh->akBoneConnections[i]];
		g_TransformedVertex[i] = pkMesh->akFrames[0].akVertex[i];*/

		kFullTransform.Identity();
		kFullTransform = g_FullBoneTransform[ pkMesh->akBoneConnections[i] ];
		g_TransformedVertex[i] = kFullTransform.VectorTransform(pkMesh->akFrames[0].akVertex[i]);
		g_TransformedNormal[i] = kFullTransform.VectorRotate(pkMesh->akFrames[0].akNormal[i]);
		
		//g_TransformedVertex[i] = pkMesh->akFrames[0].akVertex[i];
		//g_TransformedNormal[i] = pkMesh->akFrames[0].akNormal[i];
		}
}

int Mad_Core::GetNumOfBones()
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
	int i;
	cout << "Num Of Animations: " << m_kBoneAnim.size() << endl;
	for(i=0; i<m_kBoneAnim.size(); i++)
		cout << " [" << i << "]: " << m_kBoneAnim[i].m_szName << endl;

	cout << "Num Of Mesh: " << m_kMesh.size() << endl;
	for(i=0; i<m_kMesh.size(); i++)
		cout << " [" << i << "]: " << m_kMesh[i].kHead.iNumOfSubMeshes << endl;

	cout << "Num Of Bones: " << m_kSkelleton.size() << endl;


}
