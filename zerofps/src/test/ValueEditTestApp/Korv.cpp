// Korv.cpp: implementation of the Korv class.
//
//////////////////////////////////////////////////////////////////////

#include "Korv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Korv::Korv()
{
Storlek=1231231233.3111125435435435;
Skin=true;
Status="stinkig";
Vek.set(1.3,2.1,-0.01, 1.0);
TestVek.push_back(44);
TestVek.push_back(1);
TestVek.push_back(5);
TestVek.resize(15);
}


Korv::~Korv()
{

}


vector<Mat::PropertyValues> Korv::GetPropertyValues()
{

	cout<< "stinky" <<endl;
	vector<Mat::PropertyValues> kReturn(4);

kReturn[0].kValueName="Storlek";
kReturn[0].iValueType=VALUETYPE_FLOAT;
kReturn[0].pkValue=(void*)&Storlek;
//kReturn[0].fUpperBound=2.0;
//kReturn[0].fLowerBound=-6.0;

kReturn[1].kValueName="Vek";
kReturn[1].iValueType=VALUETYPE_VECTOR4;
kReturn[1].pkValue=(void*)&Vek;



kReturn[2].kValueName="Status";
kReturn[2].iValueType=VALUETYPE_STRING;
kReturn[2].pkValue=(void*)&Status;
kReturn[2].fUpperBound=5.0;
kReturn[2].fLowerBound=2.0;

kReturn[3].kValueName="TestVek";
kReturn[3].iValueType=VALUETYPE_INT;
kReturn[3].pkValue=(void*)&TestVek;
//out <<"TestVek.size:" <<TestVek.size() <<endl; 
kReturn[3].iNumberOfValues=TestVek.size();
kReturn[3].fUpperBound=5.0;


return kReturn;
};