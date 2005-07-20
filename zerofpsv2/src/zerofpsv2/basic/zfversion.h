#ifndef _BASIC_ZFVERSION_H_
#define _BASIC_ZFVERSION_H_

#include <string>
#include "basic_x.h"

using namespace std;

/**	\brief	Zerofps version number.

	The version number is made up of three parts a.b.c.

    * a: The first number is the primary version number. The one usally find on a software product. The current 
		version of mistlands will use 0 and version 1 will be when hack/slash is done.
    * b: The milestone number.
    * c: A number that we increase with each test release, the build number.
*/
class BASIC_API ZFVersion
{
public:
	int	m_iMajor;	//	m_iMajor.m_iMinor.m_iBuild
	int	m_iMinor;
	int	m_iBuild;

	ZFVersion();		
	ZFVersion(int iMajor, int iMinor, int iBuild);
	~ZFVersion();

	void Set(int iMajor, int iMinor, int iBuild);

	bool operator==(const ZFVersion &kOther)	const;
	bool operator!=(const ZFVersion &kOther)	const;

};

#endif

