#include "physicsengine.h"
#include "cssphere.h"
#include "../../engine/zerofps.h"
#include "../../engine/entitymanager.h"

PhysicsEngine::PhysicsEngine() 
: ZFSubSystem("PhysicsEngine")
{
	m_bUpdate=true;
}

bool PhysicsEngine::StartUp()	
{ 
	m_pkZeroFps		= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));		
	m_pkObjectMan	= static_cast<EntityManager*>(GetSystem().GetObjectPtr("EntityManager"));		
	m_pkRender	= static_cast<Render*>(GetSystem().GetObjectPtr("Render"));			

	return true; 
}

bool PhysicsEngine::ShutDown() 
{ 
	return true; 
}
bool PhysicsEngine::IsValid()	{ return true; }

void PhysicsEngine::Update()
{
	if(!m_bUpdate)
		return;
	
	//clear list
	m_kPropertys.clear();
	
	//get frame time
	m_fFrameTime = m_pkZeroFps->m_pkObjectMan->GetSimDelta();
	
	//get all physicpropertys
	m_pkObjectMan->GetWorldObject()->GetAllPropertys(&m_kPropertys,PROPERTY_TYPE_PHYSIC,PROPERTY_SIDE_SERVER);

	int iSize = m_kPropertys.size();
	m_pkZeroFps->DevPrintf("om", "Phy::Update(%s, %s,%d) = %d",
		"PROPERTY_TYPE_PHYSIC","PROPERTY_SIDE_SERVER",0,iSize);

	for(vector<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) {	
		static_cast<P_Physic*>(*it)->Update();		
	}

	CalcNewPos();
	CheckCollisions();	
	HandleCollisions();

}

Vector3 PhysicsEngine::GetNewPos(P_Physic* pkPP)
{
	return pkPP->GetObject()->GetWorldPosV() + GetNewVel(pkPP) * m_fFrameTime;
}

Vector3 PhysicsEngine::GetNewVel(P_Physic* pkPP)
{
	Entity* pkObject=pkPP->GetObject();
	
	//add object acceleration
	Vector3 Acc=pkObject->GetAcc();
	Vector3 Vel=pkObject->GetVel();
	
	if(pkPP->m_bGravity)
		Acc+=Vector3(0,-15,0);
	
	if(pkPP->m_bFloat)
		if(pkObject->GetWorldPosV().y < 0) 
		{
			Acc+=Vector3(0,11,0);
			Acc+= (Vel*-1) *0.3;			//simulate water friktion		
		}
		
	return Vel + (Acc *  m_fFrameTime);
}



Collision* PhysicsEngine::DeepTest(P_Physic* pkPP1,P_Physic* pkPP2)
{
//	cout<<"deep testing "<<endl;

	Collision* pkCD=NULL;
	
	CollisionShape* CS1=pkPP1->GetColShape();
	CollisionShape* CS2=pkPP2->GetColShape();
	
	CollisionShape* CSP1=pkPP1->GetColSphere();
	CollisionShape* CSP2=pkPP2->GetColSphere();
	
	
	if(CS1!=NULL && CS2!=NULL)	
		pkCD=CS1->Test(CS2,true);
	else if(CS1!=NULL && CS2==NULL)	
		pkCD=CS1->Test(CSP2,true);
	else if(CS1==NULL && CS2!=NULL)	
		pkCD=CSP1->Test(CS2,true);
	else if(CS1==NULL && CS2==NULL)
		pkCD=CSP1->Test(CSP2,true);

	return pkCD;
}


void PhysicsEngine::CalcNewPos()
{
	for(vector<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) 
	{	
		P_Physic* PP = static_cast<P_Physic*>(*it);
		
		PP->m_kOldPos = PP->GetObject()->GetWorldPosV();
		PP->m_kNewPos=GetNewPos(PP);
		PP->m_kNewVel=GetNewVel(PP);		
		PP->m_kNewAcc=PP->GetObject()->GetAcc();				
		
	}
}

