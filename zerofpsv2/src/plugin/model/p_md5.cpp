#pragma warning(disable : 4275)		
#include "p_md5.h"

#define MD2TOMAD_SCALE 1

P_Md5::P_Md5()
{
	strcpy(m_acName,"P_Md5");		
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_bNetwork = true;	
	m_iVersion = 2;
	m_iSortPlace=4;
	
	m_pkTextureManger =	static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));				
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));		
	LoadMesh("md5/mpplayer.md5mesh");

	ZSSVFileSystem* fs = static_cast<ZSSVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));

	char* ext;
	vector<string> kstrFileNames;
	fs->ListDir(&kstrFileNames, "md5/");
	string FullName;
	for(int i=0; i<kstrFileNames.size(); i++)
	{
		ext = strstr(kstrFileNames[i].c_str(), ".md5anim");
		if(ext)
		{
			FullName = "md5/" + kstrFileNames[i];
			cout << "Should load : " << FullName << endl;
			LoadAnimation(FullName);
		}

	}

	m_iPlayAnim = 0;
	m_iRenderFrame = 0;
	m_fLastFrame = m_fNextFrame = 0;
}

P_Md5::~P_Md5()
{

}


void P_Md5::DrawBone(Vector3 From, Vector3 To, Vector3 Color)
{
	glDisable (GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	
	glPointSize (3.0f);
	glColor3f (Color.x,Color.y,Color.z);
	glBegin (GL_LINES);
		glVertex3f(From.x,From.y,From.z);
		glVertex3f(To.x,To.y,To.z);
	glEnd ();

	glColor3f (0, 0, 0.8f);	
	glBegin (GL_POINTS);
	glVertex3f(From.x,From.y,From.z);
	glVertex3f(To.x,To.y,To.z);
	glEnd ();

	glPointSize (1.0f);
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
}

void P_Md5::SetBindPose()
{
	for(unsigned int i=0; i<m_iNumOfJoints; i++) 
	{
		m_pkBone[i].m_kPosition = m_pkJoint[i].m_kPosition;	
		m_pkBone[i].kQuat = m_pkJoint[i].kQuat;	
	}
}

void P_Md5::DrawSkelleton()
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	
	Vector3 Position;

	ZSSRender* pkRender = static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));

	glColor3f(1,1,1);
	glPointSize (12.0f);
	glColor3f (0.8f, 0, 0);

	for(unsigned int i=0; i<m_iNumOfJoints; i++) 
	{
		if (m_pkJoint[i].m_iParentIndex >= 0) 
		{
			DrawBone(m_pkBone[m_pkJoint[i].m_iParentIndex].m_kPosition,
				m_pkBone[i].m_kPosition,Vector3(1, 0.7f, 0));
		}
		else 
		{
			// Draw marker for parent bone.
			glPointSize (5.0f);
			glColor3f (0.8f, 0, 0);
			glBegin (GL_POINTS);
			Position = m_pkBone[i].m_kPosition;
				glVertex3f(Position.x,Position.y,Position.z);
			glEnd ();
		}
	}
 
	glPopAttrib();
}

void P_Md5::DrawSkelleton_old()
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	
	Vector3 Position;

	ZSSRender* pkRender = static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));

	glColor3f(1,1,1);
	glPointSize (12.0f);
	glColor3f (0.8f, 0, 0);

	for(unsigned int i=0; i<m_iNumOfJoints; i++) 
	{
		if (m_pkJoint[i].m_iParentIndex >= 0) 
		{
			Matrix4 kMat;
			m_pkJoint[ m_pkJoint[i].m_iParentIndex ].kQuat.ToRotationMatrix( kMat );
			kMat.SetPos( m_pkJoint[ m_pkJoint[i].m_iParentIndex ].m_kPosition );

				m_pkBone[i].m_kPosition = kMat.VectorTransform( m_pkJoint[ i ].m_kPosition  );
			m_pkBone[i].kQuat *= m_pkBone[m_pkJoint[i].m_iParentIndex].kQuat;

			DrawBone(m_pkJoint[m_pkJoint[i].m_iParentIndex].m_kPosition,
				m_pkJoint[i].m_kPosition,Vector3(1, 0.7f, 0));
		}
		else 
		{
			m_pkBone[i].m_kPosition = m_pkJoint[i].m_kPosition;
			m_pkBone[i].kQuat = m_pkJoint[i].kQuat;

			// Draw marker for parent bone.
			glPointSize (5.0f);
			glColor3f (0.8f, 0, 0);
			glBegin (GL_POINTS);
			Position = m_pkBone[i].m_kPosition;
				glVertex3f(Position.x,Position.y,Position.z);
			glEnd ();
		}

		//Position = m_pkJoint[i].m_kPosition;
		//glVertex3f(Position.x,Position.y,Position.z);
	}
 
	glPopAttrib();
}

