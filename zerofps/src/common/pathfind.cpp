// pathfind.cpp: implementation of the PathFind class.
// blub

//
//////////////////////////////////////////////////////////////////////

#include "pathfind.h"
#include "tileengine.h"

int PathFind::m_siMapWidth = 255;

static int TEMP_TERRAIN[65536];

static Point g_kFrom, g_kTo;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PathFind::PathFind(int* piMapTerrain, int iMapWidth, unsigned int uiBlockedValue) 
	: ZFObject("PathFind"), m_pkSearchNode( NULL ), BLOCKED_VALUE(uiBlockedValue)
{
	m_piMapTerrain = piMapTerrain;
	m_siMapWidth = iMapWidth;
	m_bPathIsReversed = false;
}

PathFind::~PathFind()
{

}

bool PathFind::Rebuild(int iStartPosX, int iStartPosY, int iDestPosX, int iDestPosY)
{
	//vector<Tile>* pkTiles = TileEngine::m_pkInstance->GetTilesPointer();

	if(m_bPathIsReversed == false)
	{
		g_kFrom = Point(iStartPosX, iStartPosY);
		g_kTo = Point(iDestPosX, iDestPosY);
	}

	memcpy(TEMP_TERRAIN, m_piMapTerrain, 
		sizeof(int)*(m_siMapWidth*m_siMapWidth)); // kopiera den riktiga terrängen till en temp variabel.

	int l = min(iStartPosX, iDestPosX); // 
	int t = max(iStartPosY, iDestPosY); // T---R
 	int r = max(iStartPosX, iDestPosX); // |   |
	int b = min(iStartPosY, iDestPosY); // L---B

	for(int y=1; y<m_siMapWidth+1; y++)
		for(int x=1; x<m_siMapWidth+1; x++)
		{
			Tile* pkTile = TileEngine::m_pkInstance->GetTile(x-1,y-1);
			if( pkTile != NULL && pkTile->kUnits.size() > 0)
			{
				TEMP_TERRAIN[y*m_siMapWidth+x] = BLOCKED_VALUE;
			}
		}

	if(ImpossibleToReach(iStartPosX, iStartPosY, iDestPosX, iDestPosY))
		return false;

	if(m_bPathIsReversed)
	{
		// Byt start och slut ruta så att sökvägen byggs upp
		// åt andra hållet. Detta är en nödvändig optimering
		// för att det skall gå fortare för algorithmen att
		// upptäcka om man har klickat på en nod som inte går att
		// nå (en ruta omringad av blockerad terrän, tex. en ö)
		swap(iStartPosX, iDestPosX);
		swap(iStartPosY, iDestPosY);

		swap(g_kFrom, g_kTo);
	}

	m_pkStartNode = new Node(iStartPosX, iStartPosY);
	m_pkDestNode = new Node(iDestPosX, iDestPosY);

	m_eState = ACTIVE;

	// Beräkna den heuristiska kostnaden
	m_pkStartNode->m_fGValue = 0; 	
	m_pkStartNode->m_fHValue = m_pkStartNode->GetHeuristicCost(m_pkDestNode->m_kSqrPos);
	m_pkStartNode->m_fFValue = m_pkStartNode->m_fGValue + m_pkStartNode->m_fHValue;
	m_pkStartNode->m_pkParent = 0;

	// Push the start node on the Open list
	m_kvOpenList.push_back( m_pkStartNode ); // heap now unsorted

	// Sort back element into heap
	push_heap( m_kvOpenList.begin(), m_kvOpenList.end(), HeapComp() );

	// Fill a queue with all (x,y) positions 
	// from the steps from start to end
	return FillQueue();
}

bool PathFind::GetNextStep(int &riSquareX, int &riSquareY)
{
	if(m_kqPath.size() > 0)
	{
		Point sqr = m_kqPath.front();
		riSquareX = sqr.x; 
		riSquareY = sqr.y;
		m_kqPath.pop();
		return true;
	}
	
	return false;
}

