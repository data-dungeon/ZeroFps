// ZGuiMenu.cpp: implementation of the ZGuiMenu class.
//
//////////////////////////////////////////////////////////////////////
 
#include "../basic/zguiskin.h"
#include "zguimenu.h"
#include "../render/zguirenderer.h"
#include "zguiresourcemanager.h"
#include "zgui.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////// //////////////////////////////////////////////////////////

int ZGuiMenu::s_iMenuIDCounter = 0;
const int MENU_ITEM_HEIGHT = 20;

ZGuiMenu::ZGuiMenu(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiWnd(kArea, pkParent, bVisible, iID)
{
	m_bIsOpen = false;
	m_bEnabled = true;
	m_bNeedToResize = true;
	RemoveWindowFlag(WF_CANHAVEFOCUS); // knappar har inte focus by default
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow

	CreateInternalControls();

	m_pkFont = m_pkResMan->Font("defguifont"); 
	if(m_pkFont)
	{
		ResizeMenu();
		m_bNeedToResize = false;
	}
}

ZGuiMenu::~ZGuiMenu()
{

}

bool ZGuiMenu::Notify(ZGuiWnd* pkWindow, int iCode)
{
	if(iCode == NCODE_CLICK_DOWN) // NCODE_OVER_CTRL
	{
		int iMenuID = pkWindow->GetID();

		for(int i=0; i<m_vkItems.size(); i++)
		{
			if((int)m_vkItems[i]->pkButton->GetID() == iMenuID)
			{
				if(m_vkItems[i]->bOpenSubMenu)
				{
					if(m_vkItems[i]->pkParent == NULL)
						HideAll();

					m_bIsOpen = true;

					ZGuiMenuItem* pkSubMenu = m_vkItems[i];

					map<ZGuiMenuItem*, bool>::iterator res = m_mkSubMenuStateMap.find( pkSubMenu );
					if ( res != m_mkSubMenuStateMap.end() )
							res->second = !res->second;

					OpenSubMenu(pkSubMenu, res->second);

					Resize(GetScreenRect().Width(), 500);
					/*GetGUI()->PlaceWndFrontBack(m_vkItems[i]->pkButton->GetParent(), true);
					GetGUI()->PlaceWndFrontBack(this, true);*/

				}
				else
				{

					Resize(GetScreenRect().Width(), 20);

					m_bIsOpen = false;


		// Skicka ett Command medelande till valt fönster.
		int* pkParams = new int[2];
		int id = iMenuID; // control id
		pkParams[0] = id;
		pkParams[1] = 0;

		ZGuiWnd* pkMain = this;

		if(pkMain == NULL)
			pkMain = GetGUI()->GetActiveMainWnd();

		GetGUI()->GetActiveCallBackFunc()(pkMain,ZGM_COMMAND, 2, pkParams);

		delete[] pkParams;




					HideAll();
				}

				break;
			}
		}
	}

	if(m_pkParent)
		m_pkParent->Notify(this, iCode);

	return false;
}

bool ZGuiMenu::Render( ZGuiRender* pkRenderer )
{
	if(!IsVisible())
		return true;

	if(m_bNeedToResize == true)
	{
		if(m_pkFont != NULL)
		{
			ResizeMenu();
			m_bNeedToResize = false;
		}
		else
		{
			m_pkFont = m_pkResMan->Font("defguifont"); 
		}
	}

	if(m_pkLabel)
		m_pkLabel->Render(pkRenderer); 

	for(int i=0; i<m_vkItems.size(); i++)
	{
		m_vkItems[i]->pkButton->Render(pkRenderer); 
	}

	return true;
}

void ZGuiMenu::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
//	pkSkinDesc.push_back( SKIN_DESC(&(ZGuiSkin*) m_pkSkinBnUp, string("Button up")) );
}


