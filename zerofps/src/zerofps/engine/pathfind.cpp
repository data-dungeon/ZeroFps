// pathfind.cpp: implementation of the PathFind class.
//
//////////////////////////////////////////////////////////////////////

#include "pathfind.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PathFind::PathFind(int* piMapTerrain, int iMapWidth, unsigned int uiBlockedValue) 
	: ZFObject("PathFind") , m_pkSearchNode( NULL ), BLOCKED_VALUE(uiBlockedValue)
{
	m_piMapTerrain = piMapTerrain;
	m_iMapWidth = iMapWidth;
}

PathFind::~PathFind()
{

}

///////////////////////////////////////////////////////////////////////
// Name:		Reset
// Description:	Public function that the user can call to abort and destroy the path.
//
void PathFind::Reset()
{
	DeleteAllNodes();
	while(m_kqPath.size())
		m_kqPath.pop();
}

///////////////////////////////////////////////////////////////////////
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

	SEARCH_STATE uiSearchState = m_eState;
	
	do
	{
		uiSearchState = FindNext();
	} while(uiSearchState == ACTIVE); // Fortsätt så länge uiSearchState är Aktiv

	if(uiSearchState == FOUND_DESTINATION)
	{
		// Set search node to start node
		m_pkSearchNode = m_pkStartNode;
		
		Point kSqr(m_pkStartNode->m_kSqrPos.x, m_pkStartNode->m_kSqrPos.y);
		m_kqPath.push(kSqr);
	
		// Hämta alla path noder, en efter en
		// och pusha dem till en kö.
		NodePtr pkNode = m_pkStartNode;
		while(1)
		{
			pkNode = GetNextNode();
			if(!pkNode)
				break;

			kSqr.x = pkNode->m_kSqrPos.x;
			kSqr.y = pkNode->m_kSqrPos.y;
			m_kqPath.push(kSqr);
		};

		DeleteNodes();
	}
	else
	{
		// Hittade ingen lösning på problemet.
		return false;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////
// Name:		GetTerrainCost
// Description:	Gets a cost value from a square in the map terrain array.
//
int PathFind::GetTerrainCost(int x, int y)
{
	if( x < 0 || x >= m_iMapWidth ||
		y < 0 || y >= m_iMapWidth)
	{
		return BLOCKED_VALUE;	 
	}

	return m_piMapTerrain[(y*m_iMapWidth)+x];
}

///////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////
// Name:		DeleteUnusedNodes
// Description:	Delete nodes in the open and closed list 
//				that aren't needed for the solution.
//
void PathFind::DeleteUnusedNodes()
{
	NODLISTITER itOpen = m_kvOpenList.begin();
	while(itOpen != m_kvOpenList.end())
	{
		NodePtr pkNode = (*itOpen);

		if(!pkNode->m_pkChild)
		{
			delete pkNode;
			pkNode = NULL;
		}

		itOpen ++;
	}

	m_kvOpenList.clear();

	NODLISTITER itClosed;
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

///////////////////////////////////////////////////////////////////////
// Name:		DeleteAllNodes
// Description:	Frigör minne för alla noder som finns i bägge listorna. 
//
void PathFind::DeleteAllNodes()
{
	NODLISTITER itOpen = m_kvOpenList.begin();
	while( itOpen != m_kvOpenList.end() )
	{
		NodePtr pkNode = (*itOpen);
		delete pkNode;
		itOpen++;
	}

	m_kvOpenList.clear();

	NODLISTITER itClosed;
	for( itClosed = m_kvClosedList.begin(); 
	     itClosed != m_kvClosedList.end(); 
		 itClosed ++ )
	{
		NodePtr pkNode = (*itClosed);
		delete pkNode;
	}

	m_kvClosedList.clear();
}

///////////////////////////////////////////////////////////////////////
// Name:		DeleteNodes
// Description:	Frigör minne för alla söknoder.  
//				Skall anropas efter varje fullständig sökning.
//
void PathFind::DeleteNodes()
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
		// Startnod och slutnod är lika och därför
		// de enda noderna som finns.
		delete m_pkStartNode;
		delete m_pkDestNode;
	}
}

