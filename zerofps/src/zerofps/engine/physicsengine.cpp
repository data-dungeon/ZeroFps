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

	int iSize = m_kPropertys.size();

	for(list<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) {	
		static_cast<PhysicProperty*>(*it)->Update();		
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
	Vector3 Vel=pkObject->GetVel();
	
	if(pkPP->m_bGravity)
		Acc+=Vector3(0,-11,0);
	
	if(pkPP->m_bFloat)
		if(pkObject->GetPos().y < 0) 
		{
			Acc+=Vector3(0,11,0);
			Acc+= (Vel*-1) *0.3;			//simulate water friktion		
		}
		
	return Vel + (Acc *  m_fFrameTime);
}



Collision* PhysicsEngine::DeepTest(PhysicProperty* pkPP1,PhysicProperty* pkPP2)
{
//	cout<<"deep testing "<<endl;

	Collision* pkCD=NULL;
	
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
	
	
	for(list<Property*>::iterator it3=m_kPropertys.begin();it3!=m_kPropertys.end();it3++) 
	{	
		PhysicProperty* PP1 = static_cast<PhysicProperty*>(*it3);			
	
		if(PP1->m_bStride)
			Stride(PP1);		
	}	
	
	
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
										
				Collision* pkCD=NULL;
				pkCD=DeepTest(PP1,PP2);
		
				
				if(pkCD!=NULL)
				{
					pkCD->m_bAdded=false;
					pkCD->m_bChecked=false;						
					
					m_kCollisions.push_back(pkCD);
									
					//create collision pointers (for sorting)
					CP* tempcp1=new CP;		
					CP* tempcp2=new CP;								
					tempcp1->m_pkPP=pkCD->m_pkPP1;
					tempcp2->m_pkPP=pkCD->m_pkPP2;
					tempcp1->m_pkCol=pkCD;
					tempcp2->m_pkCol=pkCD;
					
					m_kCPs.push_back(tempcp1);
					m_kCPs.push_back(tempcp2);						
					
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

//	if(m_kCPs.size()/2>1)
//		cout<<"nr Of tests: "<<m_kCPs.size()/2<<endl;

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
				{
					pkCol->m_bChecked=true;
				}
				else
				{
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
				Vector3 kOldNewPos1=(*itCp)->m_pkPP->m_kNewPos;
					
				//set the objects m_kNewPos to the one after a collision with the last solide object
				if(pkCO->m_pkPP1==(*itCp)->m_pkPP)
				{
					(*itCp)->m_pkPP->m_kNewPos=pkCO->m_kPos1;
					//cout<<"taking new pos from "<<pkCO->m_pkPP1->GetObject()->GetName()<<endl;
				}
				else
				{
					(*itCp)->m_pkPP->m_kNewPos=pkCO->m_kPos2;
					//cout<<"taking new pos from "<<pkCO->m_pkPP2->GetObject()->GetName()<<endl;					
				}
				
				//test collision with the new m_kNewPos that would be after a collision with the first object				
				Collision* pkCD=DeepTest(pkCol->m_pkPP1,pkCol->m_pkPP2);
						
				//if a collision still occurs then update the collision data and puch it in kCols
				if(pkCD!=NULL)
				{				
					//uppdate old data
					pkCol->m_kPos1=pkCD->m_kPos1;						
					pkCol->m_kPos2=pkCD->m_kPos2;						
											
											
					pkSPP=(*itCp)->m_pkPP;	
					pkCO=pkCol;
												
											
					if(pkCol->m_bChecked==false)
					{
						pkCol->m_bChecked=true;
					}
					else
					{
						pkCol->m_bAdded=true;
						kCols.push_back(pkCol);				
					}
					
					delete pkCD;
								
				}
				
				(*itCp)->m_pkPP->m_kNewPos=kOldNewPos1;				
			}
		}
	}
	
	
	unsigned int i;
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
		(*it)->GetObject()->GetAcc().Set(0,0,0);		
	}

	for(i=0;i<kCols.size();i++) 
	{	
		kCols[i]->m_pkPP1->GetObject()->Touch(kCols[i]);
		kCols[i]->m_pkPP2->GetObject()->Touch(kCols[i]);	
	}
	
//	cout<<"nr Of collisions: "<<kCols.size()<<endl;
}

void PhysicsEngine::ClearCollisions()
{
	for(unsigned int i=0;i<m_kCollisions.size();i++) 
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


bool PhysicsEngine::TestLine(list<Object*>* pkObList,Vector3 kPos,Vector3 kVec)
{
	pkObList->clear();
	
	list<PhysicProperty*> pkPPList;	
	pkPPList.clear();

	
	//get all physics propertys
	if(!TestLine(&pkPPList,kPos,kVec))
		return false;	
	
	//put objects in object list
	for(list<PhysicProperty*>::iterator it=pkPPList.begin();it!=pkPPList.end();it++) 	
	{
		pkObList->push_back((*it)->GetObject());	
	}

	//clear physicproperty list
	pkPPList.clear();

	return true;
}


bool PhysicsEngine::TestLine(list<PhysicProperty*>* pkPPList,Vector3 kPos,Vector3 kVec)
{
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
		
		if(Distance < fRadius)
		{			
			pkPPList->push_back(static_cast<PhysicProperty*>(*it));
		}		
	}
	
	if(pkPPList->size()==0)
		return false;
	
	return true;
}