void PhysicsEngine::CheckCollisions()
{
	//clear all known collisions
	ClearCollisions();	
	
	
	for(vector<Property*>::iterator it3=m_kPropertys.begin();it3!=m_kPropertys.end();it3++) 
	{	
		P_Physic* PP1 = static_cast<P_Physic*>(*it3);			
	
		if(PP1->m_bStride)
			Stride(PP1);		
	}	
	
	
	for(vector<Property*>::iterator it1=m_kPropertys.begin();it1!=m_kPropertys.end();it1++) 
	{	
		P_Physic* PP1 = static_cast<P_Physic*>(*it1);			
		
		
		for(vector<Property*>::iterator it2 = it1;it2!=m_kPropertys.end();it2++) 
		{	
			//dont collide with our self 
			if(it1==it2)
				continue;
				
			P_Physic* PP2 = static_cast<P_Physic*>(*it2);
	
			//dont collide two static objects =)
			if(PP1->m_bStatic && PP2->m_bStatic)
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


bool PhysicsEngine::TestMotionSpheres(P_Physic* pkPP1,P_Physic* pkPP2)
{
	Sphere sp1;
	Sphere sp2;
	
	Vector3 o1pos = pkPP1->GetObject()->GetWorldPosV() ;
	Vector3 o2pos = pkPP2->GetObject()->GetWorldPosV();
	
	sp1.m_kPos.Lerp(o1pos,pkPP1->m_kNewPos,0.5);
	sp1.m_fRadius=(pkPP1->GetObject()->GetWorldPosV().DistanceTo(pkPP1->m_kNewPos) / 2) + static_cast<CSSphere*>(pkPP1->GetColSphere())->m_fRadius;

	sp2.m_kPos.Lerp(o2pos,pkPP2->m_kNewPos,0.5);
	sp2.m_fRadius=(pkPP2->GetObject()->GetWorldPosV().DistanceTo(pkPP2->m_kNewPos) / 2) + static_cast<CSSphere*>(pkPP2->GetColSphere())->m_fRadius;

	
	float Dist= sp1.m_kPos.DistanceTo(sp2.m_kPos);

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


	P_Physic* pkSPP=NULL;
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
	

	for(vector<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) 
	{	
//		(*it)->GetObject()->GetPos() = static_cast<PhysicProperty*>(*it)->m_kOldPos;
		(*it)->GetObject()->SetWorldPosV(static_cast<P_Physic*>(*it)->m_kOldPos);	
		(*it)->GetObject()->SetWorldPosV(static_cast<P_Physic*>(*it)->m_kNewPos);	
//		(*it)->GetObject()->GetPos()=static_cast<PhysicProperty*>(*it)->m_kNewPos;
		(*it)->GetObject()->GetVel()=static_cast<P_Physic*>(*it)->m_kNewVel;
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


bool PhysicsEngine::TestLine(list<Entity*>* pkObList,Vector3 kPos,Vector3 kVec)
{
	pkObList->clear();
	
	list<P_Physic*> pkPPList;	
	pkPPList.clear();

	
	//get all physics propertys
	if(!TestLine(&pkPPList,kPos,kVec))
		return false;	
	
	//put objects in object list
	for(list<P_Physic*>::iterator it=pkPPList.begin();it!=pkPPList.end();it++) 	
	{
		pkObList->push_back((*it)->GetObject());	
	}

	//clear physicproperty list
	pkPPList.clear();

	return true;
}


bool PhysicsEngine::TestLine(list<P_Physic*>* pkPPList,Vector3 kPos,Vector3 kVec)
{
	pkPPList->clear();

	for(vector<Property*>::iterator it=m_kPropertys.begin();it!=m_kPropertys.end();it++) 
	{	
		Vector3 c=(*it)->GetObject()->GetWorldPosV() - kPos;		
		kVec.Normalize();		
		Vector3 k=kVec.Proj(c);		
		float cdis=c.Length();
		float kdis=k.Length();
		float Distance = (float) sqrt((cdis*cdis)-(kdis*kdis));
		
		float fRadius=static_cast<CSSphere*>(static_cast<P_Physic*>(*it)->GetColSphere())->m_fRadius;
		
		if(Distance < fRadius)
		{			
			pkPPList->push_back(static_cast<P_Physic*>(*it));
		}		
	}
	
	if(pkPPList->size()==0)
		return false;
	
	return true;
}



bool PhysicsEngine::Stride(P_Physic* pkPP)
{
	float fStep = 0.00;

	Vector3 kOldNewPos = pkPP->m_kNewPos;
	Vector3 kOldPos = pkPP->GetObject()->GetWorldPosV();
	Collision *pkCD = NULL;	
	
	if(kOldNewPos.x == kOldPos.x &&
		kOldNewPos.z == kOldPos.z)
		return false;
		
	Vector3 kForward = (kOldNewPos - kOldPos).Unit();		
	Vector3 kBlub = (kOldNewPos - kOldPos).Unit() * fStep;
	Vector3 kBack = -(kOldNewPos - kOldPos).Unit() * 0.05;	
	kBlub.y =0;
	
	pkPP->GetObject()->GetWorldPosV() = kOldPos + kBack;
	pkPP->m_kNewPos = kOldPos + Vector3(0,pkPP->m_bStrideHeight,0) + kBack;
	
	if(pkCD = CheckIfColliding(pkPP))
	{
		pkPP->m_kNewPos = kOldNewPos;
		pkPP->GetObject()->GetWorldPosV() = kOldPos;
		
		//cout<<"blocked up"<<endl;
		delete pkCD;
		return false;
	}
	
	pkPP->GetObject()->GetWorldPosV() = pkPP->m_kNewPos;
	pkPP->m_kNewPos = kOldNewPos + kBlub;
	pkPP->m_kNewPos.y = pkPP->GetObject()->GetWorldPosV().y;
	
	if(pkCD = CheckIfColliding(pkPP))
	{
		pkPP->m_kNewPos = kOldNewPos;
		pkPP->GetObject()->GetWorldPosV() = kOldPos;
		
		//cout<<"blocked forward"<<endl;
		delete pkCD;
		return false;		
	}

	pkPP->GetObject()->GetWorldPosV() = pkPP->m_kNewPos ; 
	pkPP->m_kNewPos = kOldNewPos + kBlub;

/*
	pkCD = NULL;
	if(pkCD = CheckIfColliding(pkPP))
	{
		
	
		float m_fGroundAngle = RadToDeg(Vector3(0,1,0).Angle(pkCD->m_kNormal1));
		cout<<"ground"<<m_fGroundAngle<<endl;
		
		if(m_fGroundAngle>65){		
			pkPP->m_kNewPos = kOldNewPos;
			pkPP->GetObject()->GetPos() = kOldPos;
			delete pkCD;
			return false;
			
			
			pkPP->m_kNewPos += kForward* 0.1;
			//cout<<"taking a longer step"<<endl;
		}
	
	}
//		cout<<"old    pos:"<<kOldPos.x<<" "<<kOldPos.y<<" "<<kOldPos.z<<endl;
//		cout<<"curent pos:"<<pkPP->GetObject()->GetPos().x<<" "<<pkPP->GetObject()->GetPos().y<<" "<<pkPP->GetObject()->GetPos().z<<endl;			
//		cout<<"next   pos:"<<pkPP->m_kNewPos.x<<" "<<pkPP->m_kNewPos.y<<" "<<pkPP->m_kNewPos.z<<endl;						
			
//		cout<<"newpos : "<<pkCD->m_kPos2.y<<endl;
//		cout<<"kOldPos: "<<kOldPos.y<<endl;
		
//		cout<<"Step height: "<<(pkCD->m_kPos2.y - pkPP->m_kNewPos.y)<<endl;		
			
//		if((pkCD->m_kPos2.y - pkPP->m_kNewPos.y)!=0)

//		{	
//			cout<<"Step height: "<<(pkCD->m_kPos2.y - pkPP->m_kNewPos.y)<<endl;
//			pkPP->GetObject()->GetPos() = pkCD->m_kPos2;
//			pkPP->m_kNewPos = pkCD->m_kPos2;
		
//			Vector3 diff = kOldNewPos - kOldPos;
//			if(diff.Length() != 0){
//				pkPP->GetObject()->GetPos() += diff.Unit() * fStep;
//				pkPP->m_kNewPos += diff.Unit() * fStep;
//			}
			
//			cout<<"upp uppp"<<endl;
//			delete pkCD;			
//			return true;		
//		}
//	}	

//	cout<<"walking"<<endl;

//	pkPP->m_kNewPos = kOldNewPos;
//	pkPP->GetObject()->GetPos() = kOldPos;
*/
	delete pkCD;
	return true;
}

Collision* PhysicsEngine::CheckIfColliding(P_Physic* pkPP)
{
	Collision* pkCD = NULL; 
	Collision* pkClosest = NULL;
	float fMinDis=99999999;

	
	P_Physic* PP1 = pkPP;			
	
	for(vector<Property*>::iterator it2 = m_kPropertys.begin();it2!=m_kPropertys.end();it2++) 
	{	
		//dont collide with our self 
		if(PP1==*it2)
			continue;
			
			
		P_Physic* PP2 = static_cast<P_Physic*>(*it2);

		if(!PP2->m_bSolid)
			continue;

		//dont collide two static objects =)
		if(PP1->m_bStatic && PP2->m_bStatic)
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





