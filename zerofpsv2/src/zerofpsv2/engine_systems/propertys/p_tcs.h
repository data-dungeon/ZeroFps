#ifndef _P_TCS_H_
#define _P_TCS_H_

#include "../../engine/property.h"
#include <vector>
#include "../engine_systems_x.h"

//#include "../tcs/tcs.h"
#include "p_mad.h"

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

class ENGINE_SYSTEMS_API P_Tcs : public Property
{
	private:
		Tcs*	m_pkTcs;
			
		bool		m_bPolygonTest;
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
      bool		m_bOnGround;
		bool		m_bActiveMoment;
		float		m_fBounce;
		float		m_fFriction;
		
      Vector3	m_kRotVel;
		Vector3	m_kWalkVel;

		Vector3	m_kExternalLinearForce;
		Vector3	m_kExternalRotForce;
		
		bool		m_bLocalStoredData;
		
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

		//temp data
		Vector3	m_kLinearVelocity;
		Vector3	m_kRotVelocity;
		Vector3	m_kLinearForce;
		Vector3	m_kRotForce;
		Vector3	m_kNewPos;
		Matrix3	m_kNewRotation;
		
		Vector3	m_kMSPos;
		float		m_fMSRadius;	

		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName, string kValue );
		float GetBoundingRadius();
		bool SetupMeshData();
	

		
	public:
		
		P_Tcs();
		~P_Tcs();		
		
		void Init();

		
		void CloneOf(Property* pkProperty) { }		
		void Update();
		
		void Enable();
		void Disable();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
		//collission groups
		bool CheckFlag(int iFlag) {return m_akTestGroups[iFlag];};
		void ResetGroupFlags() {m_akTestGroups.reset();};
		void SetTestGroupFlag(int iFlag,bool bValue) {m_akTestGroups[iFlag] = bValue;};
		
		//walk group
		void ResetWalkGroupFlags() {m_akWalkableGroups.reset();};
		bool CheckWalkGroupFlag(int iFlag) {return m_akWalkableGroups[iFlag];};
		void SetWalkGroupFlag(int iFlag,bool bValue) {m_akWalkableGroups[iFlag] = bValue;};			
		
		//forces
		void ApplyForce(Vector3 kAttachPos,const Vector3& kForce,bool bLocal =true);
		void ApplyForce(const Vector3& kForce);
		void ApplyImpulsForce(Vector3 kAttachPos,const Vector3& kForce,bool bLocal=true);
		void ApplyImpulsForce(const Vector3& kForce);
		
		Vector3 GetVel(Vector3 kPos,bool bLocal = true);
		
		//sets 
		void SetWalkVel(Vector3 kWalkVel) { m_kWalkVel = kWalkVel;};		
      void SetRotVel (Vector3 kRotVel)    { m_kRotVel = kRotVel; }		
		void SetPolygonTest(bool t) {m_bPolygonTest = t;};
		void SetRadius(float t) {m_fRadius = t;};
		void SetStatic(bool bStatic) {m_bStatic = bStatic;};		
		void SetRefetchPolygonData() {m_bHavePolygonData = false;};
		void SetGravity(bool t) {m_bGravity = t;};
		void SetGroup(int iGroup) {m_iGroup = iGroup;};		
		void SetData(vector<Mad_Face> kFaces, vector<Vector3> kVertex, vector<Vector3> kNormals , float fRadius);
		void SetHmap(HeightMap* pkMap) { m_pkHmap = pkMap; }
		
		//gets		
		Vector3 GetWalkVel() { return m_kWalkVel;}; 				
		bool GetOnGround() { return m_bOnGround;};
		
		
		//tests
		bool LineVSMesh(Vector3 &kPos,Vector3 &kDir);
		bool TestPolygon(Vector3* kVerts,Vector3 kPos1,Vector3 kPos2);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos);

		void Draw();
		Matrix4		m_kModelMatrix;
		void GenerateModelMatrix();
		Matrix4 GetModelMatrix();

		int		m_iColFace;
		Vector3	m_kColPos;
		friend class Tcs;
};

Property* Create_P_Tcs();

#endif







