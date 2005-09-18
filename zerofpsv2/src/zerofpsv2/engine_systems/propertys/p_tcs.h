#ifndef _P_TCS_H_
#define _P_TCS_H_
  
#include "../../engine/property.h"
#include <vector>
#include "../engine_systems_x.h"
#include "p_mad.h"
 
#include "../tcs/aabbtree.h"

#define TCS_GROUPS 10

using namespace std;

class Tcs;
class P_Mad;

/*
	GROUPS USED IN MISTLANDS
	
	0:   zone models
	1:	 normal characters
	2:	 spells fireballs etc
	3:   non important visual effect objects
	4:   enviroment objects (stones trees etc)
*/

enum ENGINE_SYSTEMS_API TestTypes
{
	E_NONE	= -1,
	E_SPHERE = 0,
	E_MESH	= 1,
	E_BOX		= 2,
	E_HMAP	= 3,
};

class ENGINE_SYSTEMS_API P_Tcs : public Property
{
	private:
		Tcs*			m_pkTcs;
		ZSSRender*	m_pkRender;
		
		bool		m_bHaveUpdated;
		
		bool		m_bHaveTriedToSetup;
		Vector3	m_kBoxSize;
		int		m_iTestType;
		bool		m_bStatic;
		float	 	m_fRadius;						
		bool		m_bHavePolygonData;
		bool		m_bGravity;
		bool		m_bCharacter;
		int	 	m_iGroup;				
		float 	m_fLegLength;		
		float		m_fMass;
		float		m_fInertia;
		float		m_fAirFriction;
		bool		m_bActiveMoment;
		float		m_fBounce;
		float		m_fFriction;
		bool		m_bSleeping;
		bool		m_bCantSleep;
		bool		m_bDisableOnSleep;
		bool		m_bRemoveOnSleep;
		bool		m_bNoColRespons;
		
		map<int,bool>	m_kTrigging;

		Vector3	m_kExternalLinearForce;
		Vector3	m_kExternalRotForce;
		
		bool		m_bLocalStoredData;
		
		//friends
		vector<P_Tcs*>		m_kSleepingFriends;
		
		//test flags
		bitset<TCS_GROUPS>	m_akTestGroups;
		bitset<TCS_GROUPS>	m_akWalkableGroups;		
		

		//mesh data
		int						m_iModelID;
		float 					m_fScale;
		P_Mad*					m_pkMad;
		HeightMap*				m_pkHmap;
		vector<Mad_Face>*		m_pkFaces;			// Faces in mesh.
		vector<Vector3>*		m_pkVertex;			// Vertex frames for mesh.
		vector<Vector3>*		m_pkNormal;
	
		
		AABBTree					m_kAABBTree;
		
		//temp data
		Vector3	m_kAABBMax;
		Vector3	m_kAABBMin;
		Matrix4	m_kAABBRotation;
		
		bool		m_bActive;      
		bool		m_bOnGround;		
		bool		m_bTempStatic;				
		Vector3	m_kLastPos;				//used to determin if a body is resting
		float		m_fMoveDistance;		//total movement since last rest find call
		
		Vector3	m_kLinearVelocity;
		Vector3	m_kRotVelocity;
		Vector3	m_kLinearForce;
		Vector3	m_kRotForce;
		Vector3	m_kNewPos;
		Matrix4	m_kNewRotation;
		
		//bakup used for push/pop calls in tcs
		Vector3	m_kOldNewPos;
		Vector3	m_kOldLinearVelocity;
		Matrix4	m_kOldNewRotation;
		Vector3	m_kOldRotVelocity;
		

		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( const string& kValueName, const string& kValue );
		float GetBoundingRadius();
		bool SetupMeshData();
	
		void DrawAABBTree();
		
	public:
		
		P_Tcs();
		~P_Tcs();		
		
		void Init();

		
		void Update();
		
		void Enable();
		void Disable();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		//resting
		void Wakeup(bool bWakeChilds = true);
		void Sleep();
		void AddRestingBody(P_Tcs* pkBody);
		
		
		//collission groups
		bool CheckFlag(int iFlag) {return m_akTestGroups[iFlag];};
		void ResetGroupFlags() {m_akTestGroups.reset();};
		void SetTestGroupFlag(int iFlag,bool bValue) {m_akTestGroups[iFlag] = bValue;};
		
		//walk group
		void ResetWalkGroupFlags() {m_akWalkableGroups.reset();};
		bool CheckWalkGroupFlag(int iFlag) {return m_akWalkableGroups[iFlag];};
		void SetWalkGroupFlag(int iFlag,bool bValue) {m_akWalkableGroups[iFlag] = bValue;};			
		
		//forces
		void ClearExternalForces();
		void ApplyForce(Vector3 kAttachPos,const Vector3& kForce,bool bLocal =true);
		void ApplyImpulsForce(Vector3 kAttachPos,const Vector3& kForce,bool bLocal=true);
		void ApplyImpulsForce(const Vector3& kForce);		

		Vector3 GetVel(Vector3 kPos,bool bLocal = true);
		
		//sets 
		void SetAirFriction(float fFriction)	{m_fAirFriction = fFriction;				}
      void SetRotVel (Vector3 kRotVel)			{Wakeup();m_kRotVelocity = kRotVel; 	}		
		void SetLinVel (Vector3 kLinVel)			{Wakeup();m_kLinearVelocity = kLinVel; }				
		void SetTestType(int iTest)				{m_iTestType = iTest;	};
		void SetRadius(float t) 					{m_fRadius = t;			};
		void SetLegLength(float fLeg)				{m_fLegLength = fLeg;	};
		void SetStatic(bool bStatic) 				{m_bStatic = bStatic;	};		
		void SetRefetchPolygonData() 				{m_bHavePolygonData = false;};
		void SetGravity(bool t) 					{m_bGravity = t;			};
		void SetGroup(int iGroup) 					{m_iGroup = iGroup;		};		
		void SetData(vector<Mad_Face> kFaces, vector<Vector3> kVertex, vector<Vector3> kNormals , float fRadius);
		void SetHmap(HeightMap* pkMap) 			{m_pkHmap = pkMap; 		};
		
		//gets		
		bool  InActive()							{ return ( m_bStatic || m_bSleeping || m_bTempStatic || !m_bActive);	}
		bool  GetOnGround() 						{ return m_bOnGround;				};
		float GetRadius()							{ return m_fRadius;					};
		Vector3 GetLinVel()						{ return m_kLinearVelocity;		};
		float GetLegLength()						{ return m_fLegLength;				};		
		const map<int,bool>&	GetTriggers()	{ return m_kTrigging;				};
		bool IsTrigging(int iTriggerID)		{ return m_kTrigging.find(iTriggerID) != m_kTrigging.end();	};
		float GetAirFriction()					{ return m_fAirFriction;			};
		
		//tests
		bool LineVSMesh(Vector3 &kPos,Vector3 &kDir);
		bool TestPolygon(Vector3* kVerts,Vector3 kPos1,Vector3 kPos2);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos);

		void Draw();
		const Matrix4& GetModelMatrix() const;

		int		m_iColFace;
		Vector3	m_kColPos;
		
		friend class Tcs;
		friend class P_TcsTrigger;
};



Property* Create_P_Tcs();
void ENGINE_SYSTEMS_API Register_PTcs(ZSSZeroFps* pkZeroFps);


#endif