PathFind::SEARCH_STATE PathFind::SearchStep()
{
	// Avbryt om sökningen är avklarad.
	if( m_eState == FOUND_DESTINATION )
		return m_eState; 

	// Avbryt om listan är tom
	// dvs. vi har inte hittat någon
	// slut position.
	if( m_kvOpenList.empty() )
	{
		FreeAllNodes();
		m_eState = CANT_REACH_DESTINATION;
		return m_eState;
	}
	
	// Hämta den nod som är bäst, som har lägst f, 
	// och ta bort den ur den öppna listan. 
	NodePtr pkNode = m_kvOpenList.front();
	pop_heap( m_kvOpenList.begin(), m_kvOpenList.end(), HeapComp() );
	m_kvOpenList.pop_back();

	// Kolla om vi har nått slut destinationen.
	if(	pkNode->m_kSqrPos.x == m_pkDestNode->m_kSqrPos.x && 
		pkNode->m_kSqrPos.y == m_pkDestNode->m_kSqrPos.y)
	{
		// Lagra undan slut noden.
		m_pkDestNode->m_pkParent = pkNode->m_pkParent;

		// Om destinations noden och start noden INTE är lika.
		if( pkNode != m_pkStartNode )
		{
			delete pkNode;

			// Tilldela en child pekare till varje nod som har ett barn
			NodePtr pkChildNode = m_pkDestNode;
			NodePtr pkParentNode = m_pkDestNode->m_pkParent;

			do 
			{
				pkParentNode->m_pkChild = pkChildNode;
				pkChildNode = pkParentNode;
				pkParentNode = pkParentNode->m_pkParent;
			} 
			while( pkChildNode != m_pkStartNode ); // Start is always the first node by definition
		}

		DeleteUnusedNodes();
		m_eState = FOUND_DESTINATION;
		return m_eState;
	}
	// Vi har inte kommit till slutet ännu.
	// Lägg till grannar.
	else 
	{
		// Börja med att tömma vektorn över alla grannar.
		m_kvNeighbours.clear(); 

		int iParentX = -1; 
		int iParentY = -1; 
		int x = pkNode->m_kSqrPos.x;
		int y = pkNode->m_kSqrPos.y;

		Point* kParentSquare;
		if(pkNode->m_pkParent)
			kParentSquare = &pkNode->m_pkParent->m_kSqrPos;
		else
			kParentSquare = NULL;

		// Hämta föräldernas x och y position som vi behöver ha 
		// och jämnföra med.
		if(kParentSquare)
		{
			iParentX = kParentSquare->x;
			iParentY = kParentSquare->y;
		}
		
		Point kNewSquare;

		// Lägg till alla grannar som är OK
		if((GetTerrainCost( x-1, y ) < BLOCKED_VALUE) && !( (iParentX == x-1) && (iParentY == y))) 
		{
			kNewSquare = Point(x-1, y);
			AddNeighbour( kNewSquare );
		}	
		if((GetTerrainCost( x, y-1 ) < BLOCKED_VALUE) && !( (iParentX == x) && (iParentY == y-1))) 
		{
			kNewSquare = Point(x, y-1);
			AddNeighbour( kNewSquare );
		}	
		if((GetTerrainCost( x+1, y ) < BLOCKED_VALUE) && !( (iParentX == x+1) && (iParentY == y))) 
		{
			kNewSquare = Point(x+1, y);
			AddNeighbour( kNewSquare );
		}	
		if((GetTerrainCost( x, y+1 ) < BLOCKED_VALUE) && !( (iParentX == x) && (iParentY == y+1)))
		{
			kNewSquare = Point(x, y+1);
			AddNeighbour( kNewSquare );
		}	

		// Nu undersöker vi varje granne...
		vector<NodePtr>::iterator itNeighbour;

		for( itNeighbour = m_kvNeighbours.begin(); 
		     itNeighbour != m_kvNeighbours.end(); 
			 itNeighbour ++ )
		{
			// Hämta g-värdet från noden, som är kostnaden för hur långt
			// tid det tog att ta sig till denna nod från startnoden.
			float fNewGValue = pkNode->m_fGValue + (float)
				GetTerrainCost(pkNode->m_kSqrPos.x, pkNode->m_kSqrPos.y);

			// Kolla om vi redan har undersökt denna granne, dvs. om den redan
			// finns i någon av de 2 listorna. Gör den det och har lägre g värde
			// så skippar vi denna granne.
			vector<NodePtr>::iterator itOpenListResultat;
			for( itOpenListResultat = m_kvOpenList.begin(); 
			     itOpenListResultat != m_kvOpenList.end(); 
				 itOpenListResultat ++ )
			{
				if( (*itOpenListResultat)->m_kSqrPos.x == (*itNeighbour)->m_kSqrPos.x &&
					(*itOpenListResultat)->m_kSqrPos.y == (*itNeighbour)->m_kSqrPos.y)
					break;				
			}

			// Om loopen har avbrytits så har vi hittat elementet i den öppna listan.
			if( itOpenListResultat != m_kvOpenList.end() )
			{
				// Om elementets g-värde är lägre så tar vi
				// bort grannen och hoppar till nästa loop varv.
				if( (*itOpenListResultat)->m_fGValue <= fNewGValue )
				{
					delete (*itNeighbour);
					continue;
				}
			}

			vector< NodePtr >::iterator itClosedListResult;
			for( itClosedListResult = m_kvClosedList.begin(); 
			     itClosedListResult != m_kvClosedList.end(); 
				 itClosedListResult ++ )
			{
				if( (*itClosedListResult)->m_kSqrPos.x == (*itNeighbour)->m_kSqrPos.x &&
					(*itClosedListResult)->m_kSqrPos.y == (*itNeighbour)->m_kSqrPos.y)
					break;			
			}

			if( itClosedListResult != m_kvClosedList.end() )
			{
				// we found this state on closed
				if( (*itClosedListResult)->m_fGValue <= fNewGValue )
				{
					// the one on Closed is cheaper than this one
					delete (*itNeighbour);
					continue;
				}
			}

			// This node is the best node so far with this particular state
			// so lets keep it and set up its AStar specific data ...
			(*itNeighbour)->m_pkParent = pkNode;
			(*itNeighbour)->m_fGValue = fNewGValue;
			(*itNeighbour)->m_fHValue = (*itNeighbour)->GetHeuristicCost( m_pkDestNode->m_kSqrPos );
			(*itNeighbour)->m_fFValue = (*itNeighbour)->m_fGValue + (*itNeighbour)->m_fHValue;

			// Remove successor from closed if it was on it
			if( itClosedListResult != m_kvClosedList.end() )
			{
				// remove it from Closed
				delete (*itClosedListResult);
				m_kvClosedList.erase( itClosedListResult );
			}

			// Update old version of this node
			if( itOpenListResultat != m_kvOpenList.end() )
			{	   
				delete (*itOpenListResultat);
			   	m_kvOpenList.erase( itOpenListResultat );

				// Bygg om hela heapen
				make_heap( m_kvOpenList.begin(), m_kvOpenList.end(), HeapComp() );
			}

			// Lägg till ersättaren
			m_kvOpenList.push_back( (*itNeighbour) );

			// Sortera listan i heap-ordning
			push_heap( m_kvOpenList.begin(), m_kvOpenList.end(), HeapComp() );
		}

		// push pkNode onto Closed, as we have expanded it now
		m_kvClosedList.push_back( pkNode );

	} // end else (not goal so expand)

 	return m_eState; 
}