bool PhysicsEngine::Stride(PhysicProperty* pkPP)
{
	float fStep = 0.00;

	Vector3 kOldNewPos = pkPP->m_kNewPos;
	Vector3 kOldPos = pkPP->GetObject()->GetPos();
	Collision *pkCD = NULL;	
	
//	if(kOldNewPos == kOldPos)
//		return false;
	
	if(kOldNewPos.x == kOldPos.x &&
		kOldNewPos.z == kOldPos.z)
		return false;
		
	Vector3 kBlub = (kOldNewPos - kOldPos).Unit() * fStep;
	Vector3 kBack = -(kOldNewPos - kOldPos).Unit() * 0.05;	
	kBlub.y =0;
	
	pkPP->GetObject()->GetPos() = kOldPos + kBack;
	pkPP->m_kNewPos = kOldPos + Vector3(0,pkPP->m_bStrideHeight,0) + kBack;
//	pkPP->m_kNewPos.y = kOldNewPos.y + pkPP->m_bStrideHeight;
	
	if(pkCD = CheckIfColliding(pkPP))
	{
		pkPP->m_kNewPos = kOldNewPos;
		pkPP->GetObject()->GetPos() = kOldPos;
		
		cout<<"blocked up"<<endl;
		delete pkCD;
		return false;
	}
	
	pkPP->GetObject()->GetPos() = pkPP->m_kNewPos;
	pkPP->m_kNewPos = kOldNewPos + kBlub;
	pkPP->m_kNewPos.y = pkPP->GetObject()->GetPos().y;
	
	if(pkCD = CheckIfColliding(pkPP))
	{
		pkPP->m_kNewPos = kOldNewPos;
		pkPP->GetObject()->GetPos() = kOldPos;
		
		cout<<"blocked forward"<<endl;
		delete pkCD;
		return false;		
	}

	pkPP->GetObject()->GetPos() = pkPP->m_kNewPos ; 
	pkPP->m_kNewPos = kOldNewPos + kBlub;

/*
	pkCD = NULL;
	if(pkCD = CheckIfColliding(pkPP))
	{

//		cout<<"old    pos:"<<kOldPos.x<<" "<<kOldPos.y<<" "<<kOldPos.z<<endl;
//		cout<<"curent pos:"<<pkPP->GetObject()->GetPos().x<<" "<<pkPP->GetObject()->GetPos().y<<" "<<pkPP->GetObject()->GetPos().z<<endl;			
//		cout<<"next   pos:"<<pkPP->m_kNewPos.x<<" "<<pkPP->m_kNewPos.y<<" "<<pkPP->m_kNewPos.z<<endl;						
			
//		cout<<"newpos : "<<pkCD->m_kPos2.y<<endl;
//		cout<<"kOldPos: "<<kOldPos.y<<endl;
		
		cout<<"Step height: "<<(pkCD->m_kPos2.y - pkPP->m_kNewPos.y)<<endl;		
			
		if((pkCD->m_kPos2.y - pkPP->m_kNewPos.y)!=0)
		{	
//			cout<<"Step height: "<<(pkCD->m_kPos2.y - pkPP->m_kNewPos.y)<<endl;
//			pkPP->GetObject()->GetPos() = pkCD->m_kPos2;
//			pkPP->m_kNewPos = pkCD->m_kPos2;
		
//			Vector3 diff = kOldNewPos - kOldPos;
//			if(diff.Length() != 0){
//				pkPP->GetObject()->GetPos() += diff.Unit() * fStep;
//				pkPP->m_kNewPos += diff.Unit() * fStep;
//			}
			
			cout<<"upp uppp"<<endl;
			delete pkCD;			
			return true;		
		}
	}	

//	cout<<"walking"<<endl;

	pkPP->m_kNewPos = kOldNewPos;
	pkPP->GetObject()->GetPos() = kOldPos;
*/		
	delete pkCD;
	return false;
}

Collision* PhysicsEngine::CheckIfColliding(PhysicProperty* pkPP)
{
	Collision* pkCD = NULL; 
	Collision* pkClosest = NULL;
	float fMinDis=99999999;

	
	PhysicProperty* PP1 = pkPP;			
	
	for(list<Property*>::iterator it2 = m_kPropertys.begin();it2!=m_kPropertys.end();it2++) 
	{	
		//dont collide with our self 
		if(PP1==*it2)
			continue;
			
			
		PhysicProperty* PP2 = static_cast<PhysicProperty*>(*it2);

		//dont collide two static objects =)
		if(PP1->GetObject()->GetObjectType()==OBJECT_TYPE_STATIC &&
			PP2->GetObject()->GetObjectType()==OBJECT_TYPE_STATIC)
			continue;

		if(TestMotionSpheres(PP1,PP2))
		{
			pkCD=DeepTest(PP1,PP2);				
				
			if(pkCD != NULL){
				if(pkCD->m_fDistance2 < fMinDis)
				{
					pkClosest = pkCD;
					fMinDis = pkCD->m_fDistance2;
				}
				else
				{
					delete pkCD;
				}
							
				//cout<<"Distance:"<<pkCD->m_fDistance2<<endl;
				//cout<<"colliding with "<<PP2->GetObject()->GetName()<<" AT: "<<pkCD->m_kPos2.x<<" "<<pkCD->m_kPos2.y<<" "<<pkCD->m_kPos2.z<<endl;				
			}		
		}
	}
	
	return pkClosest;
}