void ZGuiMenu::CreateInternalControls()
{
	const int Ru = 153;
	const int Gu = 134;
	const int Bu = 117;

	const int Rf = 199;
	const int Gf = 178;
	const int Bf = 153;

	const int Rdf = 19;
	const int Gdf = 17;
	const int Bdf = 15;

	Rect rc = GetWndRect();
	rc.Left = 0;
	rc.Top = 0;
	rc.Right = rc.Width();
	rc.Bottom = MENU_ITEM_HEIGHT;
	
	m_pkLabel = new ZGuiLabel(rc, this, true);

	ZGuiSkin* pkSkin = new ZGuiSkin();
	
	pkSkin->m_afBkColor[0] = (1.0f / 255) * Ru;
	pkSkin->m_afBkColor[1] = (1.0f / 255) * Gu ;
	pkSkin->m_afBkColor[2] = (1.0f / 255) * Bu ;

	pkSkin->m_afBorderColor[0] = (1.0f / 255) * Rdf ;
	pkSkin->m_afBorderColor[1] = (1.0f / 255) * Gdf ;
	pkSkin->m_afBorderColor[2] = (1.0f / 255) * Bdf ;

	pkSkin->m_unBorderSize = 1;

	m_pkLabel->SetSkin(pkSkin);

	m_pkItemSkinUp = new ZGuiSkin();
	m_pkItemSkinFocus = new ZGuiSkin();
	m_pkSkinDown = new ZGuiSkin();

	m_pkItemSkinUp->m_afBkColor[0] = (1.0f / 255) * Ru;
	m_pkItemSkinUp->m_afBkColor[1] = (1.0f / 255) * Gu ;
	m_pkItemSkinUp->m_afBkColor[2] = (1.0f / 255) * Bu ;

	m_pkItemSkinFocus->m_afBkColor[0] = (1.0f / 255) * Rf ;
	m_pkItemSkinFocus->m_afBkColor[1] = (1.0f / 255) * Gf ;
	m_pkItemSkinFocus->m_afBkColor[2] =(1.0f / 255) * Bf ;

	m_pkItemSkinFocus->m_afBorderColor[0] = (1.0f / 255) * Rdf ;
	m_pkItemSkinFocus->m_afBorderColor[1] = (1.0f / 255) * Gdf ;
	m_pkItemSkinFocus->m_afBorderColor[2] = (1.0f / 255) * Bdf ;

	m_pkItemSkinFocus->m_unBorderSize = 1;

	m_pkSkinDown->m_afBkColor[0] = (1.0f / 255) * Rf ;
	m_pkSkinDown->m_afBkColor[1] = (1.0f / 255) * Gf ;
	m_pkSkinDown->m_afBkColor[2] = (1.0f / 255) * Bf ;

	m_pkSkinDown->m_afBorderColor[0] = (1.0f / 255) * Rdf ;
	m_pkSkinDown->m_afBorderColor[1] = (1.0f / 255) * Gdf ;
	m_pkSkinDown->m_afBorderColor[2] = (1.0f / 255) * Bdf ;

	m_pkSkinDown->m_unBorderSize = 1;
}

bool ZGuiMenu::AddItem(const char* szText, const char* szNameID, 
							  const char* szParentID, bool bOpenSubMenu)
{
	ZGuiMenuItem* new_item = new ZGuiMenuItem();

	new_item->szNameID = new char[strlen(szNameID)+1];
	strcpy(new_item->szNameID, szNameID);

	new_item->pkParent = GetItem(szParentID);	

	if(szNameID == NULL || (szParentID != NULL && new_item->pkParent == NULL) )
	{
		delete new_item;
		printf("Failed to add menu item. Bad arguments\n");
		return false;
	}

	int x = 0;
	int y = 0;
	int w = 100;
	int h = MENU_ITEM_HEIGHT;

	ZGuiMenuItem* pkParent = GetItem(szParentID);

	bool bShow = false;

	if(pkParent)
	{
		if(pkParent->bOpenSubMenu && pkParent->pkParent != NULL)
		{
			x = pkParent->pkButton->GetWndRect().Right;
			y = pkParent->pkButton->GetWndRect().Top;
		}
		else
		{
			x = pkParent->pkButton->GetWndRect().Left;
			y = pkParent->pkButton->GetWndRect().Bottom;
		}

		y += GetNumChilds((char*)szParentID) * MENU_ITEM_HEIGHT;
	}
	else
	{
		ZGuiMenuItem* pkLeftParent = GetRightParent();
		if(pkLeftParent)
			x = pkLeftParent->pkButton->GetScreenRect().Right; 

		bShow = true;
	}

	new_item->pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),this,bShow,0);
	new_item->pkButton->SetButtonUpSkin(m_pkItemSkinUp); 
	new_item->pkButton->SetButtonHighLightSkin(m_pkItemSkinFocus); 
	new_item->pkButton->SetButtonDownSkin(m_pkSkinDown); 
	new_item->pkButton->SetText((char*) szText, false); 
	new_item->pkButton->m_bCenterTextHorz = false;


	new_item->pkButton->SetID(s_iMenuIDCounter++);

	new_item->bOpenSubMenu = bOpenSubMenu;

	m_vkItems.push_back(new_item); 

	GetGUI()->RegisterWindow( new_item->pkButton, (char*) szNameID);

	if(new_item->bOpenSubMenu)
	{
		m_mkSubMenuStateMap.insert( 
			map<ZGuiMenuItem*,bool>::value_type( new_item, false ) );
	}

	return true;
}