void P_Md5::DrawSkin()
{

}

void P_Md5::SetAnimFrame(int iFrame)
{
	int	iCompIndex = 0;
	for(unsigned int i=0; i<m_iNumOfJoints; i++) 
	{
		int	iParent = m_pkJoint[i].m_iParentIndex;

		Vector3 kPosition		= m_kPlayingAnim->m_pkAnimBaseFrame[i].m_kPosition;
		Quaternion kQuat		= m_kPlayingAnim->m_pkAnimBaseFrame[i].kQuat;
		int iBitField			= m_kPlayingAnim->m_pkAnimJoints[i].m_iAnimFlags;

		if(iBitField != 0)
		{
			if(iBitField & 1)		kPosition.x = m_kPlayingAnim->m_pkFrame[iFrame].m_fAnimComponents[iCompIndex++];	
			if(iBitField & 2)		kPosition.y = m_kPlayingAnim->m_pkFrame[iFrame].m_fAnimComponents[iCompIndex++];	
			if(iBitField & 4)		kPosition.z = m_kPlayingAnim->m_pkFrame[iFrame].m_fAnimComponents[iCompIndex++];	
			if(iBitField & 8)		kQuat.x = m_kPlayingAnim->m_pkFrame[iFrame].m_fAnimComponents[iCompIndex++];	
			if(iBitField & 16)	kQuat.y = m_kPlayingAnim->m_pkFrame[iFrame].m_fAnimComponents[iCompIndex++];	
			if(iBitField & 32)	kQuat.z = m_kPlayingAnim->m_pkFrame[iFrame].m_fAnimComponents[iCompIndex++];	
		}

		float term = 1.0f - (kQuat.x*kQuat.x) - 
			(kQuat.y*kQuat.y) - 
			(kQuat.z*kQuat.z);
		float qw;
		if (term < 0.0f)
			qw = 0.0f;
		else
			qw = (float) sqrt(term);
		kQuat.w = qw;

		if (iParent >= 0) 
		{
			Matrix4 kMat;
			m_pkBone[ iParent ].kQuat.ToRotationMatrix( kMat );
			kMat.SetPos( m_pkBone[ iParent ].m_kPosition );

			m_pkBone[i].m_kPosition = kMat.VectorTransform( kPosition  );
			m_pkBone[i].kQuat = kQuat* m_pkBone[ iParent ].kQuat;
		}
		else 
		{
			m_pkBone[i].m_kPosition = kPosition;
			m_pkBone[i].kQuat = kQuat;
		}	
	}

	int iSmurf = 2;
}

void P_Md5::SetBaseFrame()
{
	for(unsigned int i=0; i<m_iNumOfJoints; i++) 
	{
		int	iParent = m_pkJoint[i].m_iParentIndex;

		if (iParent >= 0) 
		{
			Matrix4 kMat;
			m_pkBone[ iParent ].kQuat.ToRotationMatrix( kMat );
			kMat.SetPos( m_pkBone[ iParent ].m_kPosition );

			m_pkBone[i].m_kPosition = kMat.VectorTransform( m_kPlayingAnim->m_pkAnimBaseFrame[ i ].m_kPosition  );
			m_pkBone[i].kQuat = m_kPlayingAnim->m_pkAnimBaseFrame[i].kQuat * m_pkBone[ iParent ].kQuat;
		}
		else 
		{
			m_pkBone[i].m_kPosition = m_kPlayingAnim->m_pkAnimBaseFrame[i].m_kPosition;
			m_pkBone[i].kQuat = m_kPlayingAnim->m_pkAnimBaseFrame[i].kQuat;
		}	
	}

	int iSmurf = 2;
}