bool PathFind::AddNeighbour( Point &kNewSquare )
{
	NodePtr pkNode = new Node;

	if( pkNode )
	{
		pkNode->m_kSqrPos = kNewSquare;
		m_kvNeighbours.push_back( pkNode );
		return true;
	}

	return false;
}

void PathFind::FreeSolutionNodes()
{
	NodePtr pkNode = m_pkStartNode;

	if( m_pkStartNode->m_pkChild )
	{
		do
		{
			NodePtr pkNodeToDelete = pkNode;
			pkNode = pkNode->m_pkChild;
			delete pkNodeToDelete;
			pkNodeToDelete = NULL;

		} while( pkNode != m_pkDestNode );

		delete pkNode;
	}
	else
	{
		// if the start node is the solution we 
		// need to just delete the start and goal
		// nodes
		delete m_pkStartNode;
		delete m_pkDestNode;
	}
}

PathFind::NodePtr PathFind::GetSolutionStart()
{
	m_pkSearchNode = m_pkStartNode;

	if(m_pkStartNode)
		return m_pkStartNode;
	else
		return NULL;
}

PathFind::NodePtr PathFind::GetSolutionNext()
{
	if( m_pkSearchNode )
	{
		if( m_pkSearchNode->m_pkChild )
		{
			NodePtr pkChild, pkNextNext;

			if((pkNextNext=GetNextNextDiagonalStep(m_pkSearchNode)))
				pkChild = pkNextNext;
			else
				pkChild = m_pkSearchNode->m_pkChild;

			m_pkSearchNode = pkChild;
			return pkChild;
		}
	}

	return NULL;
}

