#ifndef _ENGINE_P_CONTAINER_H_
#define _ENGINE_P_CONTAINER_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include <vector>

using namespace std;


/**	\brief	Da P_Container
		\ingroup Common
*/
class MCOMMON_API P_Container: public Property 
{
	private:

	public:
		P_Container();


};

MCOMMON_API Property* Create_P_Container();

#endif










