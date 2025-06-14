#ifndef _ENGINE_P_PFPATH_H_
#define _ENGINE_P_PFPATH_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../../engine/zerofps.h"
#include "../common/astar.h"

using namespace std;

/** \brief	Gives a entity ability to follow a path of points. 
	 \ingroup Engine
*/
class ENGINE_API P_PfPath : public Property 
{
	private:
		ZSSZeroFps*			m_pkFps;				
		ZSSAStar*			m_pkAStar;
		ZSSRender*			m_pkRender;
		
		vector<Vector3>	m_kPath;				// The path we are following (if any).
		vector<PathNode>	m_kRawPath;			// The path unoptimized path.
		int					m_iNextGoal;		// The point we are going to.		
		float					m_fSpeed;			// Our walking speed.
		bool					m_bTilt;				// Shuld the character tilt when walking (used for 4-leged creatures)
		int					m_iNavMeshCell;	// The cell we are in. 0 if unknown.

		Vector3				m_kOffset;			
		bool					m_bHaveOffset;		
		
		void					SetupOffset();
		string				m_kRunAnim;
		string				m_kIdleAnim;

	public:
		P_PfPath();
		~P_PfPath();
	
		void Init();

		void Update();

		void Save(ZFIoInterface* pkFile);
		void Load(ZFIoInterface* pkFile,int iVersion);

		void SetPath(vector<Vector3> kPath);
		bool MakePathFind(Vector3 kDestination);
		bool HavePath();				
	
		void RenderPath();

		void SetSpeed(float fSpeed)	{	m_fSpeed = fSpeed;	}
		void SetTilt(bool bTilt)		{	m_bTilt = bTilt;		}
		bool GetTilt()						{	return m_bTilt;		}
		float GetSpeed()					{	return m_fSpeed;		}
      void ClearPath()					{	m_kPath.clear();		}

		void PackTo(NetPacket* pkNetPacket, int iConnectionID )		{ }
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID )	{ }

		void SetRunAnim (string kAnim)		{ m_kRunAnim = kAnim; }
		void SetIdleAnim (string kAnim)		{ m_kIdleAnim = kAnim; }

};

Property* Create_P_PfPath();



#endif