///////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////
// Name:		GetNextNode
// Description: Get next node from the search tree.
//
PathFind::NodePtr PathFind::GetNextNode()
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

///////////////////////////////////////////////////////////////////////
// Name:		AddNeighbour
// Description: Skapa en ny granne och initiera den med (x,y) data.
//
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

///////////////////////////////////////////////////////////////////////
// Name:		FindNext
// Description:  
//
PathFind::SEARCH_STATE PathFind::FindNext()
{
	// Avbryt om sökningen är avklarad.
	if( m_eState == FOUND_DESTINATION )
		return m_eState; 

	// Avbryt om listan är tom
	// dvs. vi har inte hittat någon
	// slut position.
	if( m_kvOpenList.empty() )
	{
		DeleteAllNodes();
		m_eState = CANT_REACH_DESTINATION;
		return m_eState;
	}
	
	NodePtr pkNode;
	
	// Välj ut och hämta ut den bästa noden (som har lägst f) 
	// ur den öppna listan och minska listans storlek.
	pkNode = m_kvOpenList.front();
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

			// Börja bakifrån (från destinations rutan) och tilldela
			// en child pekare till varje nod som har ett barn.
			// Avbryt när vi har nått start rutan.
			NodePtr pkChildNode = m_pkDestNode;
			NodePtr pkParentNode = m_pkDestNode->m_pkParent;

			do 
			{
				pkParentNode->m_pkChild = pkChildNode;
				pkChildNode = pkParentNode;
				pkParentNode = pkParentNode->m_pkParent;
			} 
			while( pkChildNode != m_pkStartNode );
		}

		// Ta bort alla oanvända noder.
		DeleteUnusedNodes();

		// Returnera att vi har lyckats finna
		// en lösning på problemet.
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

		// Vänster granne,
		if((GetTerrainCost( x-1, y ) < BLOCKED_VALUE) && 
			!( (iParentX == x-1) && (iParentY == y))) 
		{
			kNewSquare = Point(x-1, y);
			AddNeighbour( kNewSquare );
		}
		// Övre grannen.
		if((GetTerrainCost( x, y-1 ) < BLOCKED_VALUE) && 
			!( (iParentX == x) && (iParentY == y-1))) 
		{
			kNewSquare = Point(x, y-1);
			AddNeighbour( kNewSquare );
		}	
		// Högra grannen.
		if((GetTerrainCost( x+1, y ) < BLOCKED_VALUE) && 
			!( (iParentX == x+1) && (iParentY == y))) 
		{
			kNewSquare = Point(x+1, y);
			AddNeighbour( kNewSquare );
		}	
		// Undre grannen.
		if((GetTerrainCost( x, y+1 ) < BLOCKED_VALUE) && 
			!( (iParentX == x) && (iParentY == y+1)))
		{
			kNewSquare = Point(x, y+1);
			AddNeighbour( kNewSquare );
		}	

		// Nu undersöker vi varje granne...
		NODLISTITER itNeighbour; // iterator till en vektor med nod-pekare
		for( itNeighbour = m_kvNeighbours.begin(); 
		     itNeighbour != m_kvNeighbours.end(); 
			 itNeighbour ++ )
		{
			// Skapa ett g-värde för noden, som är kostnaden för hur långt
			// tid det skulle ta att vandra från denna nod från startnoden.
			float fNewGValue = pkNode->m_fGValue + (float)
				GetTerrainCost(pkNode->m_kSqrPos.x, pkNode->m_kSqrPos.y);

			//
			// Kolla om vi redan har undersökt denna granne, dvs. om den redan
			// finns i någon av de 2 listorna. Gör den det och har lägre g värde
			// så så tar vi bort grannen och hoppar till nästa loop varv.
			//
			NODLISTITER itOpenListResultat; 
			for( itOpenListResultat = m_kvOpenList.begin(); 
			     itOpenListResultat != m_kvOpenList.end(); 
				 itOpenListResultat ++ )
			{
				if( (*itOpenListResultat)->m_kSqrPos.x == (*itNeighbour)->m_kSqrPos.x &&
					(*itOpenListResultat)->m_kSqrPos.y == (*itNeighbour)->m_kSqrPos.y)
					break;				
			}
			if( itOpenListResultat != m_kvOpenList.end() )
			{
				if( (*itOpenListResultat)->m_fGValue <= fNewGValue ) 
				{
					// Elementet fanns i den stängda listan
					// men eftersom det var lägre så behåller vi 
					// detta, tar bort grannen och fortsätter leta.
					delete (*itNeighbour);
					continue;
				}
			}
			NODLISTITER itClosedListResult; // iterator till en vektor med nod-pekare
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
				if( (*itClosedListResult)->m_fGValue <= fNewGValue ) 
				{ 
					// Elementet fanns i den stängda listan
					// men eftersom det var lägre så behåller vi 
					// detta, tar bort grannen och fortsätter leta.
					delete (*itNeighbour);
					continue;
				}
			}

			// Vi har hittat den bästa noden hittils och initierar därför 
			// dess värden och behåller denna noden.
			(*itNeighbour)->m_pkParent = pkNode;
			(*itNeighbour)->m_fGValue = fNewGValue;
			(*itNeighbour)->m_fHValue = 
				(*itNeighbour)->GetHeuristicCost( m_pkDestNode->m_kSqrPos );
			(*itNeighbour)->m_fFValue = 
				(*itNeighbour)->m_fGValue + (*itNeighbour)->m_fHValue;

			// Elementet fanns inte i den stängda listan
			// så ta bort det.
			if( itClosedListResult != m_kvClosedList.end() )
			{
				delete (*itClosedListResult);
				m_kvClosedList.erase( itClosedListResult );
			}
			// Elementet fanns inte i den öppna listan
			// så ta bort det och bygg om heapen.
			if( itOpenListResultat != m_kvOpenList.end() )
			{	   
				delete (*itOpenListResultat);
			   	m_kvOpenList.erase( itOpenListResultat );
				make_heap( m_kvOpenList.begin(), m_kvOpenList.end(), HeapComp() );
			}

			// Vi har nu inte hittat någon bättre ersättare därför
			// lägger vi till grannen till den öppna listan
			// och sorterar den i heap-ordning.
			m_kvOpenList.push_back( (*itNeighbour) );
			push_heap( m_kvOpenList.begin(), m_kvOpenList.end(), HeapComp() );

		} // Fortsätt undersöka nästa granne...

		// Lägg till noden till den stängda listan
		// eftersom vi nu är färdig med den.
		m_kvClosedList.push_back( pkNode );
	}

	// Har funktionen inte avbrutits här så kommer vi anropa funktionen pånytt...
 	return m_eState; 
}

