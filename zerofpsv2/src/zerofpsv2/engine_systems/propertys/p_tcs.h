#ifndef _P_TCS_H_
#define _P_TCS_H_

#include "../../engine/property.h"
#include <vector>
#include "../engine_systems_x.h"

#include "../tcs/tcs.h"

#define TCS_GROUPS 10

using namespace std;
class Tcs;


/*
	GROUPS USED IN MISTLANDS
	
	0:   zone models
	1:	 normal characters
	2:	 spells fireballs etc
	3:   non important visual effect objects



*/

class ENGINE_SYSTEMS_API P_Tcs : public Property
{
	private:
		Tcs*	m_pkTcs;
			
		bool	m_bPolygonTest;
		float	 m_fRadius;						
		bool	m_bHavePolygonData;
		bool	m_bGravity;
		
		int	 m_iGroup;		
		bitset<TCS_GROUPS>	m_akTestGroups;

		//mesh data
		int						m_iModelID;
		float 					m_fScale;
		vector<Mad_Face>*		m_pkFaces;			// Faces in mesh.
		vector<Vector3>*		 m_pkVertex;			// Vertex frames for mesh.
		vector<Vector3>*		 m_pkNormal;

		//temp data
		Vector3	m_kNewPos;
		Vector3	m_kMSPos;
      Vector3 m_kRotVel;

		float		m_fMSRadius;	

		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName, string kValue );
		float GetBoundingRadius();
		bool SetupMeshData();
	
	public:
		
		P_Tcs();
		~P_Tcs();		
		
		void Init();

      void SetRotVel (Vector3 kRotVel)    { m_kRotVel = kRotVel; }
		
		void CloneOf(Property* pkProperty) { }		
		void Update();
		
		void Enable();
		void Disable();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
		bool CheckFlag(int iFlag) {return m_akTestGroups[iFlag];};
		void SetGroup(int iGroup) {m_iGroup = iGroup;};
		void ResetGroupFlags() {m_akTestGroups.set();};
		void SetTestGroupFlag(int iFlag,bool bValue) {m_akTestGroups[iFlag] = bValue;};
		void SetPolygonTest(bool t) {m_bPolygonTest = t;};
		void SetRadius(float t) {m_fRadius = t;};
		void SetRefetchPolygonData() {m_bHavePolygonData = false;};
		void SetGravity(bool t) {m_bGravity = t;};
		
		friend class Tcs;
};

Property* Create_P_Tcs();


#endif