void P_Md5::DrawBindPose()
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glEnable(GL_TEXTURE_2D);
	glColor3f(1,1,1);

	m_pkTextureManger->BindTexture( m_iTexture );

	for(int iV = 0; iV < m_iNumOfVertex; iV++)
	{
		Vector3 kVertex = Vector3::ZERO;
		Vector3 kV;

		// Loopa alla vikter och addera
		for(int iW = m_pkVertex[iV].m_iViktIndex; iW < (m_pkVertex[iV].m_iViktIndex + m_pkVertex[iV].m_iNumOfVikter); iW++ )
		{
			Matrix4 kMat;
			kMat.Identity();
			m_pkBone[m_pkWeight[iW].m_iBone].kQuat.ToRotationMatrix( kMat );
			kMat.SetPos( m_pkBone[m_pkWeight[iW].m_iBone].m_kPosition );
			//kMat.VectorTransform( m_pkJoint[m_pkWeight[iW].m_iBone].m_kPosition );
			
			kV = kMat.VectorTransform( m_pkWeight[iW].m_kPosition );
			kVertex += (kV * m_pkWeight[iW].m_fWeight);
		}

		m_pkCalcVertex[iV] = kVertex;
	}

	Vector3 kVertex;
	glBegin(GL_TRIANGLES);
	for(int iT = 0; iT < m_iNumOfTriangles; iT++)
	{
		kVertex = m_pkCalcVertex[m_pkTriangle[iT].m_iIndex[2]];
      glTexCoord2f( m_pkVertex[m_pkTriangle[iT].m_iIndex[2]].m_s, m_pkVertex[m_pkTriangle[iT].m_iIndex[2]].m_t );
		glVertex3f(kVertex.x,kVertex.y,kVertex.z);

		kVertex = m_pkCalcVertex[m_pkTriangle[iT].m_iIndex[1]];
      glTexCoord2f( m_pkVertex[m_pkTriangle[iT].m_iIndex[1]].m_s, m_pkVertex[m_pkTriangle[iT].m_iIndex[1]].m_t );
		glVertex3f(kVertex.x,kVertex.y,kVertex.z);

		 kVertex = m_pkCalcVertex[m_pkTriangle[iT].m_iIndex[0]];
      glTexCoord2f( m_pkVertex[m_pkTriangle[iT].m_iIndex[0]].m_s, m_pkVertex[m_pkTriangle[iT].m_iIndex[0]].m_t );
		glVertex3f(kVertex.x,kVertex.y,kVertex.z);
	}
	glEnd();

	glPopAttrib();
}


void P_Md5::Update()
{
	m_kPlayingAnim = m_pkAnim[ m_iPlayAnim ];
	
	float fDelta = 0;

	float fCurrentTime = m_pkEntityManager->GetSimTime();
	if(fCurrentTime >= m_fNextFrame)
	{
		m_iRenderFrame++;
		m_fNextFrame = fCurrentTime + (1.0 / m_kPlayingAnim->m_iFrameRate);
		if(m_iRenderFrame >= m_kPlayingAnim->m_iNumOfFrames)
		{
			m_iPlayAnim++;
			if(m_iPlayAnim >= m_pkAnim.size())
				m_iPlayAnim = 0;

			m_iRenderFrame = 0;
		}
		m_fLastFrame = fCurrentTime;

	}
	else
	{
		fDelta = (fCurrentTime - m_fLastFrame) * m_kPlayingAnim->m_iFrameRate;
	}


	Matrix3 kRot;
	kRot = m_pkEntity->GetWorldRotM();		
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetIWorldPosV());
	m_pkZShaderSystem->MatrixMult(Matrix4(kRot));
	m_pkZShaderSystem->MatrixMult(Matrix4(kRot));
	m_pkZShaderSystem->MatrixScale(0.02);									

	SetAnimFrame(m_iRenderFrame);
	DrawSkelleton();
	DrawBindPose();
	m_pkZShaderSystem->MatrixPop();
}

vector<PropertyValues> P_Md5::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
/*	kReturn[0].kValueName = "m_kMadFile";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_strFileName;

	kReturn[1].kValueName = "scale";
	kReturn[1].iValueType = VALUETYPE_FLOAT;
	kReturn[1].pkValue    = (void*)&m_fScale;*/
		
	return kReturn;
}

