// Korv.cpp: implementation of the Korv class.
//
//////////////////////////////////////////////////////////////////////

#include "Korv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Korv::Korv()
{
Storlek=3;
Skin=true;
Status="stinkig";
Vek.set(1.0,2.0,3.0, 4.0);
}

Korv::~Korv()
{

}

vector<Mat::PropertyValues> Korv::GetPropertyValues()
{
vector<Mat::PropertyValues> kReturn(3);

kReturn[0].kValueName="Storlek";
kReturn[0].iValueType=VALUETYPE_INT;
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


return kReturn;
};