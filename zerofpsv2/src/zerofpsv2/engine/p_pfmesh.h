#ifndef _ENGINE_P_PFMESH_H_
#define _ENGINE_P_PFMESH_H_

#include "property.h"
#include "engine_x.h"

using namespace std;

class ENGINE_API P_PfMesh : public Property 
{
	private:
		vector<Vector3>	m_kPath;
		int					m_iNextGoal;

	public:
		P_PfMesh();
		~P_PfMesh();

		void CloneOf(Property* pkProperty) { }
		void Update();

		void Save(ZFIoInterface* pkFile);
		void Load(ZFIoInterface* pkFile);

		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		void SetPath(vector<Vector3> kPath);

	protected:
		vector<PropertyValues> GetPropertyValues();
};

Property* Create_P_PfMesh();



#endif