bool P_Md5::HandleSetValue( const string& kValueName ,const string& kValue )
{
/*	if(strcmp(kValueName.c_str(), "m_kMadFile") == 0) 
	{
		LoadModel(kValue);
		return true;
	}*/
	
	return false;
}


void P_Md5::Save(ZFIoInterface* pkPackage)
{
//	pkPackage->Write(m_fSize);
}

void P_Md5::Load(ZFIoInterface* pkPackage,int iVersion)
{
}


void P_Md5::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
//	pkNetPacket->Write(m_fSize);
	
//	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_Md5::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
//	pkNetPacket->Read(m_fSize);
}

void P_Md5::HandleMeshFileCommand(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	char* ucpToken;

	if (!strcmp (ucpTokenCmd, "MD5Version"))
	{
		ucpToken = pkScript->GetToken();
		int iVersion = atof(ucpToken);
		cout << "Md5 Version: " << iVersion << endl;
		return;	
	}

	if (!strcmp (ucpTokenCmd, "commandline"))
	{
		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "numJoints"))
	{
		ucpToken = pkScript->GetToken();
		m_iNumOfJoints = atof(ucpToken);
		m_pkJoint = new MD5Joint[m_iNumOfJoints];
		m_pkBone = new MD5AnimBone[m_iNumOfJoints];
		return;	
	}

	if (!strcmp (ucpTokenCmd, "numMeshes"))
	{
		ucpToken = pkScript->GetToken();
		m_iNumOfMesh = atof(ucpToken);
		return;	
	}

	if (!strcmp (ucpTokenCmd, "joints"))
	{
		ReadJoints(pkScript, ucpTokenCmd);
		return;	
	}

	if (!strcmp (ucpTokenCmd, "mesh"))
	{
		ReadMeshes(pkScript, ucpTokenCmd);
		return;	
	}

}


void P_Md5::ReadMeshes(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	char* ucpToken;
	ucpToken = pkScript->GetToken();		// Read away {

	ucpToken = pkScript->GetToken();	
	while(strcmp(ucpToken, "}"))
	{
		if (!strcmp (ucpTokenCmd, "shader"))
		{
			ucpToken = pkScript->GetToken();
			cout << "Shader: " << ucpToken << endl;
		}

		if (!strcmp (ucpTokenCmd, "numverts"))
		{
			ucpToken = pkScript->GetToken();
			m_iNumOfVertex = atof(ucpToken);
			m_pkVertex = new MD5Vertex [m_iNumOfVertex];
			m_pkCalcVertex = new Vector3 [m_iNumOfVertex];
			
			cout << "Num of vertex: " << m_iNumOfVertex << endl;
		}

		if (!strcmp (ucpTokenCmd, "vert"))
		{
			ucpToken = pkScript->GetToken();
			int iIndex = atof(ucpToken);
			
			// Read S,T
			ucpToken = pkScript->GetToken();	// Read (
			ucpToken = pkScript->GetToken();
			m_pkVertex[iIndex].m_s = atof(ucpToken);
			ucpToken = pkScript->GetToken();
			m_pkVertex[iIndex].m_t = atof(ucpToken);
			ucpToken = pkScript->GetToken();	// Read )

			// Read vikter
			ucpToken = pkScript->GetToken();
			m_pkVertex[iIndex].m_iViktIndex = atof(ucpToken);
			ucpToken = pkScript->GetToken();
			m_pkVertex[iIndex].m_iNumOfVikter = atof(ucpToken);
		}

		if (!strcmp (ucpTokenCmd, "numtris"))
		{
			ucpToken = pkScript->GetToken();
			m_iNumOfTriangles = atof(ucpToken);
			m_pkTriangle = new MD5Triangle [m_iNumOfTriangles];
			cout << "Num of tri: " << m_iNumOfTriangles << endl;
		}

		if (!strcmp (ucpTokenCmd, "tri"))
		{
			ucpToken = pkScript->GetToken();
			int iTriIndex = atof(ucpToken);
			ucpToken = pkScript->GetToken();
			m_pkTriangle[iTriIndex].m_iIndex[0] = atof(ucpToken);
			ucpToken = pkScript->GetToken();
			m_pkTriangle[iTriIndex].m_iIndex[1] = atof(ucpToken);
			ucpToken = pkScript->GetToken();
			m_pkTriangle[iTriIndex].m_iIndex[2] = atof(ucpToken);
		}

		if (!strcmp (ucpTokenCmd, "numweights"))
		{
			ucpToken = pkScript->GetToken();
			m_iNumOfWeight = atof(ucpToken);
			m_pkWeight = new MD5Weight [m_iNumOfWeight];
			cout << "Num of vikter: " << m_iNumOfWeight << endl;
		}

		if (!strcmp (ucpTokenCmd, "weight"))
		{
			ucpToken = pkScript->GetToken();
			int iIndex = atof(ucpToken);

			ucpToken = pkScript->GetToken();
			m_pkWeight[iIndex].m_iBone = atof(ucpToken);
			
			ucpToken = pkScript->GetToken();
			m_pkWeight[iIndex].m_fWeight = atof(ucpToken);

			ucpToken = pkScript->GetToken();	// Read (
			ucpToken = pkScript->GetToken();
			m_pkWeight[iIndex].m_kPosition.x = atof(ucpToken);
			ucpToken = pkScript->GetToken();
			m_pkWeight[iIndex].m_kPosition.y = atof(ucpToken);
			ucpToken = pkScript->GetToken();
			m_pkWeight[iIndex].m_kPosition.z = atof(ucpToken);
			ucpToken = pkScript->GetToken();	// Read )
		}


		ucpToken = pkScript->GetToken();	// Read Joint Name
	}
}

