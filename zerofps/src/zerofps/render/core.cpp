#include "mad.h"
#include "../ogl/zfpsgl.h"
#include "GL/glut.h"

Core::Core()
{
	pakTextureCoo	= NULL;
	pakFrames		= NULL;
	pakFaces		= NULL;
	pkTextureManger = NULL;
	
	iActiveFrame = 0;
	iActiveKeyFrame = 0;
}

Core::~Core()
{
	if(pakTextureCoo)
		delete [] pakTextureCoo;
	if(pakFrames)
		delete [] pakFrames;
	if(pakFaces)
		delete [] pakFaces;
}

void Core::Load(const char* MadFileName)
{
	int i;

	strcpy(Name, MadFileName);

	FILE* fp = fopen(MadFileName, "rb");

	// Read head
	fread(&kHead,sizeof(Mad_Header),1,fp);

	// Read SubMesh
	pkSubMesh = new Mad_SubMesh [kHead.iNumOfSubMeshes];
	fread((void *)pkSubMesh,sizeof(Mad_SubMesh),kHead.iNumOfSubMeshes,fp);

	// Read textures
	fread((void *)akTextures,sizeof(Mad_Texture),kHead.iNumOfTextures,fp);

	// Read Texture Coo
	pakTextureCoo = new MadTextureCoo [kHead.iNumOfVertex];
	fread((void *)pakTextureCoo,sizeof(MadTextureCoo),kHead.iNumOfVertex,fp);

	// Write Alla vertex Frames.
	pakFrames = new Mad_VertexFrame [kHead.iNumOfFrames];
	for(i=0; i<kHead.iNumOfFrames; i++)
	{
		pakFrames[i].pVertex = new MadVertex [kHead.iNumOfVertex];
		fread(pakFrames[i].pVertex,sizeof(MadVertex),kHead.iNumOfVertex,fp);
	}

	// Read triangles.
	pakFaces = new MadFace [kHead.iNumOfFaces];
	fread(pakFaces,sizeof(MadFace),kHead.iNumOfFaces,fp);

	// Read Animations.
	int iNumOfAnimations;
	fread(&iNumOfAnimations,sizeof(int), 1 ,fp);

	Mad_Animation kNyAnim;
	Mad_KeyFrame kNyKey;

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
		aiTextureIndex[i] = pkTextureManger->Load(nisse,0);
	}

	fclose(fp);
}

void Core::SetFrameI(int iFrame)
{
	if(iFrame >= 0 && iFrame < kHead.iNumOfFrames)
		iActiveFrame = iFrame;
}

void Core::LoopPlayAnim(int iAnim)
{
	iActiveKeyFrame++;
	if(iActiveKeyFrame >= akAnimation[iAnim].KeyFrame.size())
		iActiveKeyFrame = 0;

	iActiveFrame = akAnimation[iAnim].KeyFrame[iActiveKeyFrame].iVertexFrame;
}


void Core::SetFrame_NormalizedTime(float fNormTime)
{
	
}

void Core::SetFrame_InSeconds(float fSeconds)
{

}

char* Core::GetName(void)
{
	return Name;
}

void Core::draw()
{
	glColor3f(1,1,1);
	
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );

	glDisable(GL_LIGHTING);

	glVertexPointer(3,GL_FLOAT,0,pakFrames[iActiveFrame].pVertex);
	glTexCoordPointer(2,GL_FLOAT,0,pakTextureCoo);
	
	int iMADTextureNum;
	int iTextureId = aiTextureIndex[0];

	glNormal3f(0,0,1);

	for(int i=0; i<this->kHead.iNumOfSubMeshes; i++)
	{
		iMADTextureNum = pkSubMesh[i].iTextureIndex;
		iTextureId = aiTextureIndex[ iMADTextureNum ];
		if(aiReplaceTextureIndex[ iMADTextureNum ] != NO_TEXTURE)
			iTextureId = aiReplaceTextureIndex[ iMADTextureNum ];

		pkTextureManger->BindTexture(iTextureId);
		glDrawElements(GL_TRIANGLES,
			pkSubMesh[i].iNumOfTriangles * 3,
			GL_UNSIGNED_INT,
			pakFaces[ pkSubMesh[i].iFirstTriangle ].iIndex);
	}

	glPopAttrib();
}

