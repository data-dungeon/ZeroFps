#ifndef _TCS_H_
#define _TCS_H_

#include "../propertys/p_tcs.h"
#include "../propertys/p_tcstrigger.h"
#include "../../engine/zerofps.h"
#include "../engine_systems_x.h"



class P_Tcs;

class ENGINE_SYSTEMS_API Tcs_collission
{
	public:
	
		P_Tcs*	pkBody1;
		P_Tcs*	pkBody2;

		vector<Vector3> kPositions;
		vector<Vector3> kNormals;
		
		void Clear()
		{
			pkBody1 = NULL;
			pkBody2 = NULL;
			
			kPositions.clear();
			kNormals.clear();	
		}
};

class ENGINE_SYSTEMS_API Tcs : public ZFSubSystem
{
	private:
		ZeroFps*				m_pkZeroFps;
		EntityManager*		m_pkEntityMan;
		Render*				m_pkRender;
	
		vector<P_Tcs*>				m_kBodys;	
		vector<P_TcsTrigger*>	m_kTriggers;	
		vector<Tcs_collission*>	m_kCollissions;		
		
		
		Vector3				m_kLastLineTestColPos;
		Vector3				m_kLastTestPos;
		Vector3				m_kLastTestNormal;
		float					m_fLastTestDist;
		
		float					m_fSleepLinVel;
		float					m_fSleepRotVel;
		float					m_fMaxDelay;
		float					m_fMinTime;
		float					m_fMinForce;
		float					m_fMaxVel;
		float					m_fAlmostZero;
		float					m_fTimeSlice;
		float					m_fGravity;
		
		float					m_fSleepTime;
		int					m_iCollisionIterations;
		int					m_iContactIterations;
		
		P_Tcs*				m_pkBodyCopy1;
		P_Tcs*				m_pkBodyCopy2;		
				
		int					m_iNrOfCollissions;			//nr of collisions last frame
		int					m_iNrOfTests;					//nr of tests last frame
		int					m_iNrOfActiveBodies;			//nr of active bodies last frame
		int					m_iNrOfContacts;
		
		//settings
		int 					m_iHandleCollission;
		int					m_iDebugGraph;
		
		
		//main funktions
		void UpdateForces();
		void UpdateAllVelnPos(float fAtime);
		void UpdateBodyVelnPos(P_Tcs* pkBody,float fAtime);		
		
		void UpdateBodyVels(float fAtime);
		void UpdateBodyVel(P_Tcs* pkBody,float fAtime);
		void UpdateBodyPos(float fAtime);
		
		void UpdateCollissions(vector<Tcs_collission*>*	pkCollissions);
		void SyncEntitys();
		void SyncBodys();		
		void HandleCollissions();
		void HandleCollission(Tcs_collission* pkCol,bool bNoBounce = false,bool bNoAngular = false);
		void ResetForces();
		void TryToSleep(P_Tcs* pkBody1,P_Tcs* pkBody2);
						
		void StopObjects(float fAtime);	
		void ResetTempStatics();
		
		void PushVelPos();
		void PopVelPos();
		
		//triggers
		void UpdateTriggers();
		
		//chock test
		void Shock();
		
		void FindRestingBodys();
		
		//clear collissions
		void ClearCollissions();
		
		//blub
		bool IsInNerbyZone(P_Tcs* pkBody1,P_Tcs* pkBody2);
		
		//collission funktions
		void TestSphereVsSphere(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions);
		bool CollideSphereVSSphere(P_Tcs* pkBody1,P_Tcs* pkBody2);			
		
