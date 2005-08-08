#include "zocculusionquery.h"


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



