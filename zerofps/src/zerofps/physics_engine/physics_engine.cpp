#include "physics_engine.h"

Physics_Engine::Physics_Engine() 
: ZFObject("Physics_Engine")
{
	Clear();
}

void Physics_Engine::Clear()
{
	m_kBodys.clear();
}

void Physics_Engine::Update(float fTime)
{	
	m_fCurrentTime = fTime;
}

void Physics_Engine::AddBody(Body* pkBody)
{
	m_kBodys.push_back(pkBody);
}

void Physics_Engine::RemoveBody(Body* pkBody)
{
	m_kBodys.remove(pkBody);
}