		void TestSphereVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions);
		bool CollideSphereVSMesh(P_Tcs* pkSphere,P_Tcs* pkMesh);			

		void TestMeshVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions);
		bool CollideMeshVSMesh(P_Tcs* pkSphere,P_Tcs* pkMesh,Tcs_collission* pkTempCol);			
		bool CollideMeshVSMesh2(P_Tcs* pkSphere,P_Tcs* pkMesh);			
		bool CollideMeshVSMesh3(P_Tcs* pkSphere,P_Tcs* pkMesh,Tcs_collission* pkTempCol);			
		
		void TestBoxVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions);
		bool CollideBoxVSMesh(P_Tcs* pkBox,P_Tcs* pkMesh,Tcs_collission* pkTempCol);		
		bool TestLineVSQuad(Vector3* pkVertex0,Vector3* pkVertex1,Vector3* pkVertex2,Vector3* pkVertex3,Vector3* pkPos1,Vector3* pkPos2);

		void TestBoxVsBox(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions);
		bool CollideBoxVSBox(P_Tcs* pkBox,P_Tcs* pkMesh,Tcs_collission* pkTempCol);
		
		//AABB tests
		void UpdateAABBs();
		bool TestAABB(P_Tcs* pkBody1,P_Tcs* pkBody2);
		bool TestPolygonAABB(Vector3* pkPolygon1,Vector3* pkPolygon2);
				
		//line tests
		void UpdateLineTests(float fAlphaTime);		
		
		
		bool TestLineVSMesh(Vector3 kStart,Vector3 kDir,P_Tcs* pkB);
		bool TestLineVSSphere(const Vector3& kP1,const Vector3& kP2,P_Tcs* pkB);

		bool CharacterTestLineVSMesh(const Vector3& kStart,const Vector3& kDir,P_Tcs* pkMesh);
		bool CharacterTestLineVSSphere(const Vector3& kP1,const Vector3& kP2,P_Tcs* pkB);
		bool TestLineVSPolygon(const Vector3* pkVerts,const Vector3* pkPos1,const Vector3* pkPos2);
				
		bool TestSphereVSPolygon(Vector3* kVerts,P_Tcs* pkSphere);
		bool TestSides(const Vector3* kVerts,const Vector3* pkNormal,const Vector3&);
		bool TestLineVSPolygon(Vector3* pkPolygon,Vector3* pkPos1,Vector3* pkPos2,Plane* pkPlane,const Vector3& pkNormal1,const Vector3& pkNormal2);
		bool TestLineVSPolygonNoNormal(Vector3* pkPolygon,Vector3* pkPos1,Vector3* pkPos2,Plane* pkPlane);

		Vector3 GetNeighbourFaceNormal(const Vector3& kVert1,const Vector3& kVert2,const int& iCurrentFace ,P_Tcs* pkBody);
				
		bool TestPolygonVSPolygon(Vector3* pkPolgyon1,Vector3* pkPolygon2);
		float LineInsidePolygon(Vector3* pkPolgyon,Plane* pkPlane,Vector3* pkLinePos1,Vector3* pkLinePos2,Vector3* pkColPos);
		
		bool TestMotionSpheres(P_Tcs* pkBody1,P_Tcs* pkBody2,const float& fAtime);
		
		
	public:		
		
		Tcs();
		~Tcs();
		
		void Update(float fAlphaTime);
		
		void AddBody(P_Tcs* pkPTcs);
		void RemoveBody(P_Tcs* pkPTcs);		

		void AddTrigger(P_TcsTrigger* pkPTcsTrigger);
		void RemoveTrigger(P_TcsTrigger* pkPTcsTrigger);		
		
		
		
		void SetGravity(float fGravity)	{ m_fGravity=fGravity				;};
		void SetColIt(int iColIt)			{ m_iCollisionIterations = iColIt;};
		void SetConIt(int iConIt)			{ m_iContactIterations = iConIt	;};
		
		int GetNrOfCollissions() 			{ return m_iNrOfCollissions		;};
		int GetNrOfTests() 					{ return m_iNrOfTests				;};
		int GetNrOfActiveBodies() 			{ return m_iNrOfActiveBodies		;};
		int GetNrOfContacts()				{ return m_iNrOfContacts			;};		
		int GetDebugGraph()					{ return m_iDebugGraph				;};
		Vector3 GetLastLineTestPos()		{ return m_kLastLineTestColPos	;};
		
		
		//some public tests that could be usefull =)
		P_Tcs* TestLine(Vector3 kStart,Vector3 kDir,P_Tcs* pkTester);
		P_Tcs* CharacterLineTest(Vector3 kStart,Vector3 kDir,P_Tcs* pkTester);
		
		
		bool StartUp();
		bool ShutDown();
		bool IsValid();

};



#endif






