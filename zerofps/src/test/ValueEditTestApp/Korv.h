// Korv.h: interface for the Korv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KORV_H__1C2A8B4F_306B_4E8C_946F_2914CBB65A1F__INCLUDED_)
#define AFX_KORV_H__1C2A8B4F_306B_4E8C_946F_2914CBB65A1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mat.h"
#include <string>
#include <vector>
using namespace std;

class Korv :public Mat  
{
public:
	
	Korv();
	virtual ~Korv();
	vector<PropertyValues> GetPropertyValues();
	
private:
	bool Skin;
	int Storlek;
	string Status;
	Vector3 Vek;
};

#endif // !defined(AFX_KORV_H__1C2A8B4F_306B_4E8C_946F_2914CBB65A1F__INCLUDED_)
