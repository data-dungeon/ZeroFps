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

class HeightMap;
class PathFind;

class PathFindBuilder : public ZFObject
{
public:
	Point GetMapTile(Vector3 pos);
	void Build();
	PathFindBuilder(HeightMap* pkHeightMap, PathFind** m_ppkPathFind);
	~PathFindBuilder();

private:
	HeightMap* m_pkHeightMap;
	PathFind* m_pkPathFind;
	int* m_piCostMap;
};

#endif // !defined(AFX_PATHFINDBUILDER_H__CA733ED4_A7A8_48B7_9A42_DEC57BBA575C__INCLUDED_)
