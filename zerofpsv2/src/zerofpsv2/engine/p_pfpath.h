#ifndef _ENGINE_P_PFPATH_H_
#define _ENGINE_P_PFPATH_H_

#include "property.h"
#include "engine_x.h"
#include "zerofps.h"
#include "astar.h"

using namespace std;

class ENGINE_API P_PfPath : public Property 
{
	private:
		ZeroFps*				m_pkFps;			
		AStar*				m_pkAStar;
		
		vector<Vector3>	m_kPath;				// The path we are following (if any).
		int					m_iNextGoal;		// The point we are going to.		
		float					m_fSpeed;			// Our walking speed.
		int					m_iNavMeshCell;	// The cell we are in. 0 if unknown.

		Vector3				m_kOffset;
		bool					m_bHaveOffset;
		
		void					SetupOffset();

	public:
		P_PfPath();
		~P_PfPath();

		void CloneOf(Property* pkProperty) { }
		void Update();

		void Save(ZFIoInterface* pkFile);
		void Load(ZFIoInterface* pkFile);

		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		void SetPath(vector<Vector3> kPath);

		bool MakePathFind(Vector3 kDestination);
		bool HavePath();		

	protected:
		vector<PropertyValues> GetPropertyValues();
};

Property* Create_P_PfPath();



#endif