#include "p_pfmesh.h"
#include "object.h"

P_PfMesh::P_PfMesh()
{

}

P_PfMesh::~P_PfMesh()
{

}

void P_PfMesh::Update()
{

}

void P_PfMesh::Save(ZFIoInterface* pkFile)
{

}

void P_PfMesh::Load(ZFIoInterface* pkFile)
{

}

void P_PfMesh::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{

}

void P_PfMesh::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{

}

vector<PropertyValues> P_PfMesh::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
	return kReturn;
}


Property* Create_P_PfMesh()
{
	return new P_PfMesh;
}

