#include "p_clientcontrol.h"
#include "p_container.h"
#include "p_item.h"
#include "../zerofpsv2/engine/entitymanager.h"

// -----------------------------------------------------------------------------------------------

P_Container::P_Container()
{

}

Property* Create_P_Container()
{
	return new P_Container;
}
