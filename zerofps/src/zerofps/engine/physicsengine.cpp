#include "physicsengine.h"
#include "cssphere.h"

PhysicsEngine::PhysicsEngine() 
: ZFObject("PhysicsEngine")
{
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));		

	
	m_bUpdate=true;
}


void PhysicsEngine::Update()
{
	if(!m_bUpdate)
		return;
	
	//get frame time
	m_fFrameTime=m_pkZeroFps->GetFrameTime();
	
	//get all physicpropertys
	m_pkObjectMan->GetWorldObject()->GetAllPropertys(&m_kPropertys,PROPERTY_TYPE_PHYSIC,PROPERTY_SIDE_SERVER);

	CalcMotionSpheres();
	TestCollisions();
	
	
	for(list<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) {	
		PhysicProperty* PP = static_cast<PhysicProperty*>(*it);
		
		if(PP->m_bGravity)
			PP->GetObject()->GetVel()+=Vector3(0,-9.82,0)*m_fFrameTime;
		
		
		if(PP->m_bMove)			
			MoveObject(PP->GetObject());
	}

	
	m_kPropertys.clear();
}

Vector3 PhysicsEngine::GetNewPos(Object* pkObject,float fTime)
{
	Vector3 Vel = pkObject->GetVel() + pkObject->GetAcc() * fTime;
	return pkObject->GetPos() + Vel * fTime;
}

void PhysicsEngine::MoveObject(Object* pkObject)
{
	pkObject->GetVel() += pkObject->GetAcc() * m_fFrameTime;
	pkObject->GetPos() += pkObject->GetVel() * m_fFrameTime;;
}


void PhysicsEngine::CalcMotionSpheres()
{
	m_kMotionSpheres.clear();
	
	Sphere sp;		
	Vector3 kPos1;	
	Vector3 kPos2;
		
	for(list<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) {	
		CSSphere *csp = static_cast<CSSphere*>((static_cast<PhysicProperty*>(*it))->GetColSphere());
		
		kPos1=(*it)->GetObject()->GetPos();
		kPos2=GetNewPos((*it)->GetObject(),m_fFrameTime);	
		
		sp.m_kPos.Lerp(kPos1,kPos2,0.5);
		sp.m_fRadius= (abs((kPos1-kPos2).Length()) / 2) + csp->m_fRadius;
		sp.m_pkPP=static_cast<PhysicProperty*>(*it);
		
		m_kMotionSpheres.push_back(sp);
				
//		cout<<"RADIUS: "<<sp.m_fRadius<<endl;
	}
}

void PhysicsEngine::TestCollisions()
{	
	for(int i=0;i<m_kMotionSpheres.size();i++)
	{
		for(int j=i+1;j<m_kMotionSpheres.size();j++)		
		{
			if(TestSphere(&m_kMotionSpheres[i],&m_kMotionSpheres[j]))
				DeepTest(&m_kMotionSpheres[i],&m_kMotionSpheres[j]);	
			
		}
	}
}


bool PhysicsEngine::TestSphere(Sphere* S1,Sphere* S2)
{
	float Dist= abs((S1->m_kPos-S2->m_kPos).Length());

	if(Dist < (S1->m_fRadius + S2->m_fRadius))
		return true;
	else 		
		return false;

}


void PhysicsEngine::DeepTest(Sphere* S1,Sphere* S2)
{
//	cout<<"deep testing "<<endl;

	CollisionData* pkCD;
	
	CollisionShape* CS1=S1->m_pkPP->GetColShape();
	CollisionShape* CS2=S2->m_pkPP->GetColShape();
	
	CollisionShape* CSP1=S1->m_pkPP->GetColSphere();
	CollisionShape* CSP2=S2->m_pkPP->GetColSphere();
	
	
	if(CS1!=NULL && CS2!=NULL)	
		pkCD=CS1->Test(CS2,true);
	else if(CS1!=NULL && CS2==NULL)	
		pkCD=CS1->Test(CSP2,true);
	else if(CS1==NULL && CS2!=NULL)	
		pkCD=CSP1->Test(CS2,true);
	else if(CS1==NULL & CS2==NULL)
		pkCD=CSP1->Test(CSP2,true);

	if(pkCD==NULL)
		cout<<"NO Collission"<<endl;
	else	
		cout<<"Collision"<<endl;
	

}













