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

//int ZGuiMenu::s_iMenuIDCounter = 0;
const int MENU_ITEM_HEIGHT = 20;

ZGuiMenu::ZGuiMenu(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID, bool bPopup) :
	ZGuiWnd(kArea, pkParent, bVisible, iID)
{
	m_bIsOpen = false;
	m_bPopup = bPopup;
	m_bEnabled = true;
	m_bNeedToResize = true;
	RemoveWindowFlag(WF_CANHAVEFOCUS); // knappar har inte focus by default
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow

	CreateInternalControls();

	s_iMenuIDCounter = 0;

	m_pkFont = m_pkResMan->Font("defguifont"); 
	if(m_pkFont)
	{
		ResizeMenu();
		m_bNeedToResize = false;
	}

	TextureManager* pkTexMan = static_cast<TextureManager*>(
		g_ZFObjSys.GetObjectPtr("TextureManager"));

	int tex = pkTexMan->Load("data/textures/gui/mark.tga",0);
	m_vkMenuIcons.push_back(new ZGuiSkin(tex, false));

	tex = pkTexMan->Load("data/textures/gui/open_submenu.tga",0);
	m_vkMenuIcons.push_back(new ZGuiSkin(tex, false));
	

}

ZGuiMenu::~ZGuiMenu()
{	
	for(int i=0; i<m_vkItems.size(); i++)
	{
		delete m_vkItems[i];
	}
}