void P_Md5::ReadJoints(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	int iJointIndex = 0;
	char* ucpToken;
	ucpToken = pkScript->GetToken();	// Read away {

	ucpToken = pkScript->GetToken();	
	while(strcmp(ucpToken, "}"))
	{
		// Read name
		m_pkJoint[iJointIndex].m_strName = ucpToken;

		// Read Parent
		ucpToken = pkScript->GetToken();
		m_pkJoint[iJointIndex].m_iParentIndex = atof(ucpToken);

		// Read position.
		ucpToken = pkScript->GetToken();	// Read (
		ucpToken = pkScript->GetToken();
		m_pkJoint[iJointIndex].m_kPosition.x = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_pkJoint[iJointIndex].m_kPosition.y = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_pkJoint[iJointIndex].m_kPosition.z = atof(ucpToken);
		ucpToken = pkScript->GetToken();	// Read )

		// Read quaternion
		ucpToken = pkScript->GetToken();	// Read (
		ucpToken = pkScript->GetToken();
		m_pkJoint[iJointIndex].kQuat.x = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_pkJoint[iJointIndex].kQuat.y = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_pkJoint[iJointIndex].kQuat.z = atof(ucpToken);
		ucpToken = pkScript->GetToken();	// Read )

		float term = 1.0f - (m_pkJoint[iJointIndex].kQuat.x*m_pkJoint[iJointIndex].kQuat.x) - 
			(m_pkJoint[iJointIndex].kQuat.y*m_pkJoint[iJointIndex].kQuat.y) - 
			(m_pkJoint[iJointIndex].kQuat.z*m_pkJoint[iJointIndex].kQuat.z);
		float qw;
		if (term < 0.0f)
			qw = 0.0f;
		else
			qw = (float) sqrt(term);
		m_pkJoint[iJointIndex].kQuat.w = qw;



		pkScript->SkipLine();

		iJointIndex++;
		ucpToken = pkScript->GetToken();	
	}
}


void P_Md5::LoadMesh(string strFileName)
{
	m_strFileName = strFileName;

	SimpleScriptFile kScript;
	if(!kScript.LoadScript(strFileName.c_str()))
	{
		cout << "No script found" << endl;
		return;
	}
	cout << "Script found" << endl;

	char* ucpToken;
	ucpToken = kScript.GetToken();

	while(ucpToken)
	{
		HandleMeshFileCommand(&kScript, ucpToken);
		ucpToken = kScript.GetToken();
	}

	cout << "Joints: " << m_iNumOfJoints << endl;
	cout << "Meshs: " << m_iNumOfMesh << endl;

	char szTextureName[256];
	strcpy(szTextureName, m_strFileName.c_str());
	char* ext = strstr(szTextureName, ".md5mesh");
	if(ext)
		strcpy(ext, ".tga");


	m_iTexture = m_pkTextureManger->Load(szTextureName);
	m_pkTextureManger->BindTexture( m_iTexture );
}

	/*
	char* ucpToken;
	ucpToken = pkScript->GetToken();	// Read away {

	ucpToken = pkScript->GetToken();	
	while(strcmp(ucpToken, "}"))
	{
		ucpToken = pkScript->GetToken();	
	}
	*/