ZGuiMenuItem* ZGuiMenu::GetItem(const char* szNameID)
{
	if(szNameID == NULL)
		return NULL;

	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(strcmp(m_vkItems[i]->szNameID, szNameID) == 0)
			return m_vkItems[i];
	}

	return NULL;
}

int ZGuiMenu::GetNumChilds(char* szNameID)
{
	int antalChilds = 0;

	if(szNameID != NULL)
	{
		for(int i=0; i<m_vkItems.size(); i++)
		{
			if(m_vkItems[i]->pkParent)
			{
				if(strcmp(m_vkItems[i]->pkParent->szNameID, szNameID) == 0)
					antalChilds++;
			}
		}
	}

	return antalChilds;
}

ZGuiMenuItem* ZGuiMenu::GetRightParent()
{
	ZGuiMenuItem* right_parent = NULL;

	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(m_vkItems[i]->pkParent == NULL)
		{
			if(right_parent == NULL || 
				m_vkItems[i]->pkButton->GetScreenRect().Right > 
				right_parent->pkButton->GetScreenRect().Right)
			{
				right_parent = m_vkItems[i];
			}
		}
	}	

	return right_parent;
}


void ZGuiMenu::OpenSubMenu(ZGuiMenuItem* pkSubMenu, bool bOpen)
{
	vector<ZGuiMenuItem*> test;

	for(int i=0; i<m_vkItems.size(); i++)
	{
		ZGuiMenuItem* s = m_vkItems[i]->pkParent;
		bool bShow = true;

		while(1)
		{
			if(s == NULL)
				break;

			if(MenuIsOpen(s) == false) 
				bShow = false;

			s = s->pkParent;
		}

		if(bShow)
		{			
			
			m_vkItems[i]->pkButton->Show();
		}
		else
		{
			m_vkItems[i]->pkButton->Hide();

			if(m_vkItems[i]->bOpenSubMenu)
			{
				test.push_back( m_vkItems[i] );
			}
		}
	}

	vector<ZGuiMenuItem*> temp = m_vkItems;

	if(bOpen == false)
	{
		for(int i=0; i<test.size(); i++)
		{
			map<ZGuiMenuItem*, bool>::iterator it = m_mkSubMenuStateMap.begin();
			for( ; it != m_mkSubMenuStateMap.end(); it++)
			{
				if(it->first == test[i])
				{
					it->second = false;
				}
			}
		}
	}
}

bool ZGuiMenu::MenuIsOpen(ZGuiMenuItem* pkMenu)
{
	map<ZGuiMenuItem*, bool>::iterator res = m_mkSubMenuStateMap.find( pkMenu );
	if ( res != m_mkSubMenuStateMap.end() )
		return res->second;

	return false;
}

bool ZGuiMenu::IsChildOf(ZGuiMenuItem* pkSubMenu)
{
	return false;
}

void ZGuiMenu::HideAll()
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(m_vkItems[i]->pkParent != NULL)
			m_vkItems[i]->pkButton->Hide();
	}

	map<ZGuiMenuItem*, bool>::iterator it = m_mkSubMenuStateMap.begin();
	for( ; it != m_mkSubMenuStateMap.end(); it++)
	{
		it->second = false;
	}

	m_bIsOpen = false;
}