bool ZGuiMenu::Notify(ZGuiWnd* pkWindow, int iCode)
{
	if(iCode == NCODE_CLICK_UP)
	{
		int iMenuID = pkWindow->GetID();

		for(int i=0; i<m_vkItems.size(); i++)
		{
			if((int)m_vkItems[i]->pkButton->GetID() == iMenuID)
			{
				if(m_vkItems[i]->bOpenSubMenu == false)
				{
					Resize(GetScreenRect().Width(), 20);
					m_bIsOpen = false;
					HideAll();

					if(m_bPopup)
						Hide();

					// Skicka ett command message till mainprocen.
					int* pkParams = new int[2];
					pkParams[0] = iMenuID;
					pkParams[1] = 0;

					GetGUI()->GetActiveCallBackFunc()(this, ZGM_COMMAND, 2, pkParams);

					// Bocka av kryss alternativ
					if(m_vkItems[i]->iCheckMarkGroup == -1)
					{
						if(m_vkItems[i]->bUseCheckMark)
							m_vkItems[i]->bChecked = !m_vkItems[i]->bChecked;
					}
					else
					{
						m_vkItems[i]->bChecked = true;

						for(int j=0; j<m_vkItems.size(); j++)
						{
							if(i != j)
							{
								if(m_vkItems[j]->iCheckMarkGroup == m_vkItems[i]->iCheckMarkGroup)
									m_vkItems[j]->bChecked = false;
							}
						}
					}


					delete[] pkParams;
				}

				break;
			}
		}
	}
	else
	if(iCode == NCODE_CLICK_DOWN || iCode == NCODE_OVER_CTRL)
	{
		int iMenuID = pkWindow->GetID();

		for(int i=0; i<m_vkItems.size(); i++)
		{
			if((int)m_vkItems[i]->pkButton->GetID() == iMenuID)
			{
				HideAll();

				m_bIsOpen = true;

				ZGuiMenuItem* pkSubMenu = m_vkItems[i];

				map<ZGuiMenuItem*, bool>::iterator res = m_mkSubMenuStateMap.find( pkSubMenu );
				if ( res != m_mkSubMenuStateMap.end() )
						res->second = !res->second;

				if(pkSubMenu->pkParent == NULL && res->second == false && iCode == NCODE_CLICK_DOWN)
				{
					HideAll();
				}
				else
				{
					OpenSubMenu(pkSubMenu, res->second);
					Resize(GetScreenRect().Width(), 500);
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
	//if(!IsVisible())
	//	return true;

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

	if(m_pkLabel && m_bPopup == false)
		m_pkLabel->Render(pkRenderer); 

	if(m_bPopup)
	{
 		m_vkItems[0]->pkButton->Hide();
	}

	static ZGuiLabel* pkArrow = pkArrow;
	if(pkArrow == NULL)
	{
		pkArrow = new ZGuiLabel(Rect(0,0,16,16), this, false);
		ZGuiSkin* pkSkin = new ZGuiSkin();
		pkSkin->m_afBkColor[0] = 0;
		pkArrow->SetSkin(pkSkin);
	}

	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(m_vkItems[i]->pkButton->IsVisible())
		{
			m_vkItems[i]->pkButton->Render(pkRenderer); 

			if(m_vkItems[i]->pkParent != NULL)
			{
				Rect rc = m_vkItems[i]->pkButton->GetScreenRect();
				
				if(m_vkItems[i]->bOpenSubMenu)
				{
					pkArrow->SetPos(rc.Right - 17, rc.Top+2, true, true);
					pkArrow->SetSkin(m_vkMenuIcons[1]);
					pkArrow->Render(pkRenderer);
				}
				else
				if(m_vkItems[i]->bUseCheckMark)
				{
					if(m_vkItems[i]->bChecked)
					{
						pkArrow->SetPos(rc.Right - 17, rc.Top+2, true, true);
						pkArrow->SetSkin(m_vkMenuIcons[0]);
						pkArrow->Render(pkRenderer);
					}
				}
			}
		}
	}

	return true;
}

void ZGuiMenu::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
//	pkSkinDesc.push_back( SKIN_DESC(&(ZGuiSkin*) m_pkSkinBnUp, string("Button up")) );
}


void ZGuiMenu::CreateInternalControls()
{
	const int Ru = 192;
	const int Gu = 192;
	const int Bu = 192;

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

	if(m_vkItems.empty() && m_bPopup)
	{
		ZGuiMenuItem* pkRootItem = new ZGuiMenuItem;
		pkRootItem->bOpenSubMenu = false;
		pkRootItem->pkParent = NULL;
		pkRootItem->szNameID = new char[strlen("PopupRootMain")+1];
		strcpy(pkRootItem->szNameID, "PopupRootMain");
		pkRootItem->pkButton = new ZGuiButton(Rect(0,0,10,10),this,1,0);
		m_vkItems.push_back(pkRootItem); 

		s_iMenuIDCounter++;
	}

	char* szRealParent = NULL;

	if(szParentID == NULL && !m_vkItems.empty() && m_bPopup)
		szRealParent = "PopupRootMain";
	else
		szRealParent = (char*) szParentID;

	ZGuiMenuItem* new_item = new ZGuiMenuItem();

	new_item->szNameID = new char[strlen(szNameID)+1];
	strcpy(new_item->szNameID, szNameID);

	new_item->pkParent = GetItem(szRealParent);	

	if(szNameID == NULL || (szRealParent != NULL && new_item->pkParent == NULL) )
	{
		delete new_item;
		printf("Failed to add menu item. Bad arguments\n");
		return false;
	}

	int x = 0;
	int y = 0;
	int w = m_pkFont->GetLength(szText);
	int h = MENU_ITEM_HEIGHT;

	ZGuiMenuItem* pkParent = GetItem(szRealParent);

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

		y += GetNumChilds((char*)szRealParent) * MENU_ITEM_HEIGHT;
	}
	else
	{
		ZGuiMenuItem* pkLeftParent = GetRightParent();
		if(pkLeftParent)
			x = pkLeftParent->pkButton->GetScreenRect().Right; 

		bShow = true;
	}

	new_item->pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),this,bShow,s_iMenuIDCounter++);
	new_item->pkButton->SetButtonUpSkin(m_pkItemSkinUp); 
	new_item->pkButton->SetButtonHighLightSkin(m_pkItemSkinFocus); 
	new_item->pkButton->SetButtonDownSkin(m_pkSkinDown); 
	new_item->pkButton->SetText((char*) szText, false); 
	new_item->pkButton->m_bCenterTextHorz = false;



	//new_item->pkButton->SetID(s_iMenuIDCounter++);

	new_item->bOpenSubMenu = bOpenSubMenu;

	m_vkItems.push_back(new_item); 