void P_Md5::ReadAnim_Hierarchy(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	int iJointIndex = 0;
	char* ucpToken;
	ucpToken = pkScript->GetToken();	// Read away {

	ucpToken = pkScript->GetToken();	
	while(strcmp(ucpToken, "}"))
	{
		// Read name
		m_kAnim->m_pkAnimJoints[iJointIndex].m_strName = ucpToken;

		// Read Parent
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimJoints[iJointIndex].m_iParentIndex = atof(ucpToken);

		// Read Flags
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimJoints[iJointIndex].m_iAnimFlags = atof(ucpToken);

		// Read index
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimJoints[iJointIndex].m_iAnimCompIndex = atof(ucpToken);

		iJointIndex++;
		pkScript->SkipLine();
		ucpToken = pkScript->GetToken();	
	}
}

void P_Md5::ReadAnim_Bounds(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	int	iBoundIndex = 0;
	char* ucpToken;
	ucpToken = pkScript->GetToken();	// Read away {

	ucpToken = pkScript->GetToken();	// Read (

	while(strcmp(ucpToken, "}"))
	{
		// Read Min.
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBounds[iBoundIndex].m_kMin.x = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBounds[iBoundIndex].m_kMin.y = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBounds[iBoundIndex].m_kMin.z = atof(ucpToken);
		ucpToken = pkScript->GetToken();	// Read )
		
		// Read Max.
		ucpToken = pkScript->GetToken();	// Read (
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBounds[iBoundIndex].m_kMax.x = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBounds[iBoundIndex].m_kMax.y = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBounds[iBoundIndex].m_kMax.z = atof(ucpToken);
		ucpToken = pkScript->GetToken();	// Read )

		iBoundIndex++;
		ucpToken = pkScript->GetToken();	
	}
}


void P_Md5::ReadAnim_BaseFrame(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	int iJointIndex = 0;
	char* ucpToken;
	ucpToken = pkScript->GetToken();	// Read away {

	ucpToken = pkScript->GetToken();	// Read (

	while(strcmp(ucpToken, "}"))
	{
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBaseFrame[iJointIndex].m_kPosition.x = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBaseFrame[iJointIndex].m_kPosition.y = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBaseFrame[iJointIndex].m_kPosition.z = atof(ucpToken);
		ucpToken = pkScript->GetToken();	// Read )

		ucpToken = pkScript->GetToken();	// Read (
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBaseFrame[iJointIndex].kQuat.x = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBaseFrame[iJointIndex].kQuat.y = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		m_kAnim->m_pkAnimBaseFrame[iJointIndex].kQuat.z = atof(ucpToken);
		ucpToken = pkScript->GetToken();	// Read )

		float term = 1.0f - (m_kAnim->m_pkAnimBaseFrame[iJointIndex].kQuat.x*m_kAnim->m_pkAnimBaseFrame[iJointIndex].kQuat.x) - 
			(m_kAnim->m_pkAnimBaseFrame[iJointIndex].kQuat.y*m_kAnim->m_pkAnimBaseFrame[iJointIndex].kQuat.y) - 
			(m_kAnim->m_pkAnimBaseFrame[iJointIndex].kQuat.z*m_kAnim->m_pkAnimBaseFrame[iJointIndex].kQuat.z);
		float qw;
		if (term < 0.0f)
			qw = 0.0f;
		else
			qw = (float) sqrt(term);
		m_kAnim->m_pkAnimBaseFrame[iJointIndex].kQuat.w = qw;
		

		iJointIndex++;
		ucpToken = pkScript->GetToken();	
	}
}