void PathFind::FreeAllNodes()
{
	// iterate open list and delete all nodes
	vector<NodePtr>::iterator itOpen = m_kvOpenList.begin();
	while( itOpen != m_kvOpenList.end() )
	{
		NodePtr pkNode = (*itOpen);
		delete pkNode;
		itOpen++;
	}

	m_kvOpenList.clear();

	// iterate closed list and delete unused nodes
	vector<NodePtr>::iterator itClosed;
	for( itClosed = m_kvClosedList.begin(); 
	     itClosed != m_kvClosedList.end(); 
		 itClosed ++ )
	{
		NodePtr pkNode = (*itClosed);
		delete pkNode;
	}

	m_kvClosedList.clear();
}



// Name:		DeleteUnusedNodes
// Description:	Delete nodes in the open and closed list 
//				that aren't needed for the solution.
//
void PathFind::DeleteUnusedNodes()
{
	vector<NodePtr>::iterator itOpen = m_kvOpenList.begin();
	while(itOpen != m_kvOpenList.end())
	{
		NodePtr pkNode = (*itOpen);

		if(!pkNode->m_pkChild )
		{
			delete pkNode;
			pkNode = NULL;
		}

		itOpen ++;
	}

	m_kvOpenList.clear();

	vector<NodePtr>::iterator itClosed;
	for(itClosed = m_kvClosedList.begin(); 
	    itClosed != m_kvClosedList.end(); 
		itClosed ++ )
	{
		NodePtr pkNode = (*itClosed);

		if(!pkNode->m_pkChild)
		{
			delete pkNode;
			pkNode = NULL;
		}
	}

	m_kvClosedList.clear();
}



// Name:		GetHeuristicCost
// Description:	Get´s a value that is a estimate of how far we 
//				are from the destination.
//
float PathFind::Node::GetHeuristicCost( Point nodeGoal )
{
	float fDistanceX = float( ( (float)m_kSqrPos.x - (float)nodeGoal.x) );
	float fDistanceY = float( ( (float)m_kSqrPos.y - (float)nodeGoal.y) );
	return ((fDistanceX*fDistanceX) + (fDistanceY*fDistanceY));
}



// Name:		GetTerrainCost
// Description:	Gets a cost value from a square in the map terrain array.
//
int PathFind::GetTerrainCost(int x, int y)
{
	if( x < 0 || x >= m_siMapWidth ||
		y < 0 || y >= m_siMapWidth)
	{
		return BLOCKED_VALUE;	 
	}

	return TEMP_TERRAIN[y*m_siMapWidth+x];
}



