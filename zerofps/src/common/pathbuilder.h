// pathfindbuilder.h: interface for the PathFindBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHFINDBUILDER_H__CA733ED4_A7A8_48B7_9A42_DEC57BBA575C__INCLUDED_)
#define AFX_PATHFINDBUILDER_H__CA733ED4_A7A8_48B7_9A42_DEC57BBA575C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common_x.h"
#include "../zerofps/basic/basic.pkg"

#include <map>
#include <string>
using namespace std;

class HeightMap;
class PathFind;

class COMMON_API PathBuilder : public ZFObject
{
public:
	Point GetSqrFromPos(Vector3 pos);
	Vector3 GetPosFromSqr(Point square);
	void Build(int pkObjectTypeCost[5], float fMaxAngle=40.0f);
	PathBuilder(HeightMap* pkHeightMap, PathFind** m_ppkPathFind);
	~PathBuilder();

private:
	string GenerateKey(int akCosts[5]);
	HeightMap* m_pkHeightMap;
	PathFind** m_ppkPathFind;

	static int* m_piTerrain;
	static map< string, int* > m_akCostMaps;
};

#endif // !defined(AFX_PATHFINDBUILDER_H__CA733ED4_A7A8_48B7_9A42_DEC57BBA575C__INCLUDED_)