void P_Md5::ReadAnim_Frame(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	int	iFrame;
	int	iCompIndex = 0;

	char* ucpToken;
	ucpToken = pkScript->GetToken();
	iFrame = atof(ucpToken);

	m_kAnim->m_pkFrame[iFrame].m_fAnimComponents = new float [ m_kAnim->m_iNumOfAnimComponents ];

	ucpToken = pkScript->GetToken();	// Read away {

	ucpToken = pkScript->GetToken();	
	while(strcmp(ucpToken, "}"))
	{
		m_kAnim->m_pkFrame[iFrame].m_fAnimComponents[iCompIndex] =  atof(ucpToken);
		
		iCompIndex++;
		ucpToken = pkScript->GetToken();	
	}

	if(iCompIndex != m_kAnim->m_iNumOfAnimComponents)
	{
		cout << "ARGHHHHH: " << iCompIndex << "," << m_kAnim->m_iNumOfAnimComponents << endl;
	}
}


void P_Md5::HandleAnimFileCommand(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	char* ucpToken;

	if (!strcmp (ucpTokenCmd, "MD5Version"))
	{
		ucpToken = pkScript->GetToken();
		int iVersion = atof(ucpToken);
		cout << "Md5 Version: " << iVersion << endl;
		return;	
	}

	if (!strcmp (ucpTokenCmd, "commandline"))
	{
		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "numFrames"))
	{
		ucpToken = pkScript->GetToken();
		m_kAnim->m_iNumOfFrames = atof(ucpToken);
		m_kAnim->m_pkAnimBounds = new MD5Bounds[m_kAnim->m_iNumOfFrames];
		m_kAnim->m_pkFrame = new MD5Frame [m_kAnim->m_iNumOfFrames] ;
		return;	
	}

	if (!strcmp (ucpTokenCmd, "numJoints"))
	{
		ucpToken = pkScript->GetToken();
		m_kAnim->m_iNumOfJoints = atof(ucpToken);
		m_kAnim->m_pkAnimJoints = new MD5AnimJoint [m_kAnim->m_iNumOfJoints];
		m_kAnim->m_pkAnimBaseFrame = new MD5AnimBone [m_kAnim->m_iNumOfJoints];
		return;	
	}

	if (!strcmp (ucpTokenCmd, "frameRate"))
	{
		ucpToken = pkScript->GetToken();
		m_kAnim->m_iFrameRate = atof(ucpToken);
		return;	
	}

	if (!strcmp (ucpTokenCmd, "numAnimatedComponents"))
	{
		ucpToken = pkScript->GetToken();
		m_kAnim->m_iNumOfAnimComponents = atof(ucpToken);
		return;	
	}

	if (!strcmp (ucpTokenCmd, "hierarchy"))
	{
		ReadAnim_Hierarchy(pkScript, ucpTokenCmd);
		return;	
	}

	if (!strcmp (ucpTokenCmd, "bounds"))
	{
		ReadAnim_Bounds(pkScript, ucpTokenCmd);
		return;	
	}

	if (!strcmp (ucpTokenCmd, "baseframe"))
	{
		ReadAnim_BaseFrame(pkScript, ucpTokenCmd);
		return;	
	}

	if (!strcmp (ucpTokenCmd, "frame"))
	{
		ReadAnim_Frame(pkScript, ucpTokenCmd);
		return;	
	}
}

void P_Md5::LoadAnimation(string strFileName)
{
	m_kAnim = new MD5Anim;
	SimpleScriptFile kScript;
	if(!kScript.LoadScript(strFileName.c_str()))
	{
		cout << "No anim script found" << endl;
		return;
	}
	cout << "Script found" << endl;

	char* ucpToken;
	ucpToken = kScript.GetToken();

	while(ucpToken)
	{
		HandleAnimFileCommand(&kScript, ucpToken);
		ucpToken = kScript.GetToken();
	}

	m_kPlayingAnim = m_kAnim;
	m_pkAnim.push_back(m_kPlayingAnim);

	cout << "Anim.m_iFrameRate: " << this->m_kAnim->m_iFrameRate << endl;
	cout << "Anim.m_iNumOfAnimComponents: " << this->m_kAnim->m_iNumOfAnimComponents << endl;
	cout << "Anim.m_iNumOfFrames: " << this->m_kAnim->m_iNumOfFrames << endl;
	cout << "Anim.m_iNumOfJoints: " << this->m_kAnim->m_iNumOfJoints << endl;
}



Property* Create_P_Md5()
{
	return new P_Md5;
}

void Register_P_Md5(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Md5", Create_P_Md5);					

	// Register Property Script Interface
}
