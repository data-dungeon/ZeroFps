// pathfindbuilder.h: interface for the PathFindBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHFINDBUILDER_H__CA733ED4_A7A8_48B7_9A42_DEC57BBA575C__INCLUDED_)
#define AFX_PATHFINDBUILDER_H__CA733ED4_A7A8_48B7_9A42_DEC57BBA575C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "engine_x.h"
#include "../basic/basic.pkg"

#include <map>
#include <string>
using namespace std;

class HeightMap;
class PathFind;

class ENGINE_API PathBuilder : public ZFObject
{
public:
	Point GetSqrFromPos(Vector3 pos);
	Vector3 GetPosFromSqr(Point square);
	void Build(int pkObjectTypeCost[5]);
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
