#include "cssphere.h"

CSSphere::CSSphere(float fRadius)
{
	m_fRadius=fRadius;
}

CollisionData* CSSphere::Test(CollisionShape* kOther,float fTime,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CSSphere)){
		CSSphere *kCs=dynamic_cast<CSSphere*>(kOther);
		
		return Collide_CSSphere(kCs,fTime);		
	} else if(bContinue)
	{
		return kOther->Test(this,fTime,false);
	};
	
	cout<<"Unhandled collision"<<endl;
	return NULL;
}

CollisionData* CSSphere::Collide_CSSphere(CSSphere* kOther,float fTime)
{
	Object* O1=m_pkPP->GetObject();
	Object* O2=kOther->m_pkPP->GetObject();

	//caluculate movevector for both spheres
	Vector3 movevec1=m_pkPP->m_kNewPos - O1->GetPos();//-     m_pkPhysEngine->GetNewVel(m_pkPP);
	Vector3 movevec2=kOther->m_pkPP->m_kNewPos - O2->GetPos();//m_pkPhysEngine->GetNewVel(kOther->m_pkPP);
	
	//relative move vector
	Vector3 movevec = movevec1-movevec2;
	
	//if both spheres velocity is the same they cant collide
	if(movevec==Vector3(0,0,0))
		return NULL;
	

	//sum of the two spheres radius
	double sumRadii = (m_fRadius + kOther->m_fRadius);
	
	//normalize a movment vetor
	Vector3 N = movevec;
	N.Normalize();
	
	//center of a to center of b
	Vector3 C = O2->GetPos() - O1->GetPos();
	
	double D = N.Dot(C);
	
	// Another early escape: Make sure that A is moving
	// towards B! If the dot product between the movevec and
	// B.center - A.center is less that or equal to 0,
	// A isn't isn't moving towards B
	if(D<=0)
		return NULL;
				
	// Find the length of the vector C
	double lengthC = C.Length();
	
	double F = (lengthC * lengthC) - (D * D);
	
	// Escape test: if the closest that A will get to B
	// is more than the sum of their radii, there's no
	// way they are going collide
	double sumRadiiSquared = sumRadii * sumRadii;
	if(F >= sumRadiiSquared){
	  return NULL;
	}
	
	// We now have F and sumRadii, two sides of a right triangle.
	// Use these to find the third side, sqrt(T)
	double T = sumRadiiSquared - F;
	
	// If there is no such right triangle with sides length of
	// sumRadii and sqrt(f), T will probably be less than 0.
	// Better to check now than perform a square root of a
	// negative number.
	if(T < 0){
		return NULL;
	}
	
	// Therefore the distance the circle has to travel along
	// movevec is D - sqrt(T)
	double distance = D - sqrt(T);
	
	// Get the magnitude of the movement vector
	double mag = movevec.Length();
	
	// Finally, make sure that the distance A has to move
	// to touch B is not greater than the magnitude of the
	// movement vector.
	if(mag < distance){
	  return NULL;
	}
	
//	movevec.Normalize();
// movevec*=distance;

//	cout<<"distance:"<<distance<<endl;

//	cout<<"OLD MV:"<<movevec.Length()<<endl;

	float bla = distance / abs(movevec.Length());
	if(bla<0)
		return NULL;
//	cout<<"Förhållande "<<bla<<endl;
			
//	O1->GetVel()=movevec1*bla;
//	O2->GetVel()=movevec2*bla;
	
	
	//assemble collision data
	CollisionData* tempdata = new CollisionData;
/*	
	tempdata->m_pkOther = O2;
	tempdata->m_kOtherPos = O2->GetPos()+(movevec2*bla);
	tempdata->m_kOtherVel = movevec2*bla;
	tempdata->m_kOtherAcc = O2->GetAcc();
	tempdata->m_kOtherRot = O2->GetRot();
	
	tempdata->m_pkThis = O1;
	tempdata->m_kPos = O1->GetPos()+(movevec1*bla);
	tempdata->m_kVel = movevec1*bla;
	tempdata->m_kAcc = O1->GetAcc();
	tempdata->m_kRot = O1->GetRot();
*/	
	
	tempdata->m_pkPP2 = kOther->m_pkPP;
	tempdata->m_kPos2 = O2->GetPos()+(movevec2*bla);
	tempdata->m_kVel2 = movevec2*bla;
	tempdata->m_kAcc2 = O2->GetAcc();
	tempdata->m_kRot2 = O2->GetRot();
	
	tempdata->m_pkPP1 = m_pkPP;
	tempdata->m_kPos1 = O1->GetPos()+(movevec1*bla);
	tempdata->m_kVel1 = movevec1*bla;
	tempdata->m_kAcc1 = O1->GetAcc();
	tempdata->m_kRot1 = O1->GetRot();

	tempdata->m_kNormal=Vector3(0,0,0);
	
	return tempdata;
}


