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
	
	//clear list
	m_kPropertys.clear();
	
	//get frame time
	m_fFrameTime=m_pkZeroFps->GetFrameTime();
	
	//get all physicpropertys
	m_pkObjectMan->GetWorldObject()->GetAllPropertys(&m_kPropertys,PROPERTY_TYPE_PHYSIC,PROPERTY_SIDE_SERVER);



	for(list<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) {	
		PhysicProperty* PP = static_cast<PhysicProperty*>(*it);		
		//WARNING THIS CRAP SHULD NOT BE HERE!!!!!!!!!!!!!!
		PP->Update();
	}


	CalcNewPos();
	CheckCollisions();	
	HandleCollisions();



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
		Acc+=Vector3(0,-9.82,0);
	
	if(pkPP->m_bFloat)
		if(pkObject->GetPos().y < 0)
			Acc+=Vector3(0,12,0);
		
	return pkObject->GetVel() + (Acc *  m_fFrameTime);
}



CollisionData* PhysicsEngine::DeepTest(PhysicProperty* pkPP1,PhysicProperty* pkPP2)
{
//	cout<<"deep testing "<<endl;

	CollisionData* pkCD=NULL;
	
	CollisionShape* CS1=pkPP1->GetColShape();
	CollisionShape* CS2=pkPP2->GetColShape();
	
	CollisionShape* CSP1=pkPP1->GetColSphere();
	CollisionShape* CSP2=pkPP2->GetColSphere();
	
	
	if(CS1!=NULL && CS2!=NULL)	
		pkCD=CS1->Test(CS2,m_fFrameTime,true);
	else if(CS1!=NULL && CS2==NULL)	
		pkCD=CS1->Test(CSP2,m_fFrameTime,true);
	else if(CS1==NULL && CS2!=NULL)	
		pkCD=CSP1->Test(CS2,m_fFrameTime,true);
	else if(CS1==NULL && CS2==NULL)
		pkCD=CSP1->Test(CSP2,m_fFrameTime,true);

	return pkCD;
}


void PhysicsEngine::CalcNewPos()
{
	for(list<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) 
	{	
		PhysicProperty* PP = static_cast<PhysicProperty*>(*it);
		
		PP->m_kNewPos=GetNewPos(PP);
		PP->m_kNewVel=GetNewVel(PP);		
		PP->m_kNewAcc=PP->GetObject()->GetAcc();				
		
	}
}

void PhysicsEngine::CheckCollisions()
{
	//clear all known collisions
	ClearCollisions();	
	
	m_bChanged=true;
	
	while(m_bChanged)
	{
		m_bChanged=false;
		
		for(list<Property*>::iterator it1=m_kPropertys.begin();it1!=m_kPropertys.end();it1++) 
		{	
			PhysicProperty* PP1 = static_cast<PhysicProperty*>(*it1);			
			
			for(list<Property*>::iterator it2 = it1;it2!=m_kPropertys.end();it2++) 
			{	
				//dont collide with our self 
				if(it1==it2)
					continue;

				PhysicProperty* PP2 = static_cast<PhysicProperty*>(*it2);
		
				//dont collide two static objects =)
				if(PP1->GetObject()->GetObjectType()==OBJECT_TYPE_STATIC &&
					PP2->GetObject()->GetObjectType()==OBJECT_TYPE_STATIC)
					continue;
		
				if(TestMotionSpheres(PP1,PP2))
				{
					CollisionData* pkCD=NULL;
					pkCD=DeepTest(PP1,PP2);
					
					if(pkCD!=NULL)
					{
						//create collision data
						Collision* temp=new Collision;
						temp->m_pkPP1=pkCD->m_pkPP1;
						temp->m_pkPP2=pkCD->m_pkPP2;
					
						temp->m_kPos1=pkCD->m_kPos1;						
						temp->m_kPos2=pkCD->m_kPos2;						
						
						temp->m_bAdded=false;
						temp->m_bChecked=false;						
						
						m_kCollisions.push_back(temp);
										
						//create collision pointers (for sorting)
						CP* tempcp1=new CP;		
						CP* tempcp2=new CP;								
						tempcp1->m_pkPP=pkCD->m_pkPP1;
						tempcp2->m_pkPP=pkCD->m_pkPP2;
						tempcp1->m_pkCol=temp;
						tempcp2->m_pkCol=temp;
						
						m_kCPs.push_back(tempcp1);
						m_kCPs.push_back(tempcp2);						
						
						delete pkCD;
					}
				}
			}
		}
	}
}


bool PhysicsEngine::TestMotionSpheres(PhysicProperty* pkPP1,PhysicProperty* pkPP2)
{
	Sphere sp1;
	Sphere sp2;
	
	sp1.m_kPos.Lerp(pkPP1->GetObject()->GetPos(),pkPP1->m_kNewPos,0.5);
	sp1.m_fRadius=((pkPP1->GetObject()->GetPos()-pkPP1->m_kNewPos).Length() / 2) + static_cast<CSSphere*>(pkPP1->GetColSphere())->m_fRadius;

	sp2.m_kPos.Lerp(pkPP2->GetObject()->GetPos(),pkPP2->m_kNewPos,0.5);
	sp2.m_fRadius=((pkPP2->GetObject()->GetPos()-pkPP2->m_kNewPos).Length() / 2) + static_cast<CSSphere*>(pkPP2->GetColSphere())->m_fRadius;

	
	float Dist= (sp1.m_kPos-sp2.m_kPos).Length();

//	cout<<"HORA"<<endl;

	if(Dist < (sp1.m_fRadius + sp2.m_fRadius))
		return true;
	else 		
		return false;

}