//	GetGUI()->RegisterWindow( new_item->pkButton, (char*) szNameID);

	if(new_item->bOpenSubMenu)
	{
		m_mkSubMenuStateMap.insert( 
			map<ZGuiMenuItem*,bool>::value_type( new_item, false ) );
	}

	if(m_bPopup && !m_vkItems.empty())
	{
		OpenSubMenu(m_vkItems[0], true);
	}

	string blank_space;
	const int laps = 1+17/new_item->pkButton->GetFont()->m_iSpaceWidth;
	for(int i=0; i<laps; i++)
		blank_space += string(" ");

	string t = string(" ") + string(new_item->pkButton->GetText());

	if(new_item->pkParent != NULL)
	{
		t += blank_space;
	}

	new_item->pkButton->SetText((char*)t.c_str(), false); 
	new_item->bUseCheckMark = false;
	new_item->bChecked = false;
	new_item->iCheckMarkGroup = -1;
	
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
	ZGuiMenuItem* pkCurr = pkSubMenu;

	while(pkCurr != NULL)
	{
		for(int i=0; i<m_vkItems.size(); i++)
		{
			if(m_vkItems[i]->pkParent == pkCurr)
				m_vkItems[i]->pkButton->Show();
		}

		pkCurr = pkCurr->pkParent;
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

void ZGuiMenu::ClearAll()
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		delete m_vkItems[i]->pkButton;
		delete m_vkItems[i];
	}

	s_iMenuIDCounter = 0;

	m_vkItems.clear();
	m_mkSubMenuStateMap.clear();
}

void ZGuiMenu::HideAll()
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(m_vkItems[i]->pkParent != NULL)
			m_vkItems[i]->pkButton->Hide();
	}

	m_bIsOpen = false;
	
	map<ZGuiMenuItem*, bool>::iterator it = m_mkSubMenuStateMap.begin();
	for( ; it != m_mkSubMenuStateMap.end(); it++)
	{
		if(!it->first->pkButton->IsVisible())
			it->second = false;
	}	

	if(m_bPopup && !m_vkItems.empty())
	{
		OpenSubMenu(m_vkItems[0], true);
	}
}