void ZGuiMenu::ResizeMenu()
{
	if(m_pkFont == NULL)
		return;

	const int SPACE_BETWEEN_SUB_MENUS = 8;

	// Börja med att skala om alla utan parent till sin rätta storlek.
	int iPrevIndex=0; // 040701 = int iPrevIndex;

	for(int j=0; j<m_vkItems.size(); j++)
	{
		if(m_vkItems[j]->pkParent == NULL)
		{
			ZGuiButton* pkButton = m_vkItems[j]->pkButton;
			pkButton->SetFont(m_pkFont); 

			int width = m_pkFont->GetLength( pkButton->GetText() ); 
			pkButton->Resize(width, MENU_ITEM_HEIGHT);

			if( j > 0 )
			{
				int x = m_vkItems[iPrevIndex]->pkButton->GetWndRect().Right + SPACE_BETWEEN_SUB_MENUS; 
				int y = pkButton->GetWndRect().Top; 
				
				pkButton->SetPos(x, y, false, true);
				pkButton->SetMoveArea(pkButton->GetScreenRect(), false); 				
			}

			iPrevIndex = j;
		}
	}

	// skala om en gång till eftersom x postionen har ändrats
	for(int j=0; j<m_vkItems.size(); j++)
	{
		if(m_vkItems[j]->pkParent == NULL)
		{
			ZGuiButton* pkButton = m_vkItems[j]->pkButton;
			int width = m_pkFont->GetLength( pkButton->GetText() ); 
			pkButton->Resize(width+SPACE_BETWEEN_SUB_MENUS, MENU_ITEM_HEIGHT);
		}
	}

	vector<int> kMaxWidths(m_mkSubMenuStateMap.size());

	for(int i=0; i<kMaxWidths.size(); i++)
		kMaxWidths[i] = -1;

	// Leta rätt på storleken för att items
	int iIndex=0;
	map<ZGuiMenuItem*, bool>::iterator it = m_mkSubMenuStateMap.begin();
	for( ; it != m_mkSubMenuStateMap.end(); it++)
	{
		int max = -1;
		for(int j=0; j<m_vkItems.size(); j++)
		{
			if(m_vkItems[j]->pkParent == it->first)
			{
				ZGuiButton* pkButton = m_vkItems[j]->pkButton;
				pkButton->SetFont(m_pkFont); 

				int width = 4 + m_pkFont->GetLength( pkButton->GetText() ); 

				if(max < width)
					max = width;
			}
		}

		kMaxWidths[iIndex] = max;
		iIndex++;
	}

	// Skala om bredden på alla items
	iIndex = 0;
	it = m_mkSubMenuStateMap.begin();
	for( ; it != m_mkSubMenuStateMap.end(); it++)
	{
		for(int j=0; j<m_vkItems.size(); j++)
		{
			if(m_vkItems[j]->pkParent == it->first)
			{
				ZGuiButton* pkButton = m_vkItems[j]->pkButton;
				pkButton->Resize(kMaxWidths[iIndex], MENU_ITEM_HEIGHT);
			}
		}

		iIndex++;
	}

	// Sätt korrekt x postion
	it = m_mkSubMenuStateMap.begin();
	for( ; it != m_mkSubMenuStateMap.end(); it++)
	{
		for(int j=0; j<m_vkItems.size(); j++)
		{
			if(m_vkItems[j]->pkParent == it->first)
			{
				ZGuiButton* pkButton = m_vkItems[j]->pkButton;

				int x; 

				if(m_vkItems[j]->pkParent->pkParent == NULL)
					x = m_vkItems[j]->pkParent->pkButton->GetWndRect().Left;
				else
					x = m_vkItems[j]->pkParent->pkButton->GetWndRect().Right;

				int y = pkButton->GetWndRect().Top; 

				pkButton->SetPos(2+x, 2+y, false, true);
			}
		}
	}
}




bool ZGuiMenu::IsOpen()
{
	return m_bIsOpen;
}	

bool ZGuiMenu::IsMenuItem(ZGuiWnd* pkButton)
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(m_vkItems[i]->pkButton == pkButton)
			return true;
	}

	return false;
}