void PhysicsEngine::HandleCollisions()
{
	vector<Collision*> kCols;
	kCols.clear();

	m_kCPs.sort(SortCollision);

	PhysicProperty* pkSPP=NULL;
	Collision* pkCO=NULL;
	for(list<CP*>::iterator itCp=m_kCPs.begin();itCp!=m_kCPs.end();itCp++) 
	{
		Collision* pkCol=(*itCp)->m_pkCol;		
		
		
		if((*itCp)->m_pkPP != pkSPP)
		{
			if(pkCol->m_bAdded==false)
			{
				if(pkCol->m_bChecked==false)
					pkCol->m_bChecked=true;
				else{
					pkCol->m_bAdded=true;
					kCols.push_back(pkCol);
				}
			}
	
			if(pkCol->m_pkPP1->m_bSolid && pkCol->m_pkPP2->m_bSolid)
			{
				pkSPP=(*itCp)->m_pkPP;	
				pkCO=pkCol;
			}
		}				
		else	//if the collision hapend after a collision with a solid object
		{			
					
			//check so the collision is not already added
			if(pkCol->m_bAdded==false)
			{
			
//				Vector3 kOldNewPos1=(*it)->m_pkPP->m_kNewPos;
				
//				Vector3 kOldNewPos2=pkCol->m_pkPP2->m_kNewPos;
				
				//set the objects m_kNewPos to the one after a collision with the last solide object
				if(pkCO->m_pkPP1==(*itCp)->m_pkPP)
					(*itCp)->m_pkPP->m_kNewPos=pkCO->m_kPos1;
				else
					(*itCp)->m_pkPP->m_kNewPos=pkCO->m_kPos2;

	
				CollisionData* pkCD=NULL;
				//test collision with the new m_kNewPos that would be after a collision with the first object				
				pkCD=DeepTest(pkCol->m_pkPP1,pkCol->m_pkPP2);
						
				//if a collision still occurs then update the collision data and puch it in kCols
				if(pkCD!=NULL)
				{
//					cout<<"multi collision"<<endl;
				
					pkCol->m_kPos1=pkCD->m_kPos1;						
					pkCol->m_kPos2=pkCD->m_kPos2;						
											
					if(pkCol->m_bChecked==false)
						pkCol->m_bChecked=true;
					else
					{
						pkCol->m_bAdded=true;
						kCols.push_back(pkCol);
					}
					
					delete pkCD;
				}

//				pkCol->m_pkPP1->m_kNewPos=kOldNewPos1;
//				pkCol->m_pkPP2->m_kNewPos=kOldNewPos2;					
				
			}
		}
	}
	
	
	int i;
	for( i=0;i<kCols.size();i++) 
	{	
		if(kCols[i]->m_pkPP1->m_bSolid && kCols[i]->m_pkPP2->m_bSolid)
		{
			kCols[i]->m_pkPP1->m_kNewPos=kCols[i]->m_kPos1;
			kCols[i]->m_pkPP2->m_kNewPos=kCols[i]->m_kPos2;			
		
			kCols[i]->m_pkPP1->m_kNewVel=Vector3(0,0,0);
			kCols[i]->m_pkPP2->m_kNewVel=Vector3(0,0,0);
		
		}		
	}
	

	for(list<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) 
	{	
		(*it)->GetObject()->GetPos()=static_cast<PhysicProperty*>(*it)->m_kNewPos;
		(*it)->GetObject()->GetVel()=static_cast<PhysicProperty*>(*it)->m_kNewVel;			
		(*it)->GetObject()->GetAcc()=static_cast<PhysicProperty*>(*it)->m_kNewAcc;		
	}

	for(i=0;i<kCols.size();i++) 
	{	
		kCols[i]->m_pkPP1->GetObject()->Touch(kCols[i]->m_pkPP2->GetObject());
		kCols[i]->m_pkPP2->GetObject()->Touch(kCols[i]->m_pkPP1->GetObject());
	}
}

void PhysicsEngine::ClearCollisions()
{
	for(int i=0;i<m_kCollisions.size();i++) 
	{	
		delete m_kCollisions[i];
	}
		
	for(list<CP*>::iterator it=m_kCPs.begin();it!=m_kCPs.end();it++) 
	{			
		delete (*it);
	}
	
	
	m_kCollisions.clear();
	m_kCPs.clear();
}

bool PhysicsEngine::TestLine(list<PhysicProperty*>* pkPPList,Vector3 kPos,Vector3 kVec)
{
//	PhysicProperty* CPP=NULL;
//	float CloseDist=99999999;
	pkPPList->clear();

	for(list<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) 
	{	
		Vector3 c=(*it)->GetObject()->GetPos() - kPos;		
		kVec.Normalize();		
		Vector3 k=kVec.Proj(c);		
		float cdis=c.Length();
		float kdis=k.Length();
		float Distance = sqrt((cdis*cdis)-(kdis*kdis));
		
		float fRadius=static_cast<CSSphere*>(static_cast<PhysicProperty*>(*it)->GetColSphere())->m_fRadius;
		
//		cout<<"DISTANCE:"<<Distance<<endl;
		
		if(Distance < fRadius)
		{			
			pkPPList->push_back(static_cast<PhysicProperty*>(*it));
			
/*			if(cdis < CloseDist)
			{
				CPP=(*it);
				CloseDist=cdis;			
			}		*/
		}		
	}
	
	if(pkPPList->size()==0)
		return false;
	
	return true;
}


/*


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
*		
		
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
*/