void ZGuiMenu::ResizeMenu()
{
	if(m_pkFont == NULL)
		return;

	if(m_bPopup)
	{
		map<string,int> sizes;

		for(int j=0; j<m_vkItems.size(); j++)		
		{
			if(m_vkItems[j]->pkParent == NULL)
				continue;

			map<string,int>::iterator it = sizes.find( string(m_vkItems[j]->pkParent->szNameID) );

			if(it != sizes.end())
			{
				int sz = m_pkFont->GetLength(m_vkItems[j]->pkButton->GetText());

				int max_size = it->second;

				if(max_size < sz)
					max_size = sz;

				sizes[ string(m_vkItems[j]->pkParent->szNameID) ] = max_size;
			}
			else
			{
				int sz = m_pkFont->GetLength(m_vkItems[j]->pkButton->GetText());
				sizes[ string(m_vkItems[j]->pkParent->szNameID) ] = sz;
			}
		}

		for(int j=0; j<m_vkItems.size(); j++)
		{
			if(m_vkItems[j]->pkParent == NULL)
				continue;

			int w = sizes[ string(m_vkItems[j]->pkParent->szNameID) ] + 8;
			m_vkItems[j]->pkButton->Resize(w,-1,true); 
			m_vkItems[j]->pkButton->Move(0,-MENU_ITEM_HEIGHT,false,false); 

			Rect rc = m_vkItems[j]->pkButton->GetWndRect(); 

			if(m_vkItems[j]->pkParent && m_vkItems[j]->pkParent != GetItem("PopupRootMain"))
				rc.Left = m_vkItems[j]->pkParent->pkButton->GetWndRect().Right + 1; 

			m_vkItems[j]->pkButton->SetPos(rc.Left,rc.Top,false,true);
		}

		return;
	}

	const int SPACE_BETWEEN_SUB_MENUS = 8;

	// Börja med att skala om alla utan parent till sin rätta storlek.
	int iPrevIndex=0; // 040701 = int iPrevIndex;

	for(int j=0; j<m_vkItems.size(); j++)
	{
		if(m_vkItems[j]->pkParent == NULL || m_bPopup)
		{
			ZGuiButton* pkButton = m_vkItems[j]->pkButton;
			pkButton->SetFont(m_pkFont); 

			int width = m_pkFont->GetLength( pkButton->GetText() ); 
			pkButton->Resize(width, MENU_ITEM_HEIGHT);

			if( j > 0 || m_bPopup)
			{
				int x = m_vkItems[iPrevIndex]->pkButton->GetWndRect().Right + SPACE_BETWEEN_SUB_MENUS; 
				int y = pkButton->GetWndRect().Top; 

				if(m_bPopup)
				{
					x = 0;
					y-=pkButton->GetWndRect().Height();
				}
				
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
			if(m_vkItems[j]->pkParent == it->first || m_bPopup)
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
			if(m_vkItems[j]->pkParent == it->first || m_bPopup)
			{
				ZGuiButton* pkButton = m_vkItems[j]->pkButton;
				pkButton->Resize(kMaxWidths[iIndex] /*+ 25*/, MENU_ITEM_HEIGHT);
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

void ZGuiMenu::SetCheckMark(char* szItemNameID, bool bSet)
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(!strcmp(m_vkItems[i]->szNameID, szItemNameID))
		{
			m_vkItems[i]->bChecked = bSet;
			break;
		}
	}
}

void ZGuiMenu::SetCheckMarkGroup(char* szItemNameID, int iGroup)
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(!strcmp(m_vkItems[i]->szNameID, szItemNameID))
		{
			m_vkItems[i]->iCheckMarkGroup = iGroup;
			break;
		}
	}
}

void ZGuiMenu::UseCheckMark(char* szItemNameID, bool bUse)
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(!strcmp(m_vkItems[i]->szNameID, szItemNameID))
		{
			m_vkItems[i]->bUseCheckMark = bUse;
			break;
		}
	}
}

void ZGuiMenu::SetItemIcon(char* szItemNameID, char* szIconFile)
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(!strcmp(m_vkItems[i]->szNameID, szItemNameID))
		{
			//m_vkItems[i]->iIconIndex
			break;
		}
	}
}

void ZGuiMenu::SetItemText(char* szItemNameID, char* szText)
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(!strcmp(m_vkItems[i]->szNameID, szItemNameID))
		{
			m_vkItems[i]->pkButton->SetText(szText);
			break;
		}
	}
}

bool ZGuiMenu::IsOpen()
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(m_vkItems[i]->pkButton->IsVisible() &&
			m_vkItems[i]->pkParent != NULL)
			return true;
	}

	return false;
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

bool ZGuiMenu::HooverItem(int x, int y)
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(m_vkItems[i]->pkButton->IsVisible() &&
			m_vkItems[i]->pkButton->GetScreenRect().Inside(x,y))
			return true;
	}

	return false;
}

char* ZGuiMenu::GetItemName(int iID)
{
	for(int i=0; i<m_vkItems.size(); i++)
	{
		if(m_vkItems[i]->pkButton->GetID() == iID)
			return m_vkItems[i]->szNameID;
	}

	return NULL;
}