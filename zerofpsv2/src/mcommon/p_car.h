#ifndef _P_CAR_H_
#define _P_CAR_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "mcommon_x.h"

#include "../zerofpsv2/engine/inputhandle.h"
#include "../zerofpsv2/engine_systems/propertys/p_tcs.h"

using namespace std;

class MCOMMON_API P_Car : public Property
{
	private:
		InputHandle*	m_pkInputHandle;

		bitset<6>		m_kControls;

		void Trust(Vector3 kPos,Vector3 kForce);
		Vector3 LineTest(Vector3 kStart,Vector3 kStop);

	public:

		P_Car();
		~P_Car();

		void CloneOf(Property* pkProperty) { }

		void Update();
		void Init();

		//void Save(ZFIoInterface* pkPackage);
		//void Load(ZFIoInterface* pkPackage);

};

MCOMMON_API Property* Create_P_Car();

#endif