float Core::GetAnimationLengthInS(int iAnim)
{
	int iNumOfKeyFrames = akAnimation[iAnim].KeyFrame.size();
	return (iNumOfKeyFrames * 0.1);
}

int Core::GetAnimationTimeInFrames(int iAnim)
{
	return akAnimation[iAnim].KeyFrame.size();
}

void Core::SetReplaceTexture(char* szFileName)
{
	char nisse[256];
	for(int i = 0; i< kHead.iNumOfTextures; i++) {
		sprintf(nisse, "../data/textures/%s.bmp", szFileName);
		aiReplaceTextureIndex[i] = pkTextureManger->Load(nisse,0);
	}

}

void Core::LoadSkelleton(const char* MadFileName)
{
	FILE*	fp = fopen("test.sd","rb");

	Mad_Bone kNewBone;
	int iNumOfBones;
	fread(&iNumOfBones, 1, sizeof(int), fp);

	for(int i=0; i<iNumOfBones; i++) {
		fread(&kNewBone, 1, sizeof(Mad_Bone), fp);
		kSkelleton.push_back(kNewBone);
		}

	fclose(fp);

/*	Mad_Bone	newbone;

	newbone.iParent = -1;
	newbone.strName = "left_ankle";
	newbone.kBindPose.kPosition.Set(3.663815, -46.230229, -2.260499);
	newbone.kBindPose.kRotation.Set(-0.000000, 0.000000, -0.000000);
	kSkelleton.push_back(newbone);

	newbone.iParent = 0;
	newbone.strName = "left_ball";
	newbone.kBindPose.kPosition.Set(0.000000, -1.410943, 6.541643);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.000000);
	kSkelleton.push_back(newbone);

	newbone.iParent = 1;
	newbone.strName = "left_toe";
	newbone.kBindPose.kPosition.Set(0.000000, 0.384803, 2.821885);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.000000 );
	kSkelleton.push_back(newbone);

	newbone.iParent = -1;
	newbone.strName = "right_ankle";
	newbone.kBindPose.kPosition.Set(-3.863659, -46.230227, -2.260498);
	newbone.kBindPose.kRotation.Set(-0.000000, 0.000000, -0.000000 );
	kSkelleton.push_back(newbone);

	newbone.iParent = 3;
	newbone.strName = "right_ball";
	newbone.kBindPose.kPosition.Set(0.000000, -1.410900, 6.541640);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.000000);
	kSkelleton.push_back(newbone);

	newbone.iParent = 4;
	newbone.strName = "right_toe";
	newbone.kBindPose.kPosition.Set(0.000000, 0.384800, 2.821890);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.000000);
	kSkelleton.push_back(newbone);

	newbone.iParent = -1;
	newbone.strName = "back_root";
	newbone.kBindPose.kPosition.Set(0.000000, -14.810862, -1.872501);
	newbone.kBindPose.kRotation.Set(-1.570796, -0.620249, 1.570796);
	kSkelleton.push_back(newbone);

	newbone.iParent = 6;
	newbone.strName = "back_a";
	newbone.kBindPose.kPosition.Set(0.691626, -0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.334409 );
	kSkelleton.push_back(newbone);

	newbone.iParent = 7;
	newbone.strName = "back_b";
	newbone.kBindPose.kPosition.Set(3.204740, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.117843);
	kSkelleton.push_back(newbone);

	newbone.iParent = 8;
	newbone.strName = "back_c";
	newbone.kBindPose.kPosition.Set(3.913575, 0.000000, -0.000000);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.123647);
	kSkelleton.push_back(newbone);

	newbone.iParent = 9;
	newbone.strName = "back_d";
	newbone.kBindPose.kPosition.Set(3.366345, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.108668 );
	kSkelleton.push_back(newbone);

	newbone.iParent = 10;
	newbone.strName = "back_e";
	newbone.kBindPose.kPosition.Set(3.603846, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.092311);
	kSkelleton.push_back(newbone);

	newbone.iParent = 11;
	newbone.strName = "back_shoulderJoin";
	newbone.kBindPose.kPosition.Set(3.528364, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, -0.300676);
	kSkelleton.push_back(newbone);

	newbone.iParent = 12;
	newbone.strName = "back_neckJoin";
	newbone.kBindPose.kPosition.Set(2.575882, 0.000000, -0.000000);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.046964);
	kSkelleton.push_back(newbone);

	newbone.iParent = 13;
	newbone.strName = "neck_a";
	newbone.kBindPose.kPosition.Set(2.006386, -0.000000, -0.000000);
	newbone.kBindPose.kRotation.Set(3.141593, 0.000000, -0.017356);
	kSkelleton.push_back(newbone);

	newbone.iParent = 14;
	newbone.strName = "neck_b";
	newbone.kBindPose.kPosition.Set(2.510304, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(-3.141593, 0.000000, -0.127773);
	kSkelleton.push_back(newbone);

	newbone.iParent = 15;
	newbone.strName = "neck_c";
	newbone.kBindPose.kPosition.Set(2.150091, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(-3.141593, 0.000000, -0.746925);
	kSkelleton.push_back(newbone);

	newbone.iParent = 16;
	newbone.strName = "skull";
	newbone.kBindPose.kPosition.Set(5.480104, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(0.028297, 0.011085, -0.888757);
	kSkelleton.push_back(newbone);

	newbone.iParent = 15;
	newbone.strName = "jaw";
	newbone.kBindPose.kPosition.Set(0.040892, -1.233264, -0.000000);
	newbone.kBindPose.kRotation.Set(-0.000000, 3.141593, 1.254579 );
	kSkelleton.push_back(newbone);

	newbone.iParent = 18;
	newbone.strName = "chin";
	newbone.kBindPose.kPosition.Set(3.844148, 0.000000, -0.000000);
	newbone.kBindPose.kRotation.Set(0.000000, 3.141593, 1.254579);
	kSkelleton.push_back(newbone);

	newbone.iParent = 12;
	newbone.strName = "left_collar";
	newbone.kBindPose.kPosition.Set(0.076044, -0.034703, -3.000000 );
	newbone.kBindPose.kRotation.Set(-1.570796, 1.460139, 0.141799);
	kSkelleton.push_back(newbone);

	newbone.iParent = 20;
	newbone.strName = "left_shoulder";
	newbone.kBindPose.kPosition.Set(4.516650, 0.099387, -0.000225);
	newbone.kBindPose.kRotation.Set(-0.002249, -0.000055, 0.110657 );
	kSkelleton.push_back(newbone);

	newbone.iParent = 21;
	newbone.strName = "left_elbow";
	newbone.kBindPose.kPosition.Set(10.500000, -0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(-3.141593, 0.000032, 0.000000);
	kSkelleton.push_back(newbone);

	newbone.iParent = 22;
	newbone.strName = "left_forearm";
	newbone.kBindPose.kPosition.Set(6.000000, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.000000 );
	kSkelleton.push_back(newbone);

	newbone.iParent = 23;
	newbone.strName = "left_wrist";
	newbone.kBindPose.kPosition.Set(4.000000, 0.000000, 0.000000 );
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, 0.000000);
	kSkelleton.push_back(newbone);

	newbone.iParent = 24;
	newbone.strName = "left_hand_root";
	newbone.kBindPose.kPosition.Set(0.750000, -0.000000, -0.000299);
	newbone.kBindPose.kRotation.Set(1.570836, -0.000023, 0.000000);
	kSkelleton.push_back(newbone);

	newbone.iParent = 25;
	newbone.strName = "left_middle_palm";
	newbone.kBindPose.kPosition.Set(1.750000, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(-0.000040, 0.000004, 0.102896);
	kSkelleton.push_back(newbone);

	newbone.iParent = 26;
	newbone.strName = "left_middle1";
	newbone.kBindPose.kPosition.Set(1.153401, -0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(0.004229, -0.031508, -0.024069 );
	kSkelleton.push_back(newbone);

	newbone.iParent = 27;
	newbone.strName = "left_middle2";
	newbone.kBindPose.kPosition.Set(1.163146,-0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(-3.141593, -0.000000, -0.007328);
	kSkelleton.push_back(newbone);

	newbone.iParent = 28;
	newbone.strName = "left_middle3";
	newbone.kBindPose.kPosition.Set(0.906769, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(3.141593, 0.000000, -0.006920);
	kSkelleton.push_back(newbone);

	newbone.iParent = 29;
	newbone.strName = "left_middle4";
	newbone.kBindPose.kPosition.Set(0.806346, 0.000000, -0.000000);
	newbone.kBindPose.kRotation.Set(-1.570796, 0.000000, -0.066679);
	kSkelleton.push_back(newbone);

	newbone.iParent = 26;
	newbone.strName = "left_index1";
	newbone.kBindPose.kPosition.Set(0.868596, 0.767671, -0.000000);
	newbone.kBindPose.kRotation.Set(3.141593, 0.000000, 0.280507 );
	kSkelleton.push_back(newbone);

	newbone.iParent = 31;
	newbone.strName = "left_index2";
	newbone.kBindPose.kPosition.Set(1.175091, -0.000000, -0.000000);
	newbone.kBindPose.kRotation.Set(3.141593, 0.000000, 0.013797);
	kSkelleton.push_back(newbone);


	newbone.iParent = 32;
	newbone.strName = "left_index3";
	newbone.kBindPose.kPosition.Set(0.977500, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(3.141593, 0.000000, 0.009767);
	kSkelleton.push_back(newbone);

	newbone.iParent = 33;
	newbone.strName = "left_index4";
	newbone.kBindPose.kPosition.Set(0.629997, 0.000000, -0.000000);
	newbone.kBindPose.kRotation.Set(3.141593, -0.000000, 0.259953 );
	kSkelleton.push_back(newbone);

	newbone.iParent = 26;
	newbone.strName = "left_ring1";
	newbone.kBindPose.kPosition.Set(0.907535, -0.814006, 0.000000);
	newbone.kBindPose.kRotation.Set(-0.011744, -0.074705, -0.223939);
	kSkelleton.push_back(newbone);


	newbone.iParent = 35;
	newbone.strName = "left_ring2";
	newbone.kBindPose.kPosition.Set(0.979052, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(-0.000000, 0.000000, -0.005760  );
	kSkelleton.push_back(newbone);

	newbone.iParent = 36;
	newbone.strName = "left_ring3";
	newbone.kBindPose.kPosition.Set(0.912110, 0.000000, -0.000000);
	newbone.kBindPose.kRotation.Set(-0.000000, 0.000000, 0.002967);
	kSkelleton.push_back(newbone);

	
	newbone.iParent = 37;
	newbone.strName = "left_ring4";
	newbone.kBindPose.kPosition.Set(0.736078, 0.000000, -0.000000);
	newbone.kBindPose.kRotation.Set(-0.000000, -0.000000, 0.201838);
	kSkelleton.push_back(newbone);

	newbone.iParent = 25;
	newbone.strName = "left_pinky_palm";
	newbone.kBindPose.kPosition.Set(0.191243, -0.605827, 0.000024);
	newbone.kBindPose.kRotation.Set(3.141571, -0.000033, -0.986525);
	kSkelleton.push_back(newbone);

	
	newbone.iParent = 39;
	newbone.strName = "left_pinky_palm1";
	newbone.kBindPose.kPosition.Set(0.747740, 0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(3.141593, 0.000000, -0.820680);
	kSkelleton.push_back(newbone);

	newbone.iParent = 40;
	newbone.strName = "left_pinky1";
	newbone.kBindPose.kPosition.Set(1.774531, -0.000000, 0.000000);
	newbone.kBindPose.kRotation.Set(3.020771, -0.191691, -0.406225);
	kSkelleton.push_back(newbone);

	
	newbone.iParent = 41;
	newbone.strName = "left_pinky2";
	newbone.kBindPose.kPosition.Set(0.870738, -0.029962, 0.000000);
	newbone.kBindPose.kRotation.Set(0.000000, 0.000000, -0.026884 );
	kSkelleton.push_back(newbone);

	newbone.iParent = ;
	newbone.strName = "";
	newbone.kBindPose.kPosition.Set();
	newbone.kBindPose.kRotation.Set();
	kSkelleton.push_back(newbone);
*/
}

