#ifndef _P_DMCLICKME_H_
#define _P_DMCLICKME_H_

#include "../zerofpsv2/engine/property.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

class Visiter
{
public:
	int m_iVisiter;
	float m_fVisitTime; // finished when 0 reached
};


class MCOMMON_API P_DMClickMe: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();

		// house stuff
		list<Visiter> m_kVisiters;

		void *m_pkScript;
	
	public:
		// lite fulhackigt
		bool m_bIsHouse;

		void Click();
		void Click(int iObjID);

		// house stuff
		void AddVisiter ( int iObjID, float fVisitTime );


		P_DMClickMe();
		~P_DMClickMe();

	
		void Update();
		void Init();	
	
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);		
};

MCOMMON_API Property* Create_P_DMClickMe();

#endif