///////////////////////////////////////////////////////////////////////
// Name:		GetNextSquare
// Description: Pop the next best step from the path queue.
//
bool PathFind::GetNextSquare(int &riSquareX, int &riSquareY)
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

///////////////////////////////////////////////////////////////////////
// Name:		Rebuild
// Description: Bygg upp en sökväg mellan två rutor på kartan.
//				Sökvägen sparas i en kö, med startpositionen först.
//
bool PathFind::Rebuild( int iStartPosX, int iStartPosY, int iDestPosX, int iDestPosY )
{
	m_pkStartNode = new Node(iStartPosX, iStartPosY);
	m_pkDestNode = new Node(iDestPosX, iDestPosY);

	m_eState = ACTIVE;
	
	// Registrera startnoden
	m_pkStartNode->m_fGValue = 0; 
	m_pkStartNode->m_fHValue = m_pkStartNode->GetHeuristicCost(m_pkDestNode->m_kSqrPos);
	m_pkStartNode->m_fFValue = m_pkStartNode->m_fGValue + m_pkStartNode->m_fHValue;
	m_pkStartNode->m_pkParent = 0;

	// Lägg till startnoden till den öppna listan och 
	// sortera in elementet i heap ordning.
	m_kvOpenList.push_back( m_pkStartNode );
	push_heap( m_kvOpenList.begin(), m_kvOpenList.end(), HeapComp() );

	// Fill a queue with all (x,y) positions 
	// from start to end
	return FillQueue();
}