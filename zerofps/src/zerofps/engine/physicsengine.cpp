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
		
		//WARNING THIS CRAP SHULD NOT BE HERE!!!!!!!!!!!!!!
		PP->Update();
		
/*		
		Object* pkObject=PP->GetObject();		
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glColor3f(1,1,1);		
		glPushMatrix();
			glTranslatef(pkObject->GetPos().x,pkObject->GetPos().y,pkObject->GetPos().z);
			glutSolidSphere(static_cast<CSSphere*>(PP->GetColSphere())->m_fRadius, 10,10);
		glPopMatrix();
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);		
		glEnable(GL_CULL_FACE);
*/		

		MoveObject(PP);
	
	}

	
	m_kPropertys.clear();
}

Vector3 PhysicsEngine::GetNewPos(PhysicProperty* pkPP)
{
	return pkPP->GetObject()->GetPos() + GetNewVel(pkPP) * m_fFrameTime;
}

Vector3 PhysicsEngine::GetNewVel(PhysicProperty* pkPP)
{
	Object* pkObject=pkPP->GetObject();
	
	//add object acceleration
	Vector3 Acc=pkObject->GetAcc();
	
	if(pkPP->m_bGravity)
		Acc+=Vector3(0,-5,0);
	
	if(pkPP->m_bFloat)
		if(pkObject->GetPos().y < 0)
			Acc+=Vector3(0,10,0);
		
	return pkObject->GetVel() + (Acc *  m_fFrameTime);
}

void PhysicsEngine::MoveObject(PhysicProperty* pkPP)
{
	pkPP->GetObject()->GetVel() = GetNewVel(pkPP);
	pkPP->GetObject()->GetPos() += pkPP->GetObject()->GetVel()*m_fFrameTime;
	
//	pkObject->GetVel() += pkObject->GetAcc() * m_fFrameTime;
//	pkObject->GetPos() += pkObject->GetVel() * m_fFrameTime;;
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
		kPos2=GetNewPos(static_cast<PhysicProperty*>(*it));	
		
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
			
			
			if(m_kMotionSpheres[i].m_pkPP->GetObject()->GetObjectType()==OBJECT_TYPE_STATIC &&
				m_kMotionSpheres[j].m_pkPP->GetObject()->GetObjectType()==OBJECT_TYPE_STATIC)
				continue;
			
			
/*			
			if(m_kMotionSpheres[i].m_pkPP->GetObject()->GetVel()==Vector3(0,0,0) &&
				m_kMotionSpheres[j].m_pkPP->GetObject()->GetVel()==Vector3(0,0,0))
				continue;
*/		
		
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
		pkCD=CS1->Test(CS2,S1,S2,m_fFrameTime,true);
	else if(CS1!=NULL && CS2==NULL)	
		pkCD=CS1->Test(CSP2,S1,S2,m_fFrameTime,true);
	else if(CS1==NULL && CS2!=NULL)	
		pkCD=CSP1->Test(CS2,S1,S2,m_fFrameTime,true);
	else if(CS1==NULL && CS2==NULL)
		pkCD=CSP1->Test(CSP2,S1,S2,m_fFrameTime,true);

	if(pkCD!=NULL)	
	{
	
		//touch object 1
		S1->m_pkPP->GetObject()->Touch(pkCD);
	
		//inverse the data krap
		pkCD->m_pkOther=S1->m_pkPP->GetObject();	
		swap(pkCD->m_kOtherPos,pkCD->m_kPos);
		swap(pkCD->m_kOtherVel,pkCD->m_kVel);
		swap(pkCD->m_kOtherAcc,pkCD->m_kAcc);	
		swap(pkCD->m_kOtherRot,pkCD->m_kRot);	

		//touche object 2
		S2->m_pkPP->GetObject()->Touch(pkCD);
	
		//delete the collision data object
		delete pkCD;
	}
}