/* 0 joint1 -1  -0.023717 0.008030 0.000000  3.141593 0.000000 1.541102 
 1 joint2 0  1.303530 0.000000 0.000000  -3.141593 -0.000000 0.672710 
 2 joint3 1  1.535219 -0.000000 -0.000000  0.000000 0.000000 -2.197444 
*/
void Core::SetPosRot(int iChildBone)
{
	int iBoneID = iChildBone;
	vector<int>	BoneList;
	BoneList.push_back(iBoneID);

	while(kSkelleton[iBoneID].m_iParent != -1) {
		iBoneID = kSkelleton[iBoneID].m_iParent;
		BoneList.push_back(iBoneID);
	}

	//cout << "Bones for " << iChildBone << ": "; 
	while(BoneList.size() != 0) {
		iBoneID = BoneList[BoneList.size() - 1];
		BoneList.pop_back();
		//cout << iBoneID << "," ; 

		glColor3f(1,1,1);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(kSkelleton[iBoneID].kBindPose.kPosition.x, 
				kSkelleton[iBoneID].kBindPose.kPosition.y, 
				kSkelleton[iBoneID].kBindPose.kPosition.z);
		glEnd();
		
		glTranslatef(kSkelleton[iBoneID].kBindPose.kPosition.x, 
			kSkelleton[iBoneID].kBindPose.kPosition.y, 
			kSkelleton[iBoneID].kBindPose.kPosition.z);

		//if(kSkelleton[iBoneID].m_iParent != -1) {
			Vector3 HataSTL;
			HataSTL = kSkelleton[iBoneID].kBindPose.kRotation;
			HataSTL.x = RadToDeg(HataSTL.x);
			HataSTL.y = RadToDeg(HataSTL.y);
			HataSTL.z = RadToDeg(HataSTL.z);
			
			glRotatef(HataSTL.x, 1,0,0   );
			glRotatef(HataSTL.y, 0,1,0  );	
			glRotatef(HataSTL.z, 0,0,1  );	
		//}
		}
	//cout << endl;
}

void Core::DrawSkelleton(float x, float y, float z)
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	
	glMatrixMode(GL_MODELVIEW);

	glColor3f(1,1,1);
	glTranslatef(x,y + 10,z);
	glScalef(0.3,0.3,0.3);
	for(int i=0; i<kSkelleton.size(); i++) {
		glPushMatrix();
		SetPosRot(i);
		glutSolidSphere(0.1, 8, 8);
		glPopMatrix();

		}


 
	glPopAttrib();

}




void Core::ClearReplaceTexture(void)
{
	for(int i = 0; i< kHead.iNumOfTextures; i++)
		aiReplaceTextureIndex[i] = NO_TEXTURE;
}


void Mad_KeyFrame::Clear(void)
{
	iVertexFrame = 0;
	fFrameTime = 0;
}

void Mad_KeyFrame::operator=(const Mad_KeyFrame& kOther)
{
	iVertexFrame = kOther.iVertexFrame;
	fFrameTime = kOther.fFrameTime;
}

void Mad_Animation::Clear(void)
{
	strcpy(Name, "");
	KeyFrame.clear();
}

void Mad_Animation::operator=(const Mad_Animation& kOther)
{
	strcpy(Name, kOther.Name);
	KeyFrame = kOther.KeyFrame;
}
