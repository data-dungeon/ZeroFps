#include "collisionheightmap.h"

CollisionHeightMap::CollisionHeightMap(HeightMap *pkHeightMap)
{
	m_pkHM=pkHeightMap;
}

bool CollisionHeightMap::Collide(CollisionObject *kOther,Vector3 *pkPos,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CollisionSphere)){
		CollisionSphere *kCs=dynamic_cast<CollisionSphere*>(kOther);
		return CollideSphere(kCs,pkPos);

	} else if(typeid(*kOther)==typeid(CollisionPoint)){
		CollisionPoint *kCp = dynamic_cast<CollisionPoint*>(kOther);			
		return CollidePoint(kCp,pkPos);
		
	} else if(bContinue){
//		cout<<"Unhandled Collision,Asking kOther"<<endl;	
		return kOther->Collide(this,pkPos,false);
	
	}
		
	return false;
}

bool CollisionHeightMap::CollideSphere(CollisionSphere *pkCs,Vector3 *pkPos){

	//first check that the spehre is on the heightmap
	if( pkCs->m_kPos->x - *pkCs->m_fRadius >= m_pkHM->m_kPosition.x  &&  pkCs->m_kPos->x + *pkCs->m_fRadius <= m_pkHM->m_kPosition.x+m_pkHM->m_iHmSize )
	{
		if( pkCs->m_kPos->z - *pkCs->m_fRadius >= m_pkHM->m_kPosition.z  &&  pkCs->m_kPos->z + *pkCs->m_fRadius <= m_pkHM->m_kPosition.z+m_pkHM->m_iHmSize )
		{
			//then check if we are under the hegihtmap			
			if( pkCs->m_kPos->y - *pkCs->m_fRadius <= m_pkHM->Height( pkCs->m_kPos->x , pkCs->m_kPos->z ))
			{
				*pkPos = Vector3(pkCs->m_kPos->x,*pkCs->m_fRadius+m_pkHM->Height( pkCs->m_kPos->x , pkCs->m_kPos->z ),pkCs->m_kPos->z);				
				return true;			
			}
		}
	}
	return false;
}

bool CollisionHeightMap::CollidePoint(CollisionPoint *pkCp,Vector3 *pkPos){
	//first check that the point is on the heightmap	
	if( pkCp->m_kPos->x >= m_pkHM->m_kPosition.x  &&  pkCp->m_kPos->x <= m_pkHM->m_kPosition.x+m_pkHM->m_iHmSize )
	{
		if( pkCp->m_kPos->z >= m_pkHM->m_kPosition.z  &&  pkCp->m_kPos->z <= m_pkHM->m_kPosition.z+m_pkHM->m_iHmSize )
		{
			//then check if we are under the hegihtmap
			if( pkCp->m_kPos->y <= m_pkHM->Height( pkCp->m_kPos->x , pkCp->m_kPos->z ))
			{
				*pkPos = Vector3(pkCp->m_kPos->x,m_pkHM->Height( pkCp->m_kPos->x , pkCp->m_kPos->z ),pkCp->m_kPos->z);
				return true;
			}
		}
	}
	return false;
}