// Name:		FillQueue
// Description:	Fills a queue with Point´s that forms a grid path
//				of leagal moves between Start and Dest node.
//				Return false if the destintion was not found.
//
bool PathFind::FillQueue()
{
	// Börja med att tömma kön.
	while(m_kqPath.size())
		m_kqPath.pop();

	SEARCH_STATE uiSearchState;

	static int counter=0, max=(m_siMapWidth/6)*(m_siMapWidth/6);

	do
	{
		if(counter++ > max)
		{
			counter=0;
			Reset();
			m_bPathIsReversed = true;
			printf("Swaping startpoint and endpoint and searching again\n");
			return Rebuild(g_kFrom.x, g_kFrom.y, g_kTo.x, g_kTo.y);
		}

		uiSearchState = SearchStep();
	} while(uiSearchState == ACTIVE);

	if(uiSearchState == FOUND_DESTINATION)
	{
		NodePtr pkNode = GetSolutionStart();

		Point kSqr(pkNode->m_kSqrPos.x, pkNode->m_kSqrPos.y);
		m_kqPath.push(kSqr);
	
		while(1)
		{
			pkNode = GetSolutionNext();
			if( !pkNode )
				break;

			kSqr.x = pkNode->m_kSqrPos.x;
			kSqr.y = pkNode->m_kSqrPos.y;
			m_kqPath.push(kSqr);
		};

		FreeSolutionNodes();
	}
	else
	{
		return false;
	}

	if(m_bPathIsReversed)
		ReversePath();  // Eftersom vi byggde upp sökvägen från andra hållet
						// måste vi nu vända på kön.
	return true;
}



// Name:		Reset
// Description:	Public function that the user can call to abort and destroy a path.
//
void PathFind::Reset()
{
	FreeAllNodes();

	while(m_kqPath.size())
		m_kqPath.pop();
}



// Name:		GetNextNextDiagonalStep
// Description: Tests if the next-next step is a square that is in a diagonal
//				location according to pkNode, and if so return that node.
//
PathFind::NodePtr PathFind::GetNextNextDiagonalStep(PathFind::NodePtr pkNode)
{
	int iXPos = pkNode->m_kSqrPos.x;
	int iYPos = pkNode->m_kSqrPos.y;

	if(pkNode->m_pkChild == NULL)
		return NULL;
	if(pkNode->m_pkChild->m_pkChild == NULL)
		return NULL;

	NodePtr pkNextNext = pkNode->m_pkChild->m_pkChild;
	if(pkNextNext->m_kSqrPos.x == iXPos+1 && pkNextNext->m_kSqrPos.y == iYPos+1)
		return pkNextNext;
	if(pkNextNext->m_kSqrPos.x == iXPos-1 && pkNextNext->m_kSqrPos.y == iYPos+1)
		return pkNextNext;
	if(pkNextNext->m_kSqrPos.x == iXPos-1 && pkNextNext->m_kSqrPos.y == iYPos-1)
		return pkNextNext;
	if(pkNextNext->m_kSqrPos.x == iXPos+1 && pkNextNext->m_kSqrPos.y == iYPos-1)
		return pkNextNext;

	return NULL;
}

bool PathFind::ImpossibleToReach(int iStartPosX, int iStartPosY, int iDestPosX, int iDestPosY)
{
	// Är rutan i sig blockerad
	if(GetTerrainCost(iDestPosX, iDestPosY) == BLOCKED_VALUE)
		return true;
/*
	if( GetTerrainCost(iDestPosX+1, iDestPosY) == BLOCKED_VALUE
		&& GetTerrainCost(iDestPosX, iDestPosY+1) == BLOCKED_VALUE
		&& GetTerrainCost(iDestPosX-1, iDestPosY) == BLOCKED_VALUE
		&& GetTerrainCost(iDestPosX, iDestPosY-1) == BLOCKED_VALUE )
	{
		printf("PathFind::ImpossibleToReach!\n"); 
		return true;
	}
*/
	return false;
}

void PathFind::ReversePath()
{
	int size = m_kqPath.size();

	if(size <= 1)
		return;

	vector<Point> apa;
	apa.reserve(size);

	int i;
	for(i=0; i<size; i++)
	{
		apa.push_back(m_kqPath.front());
		m_kqPath.pop();
	}

	for(i=0; i<size; i++)
		m_kqPath.push(apa[size-1-i]);

	m_bPathIsReversed = false;
}