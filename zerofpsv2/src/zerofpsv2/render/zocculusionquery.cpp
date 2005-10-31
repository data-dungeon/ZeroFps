#include "zocculusionquery.h"
#include "../basic/zfsubsystem.h"

ZOcculusionQuery::ZOcculusionQuery()
{
	//generate one occulusion query
	glGenQueriesARB(1,&m_iQueryID);

	m_bIsQuerying = false;
}

ZOcculusionQuery::~ZOcculusionQuery()
{
	glDeleteQueriesARB(1,&m_iQueryID);
}

void ZOcculusionQuery::Begin()
{
	if(m_bIsQuerying)
		return;

	m_bIsQuerying = true;
	glBeginQueryARB(GL_SAMPLES_PASSED_ARB, m_iQueryID);
}

void ZOcculusionQuery::End()
{
	if(!m_bIsQuerying)
		return;

	m_bIsQuerying = false;
	glEndQueryARB(GL_SAMPLES_PASSED_ARB);
}


bool ZOcculusionQuery::HaveResult()
{
	GLint iReady;
	glGetQueryObjectivARB(m_iQueryID, GL_QUERY_RESULT_AVAILABLE_ARB,&iReady);	

	return iReady;
}

GLuint ZOcculusionQuery::GetResult()
{
	GLuint iResult;
	glGetQueryObjectuivARB(m_iQueryID, GL_QUERY_RESULT_ARB,&iResult);
	
	return iResult;
}




//occulusion test
ZOcculusionTest::ZOcculusionTest()
{
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));	
	m_bHaveTested = false;
	m_bVisible = true;
}

bool ZOcculusionTest::Visible(const Vector3& kMin,const Vector3& kMax)
{
	if(m_bHaveTested && HaveResult())
	{
		m_bHaveTested = false;
		m_bVisible = GetResult() > 10;					
	}
	else
	{
		m_bHaveTested = true;
		Test(kMin,kMax);	
	}

	return m_bVisible;
}

void ZOcculusionTest::DrawOcculusionBox(const Vector3& kMin,const Vector3& kMax)
{
	static ZMaterial* pkMaterial = NULL;
	if(!pkMaterial)
	{
		pkMaterial = new ZMaterial;
		pkMaterial->GetPass(0)->m_iCullFace = CULL_FACE_NONE;	
		pkMaterial->GetPass(0)->m_bLighting = false;
		pkMaterial->GetPass(0)->m_bFog = false;
		pkMaterial->GetPass(0)->m_bDepthMask = false;
	}

	static Vector3 kVertises[24];
	
	
	//botom
	kVertises[0] = kMin;
	kVertises[1].Set(kMax.x,kMin.y,kMin.z);
	kVertises[2].Set(kMax.x,kMin.y,kMax.z);
	kVertises[3].Set(kMin.x,kMin.y,kMax.z);
	
	//top
	kVertises[4] = kMax;
	kVertises[5].Set(kMax.x,kMax.y,kMin.z);
	kVertises[6].Set(kMin.x,kMax.y,kMin.z);
	kVertises[7].Set(kMin.x,kMax.y,kMax.z);

	//front
	kVertises[8] = kMin;	
	kVertises[9].Set(kMin.x,kMax.y,kMin.z);
	kVertises[10].Set(kMax.x,kMax.y,kMin.z);
	kVertises[11].Set(kMax.x,kMin.y,kMin.z);

	//left
	kVertises[12] = kMin;
	kVertises[13].Set(kMin.x,kMin.y,kMax.z);
	kVertises[14].Set(kMin.x,kMax.y,kMax.z);
	kVertises[15].Set(kMin.x,kMax.y,kMin.z);

	//right
	kVertises[16] = kMax;
	kVertises[17].Set(kMax.x,kMin.y,kMax.z);
	kVertises[18].Set(kMax.x,kMin.y,kMin.z);
	kVertises[19].Set(kMax.x,kMax.y,kMin.z);
	
	//back
	kVertises[20] = kMax;
	kVertises[21].Set(kMin.x,kMax.y,kMax.z);
	kVertises[22].Set(kMin.x,kMin.y,kMax.z);
	kVertises[23].Set(kMax.x,kMin.y,kMax.z);	


	//disable color writes 
	m_pkZShaderSystem->ForceColorMask(0);		

	//disable all glsl stuff
	m_pkZShaderSystem->SetForceDisableGLSL(true);	

	//set materail
	m_pkZShaderSystem->BindMaterial(pkMaterial);	

	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,kVertises);
	m_pkZShaderSystem->SetNrOfVertexs(24);
		
	m_pkZShaderSystem->DrawArray(QUADS_MODE);
	
	//reenable all glsl stuff
	m_pkZShaderSystem->SetForceDisableGLSL(false);

	//disable color writes 
	m_pkZShaderSystem->ForceColorMask(-1);		

}

void ZOcculusionTest::Test(const Vector3& kMin,const Vector3& kMax)
{
	//start occulusion query
	Begin();

	//draw occulusion box
	DrawOcculusionBox(kMin,kMax);
	
	//end occulusion query
	End();
}