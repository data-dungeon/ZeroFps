#ifndef _P_PROJECTILE_H_
#define _P_PROJECTILE_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"

using namespace std;

/**	\brief	Da P_Projectile
		\ingroup Common
*/
class MCOMMON_API P_Projectile: public Property {
	private:
		int	m_iOwnerID;	
		bool	m_bDidHit;
	
		vector<PropertyValues> GetPropertyValues();
	public:


		P_Projectile();
// 		~P_Projectile();
		
// 		void Update();
		void Init();
		void Touch(int iId);
		
};

MCOMMON_API Property* Create_P_Projectile();
void MCOMMON_API Register_P_Projectile(ZSSZeroFps* pkZeroFps);

#endif
