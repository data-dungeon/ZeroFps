// serialization.cpp: implementation of the Serialization class.
//
//////////////////////////////////////////////////////////////////////

#include "serialization.h"
#include "../zerofpsv2/basic/zfsystem.h"
#include "../zerofpsv2/basic/zfbasicfs.h"
#include "../zerofpsv2/basic/zfresourcedb.h"
#include "../zerofpsv2/script/zfscript.h"
#include "../zerofpsv2/render/texturemanager.h"
#include "scene.h"
#include <typeinfo>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Serialization::Serialization()
{
	m_pkBasicFS = static_cast<ZFBasicFS*>(g_ZFObjSys.GetObjectPtr("ZFBasicFS"));
	m_pkGuiResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));

	ZFResourceDB* pkResDB = static_cast<ZFResourceDB*>(g_ZFObjSys.GetObjectPtr("ZFResourceDB"));
	pkResDB->ToggleInstantExpire(true);
}

Serialization::~Serialization()
{

}

bool Serialization::SaveGUI(char* szFileName, Scene* pkScene)
{
	m_kSavedWnds.clear();
	
	string strFileName = ChangeExtension(szFileName); // byt ändelse till .lua

	FILE* pkFile = fopen(strFileName.c_str(), "w+t"); // öppna filen i skrivläge

	if(pkFile == NULL)
	{
		printf("Failed to save file! Another application may use the file\n");
	}

	//printf("Saving file %s\n", strFileName.c_str());

	fprintf(pkFile, "-- Mistlands GUI Resource file, START\n\n");

	map<string, ZGuiWnd*> kWindows;
	m_pkGuiResMan->GetWindows(kWindows); // hämta alla fönster som finns

	// kopiera alla fönster till en vektor och sortera den
	list<ZGuiWnd*> kWnd;
	
	map<string, ZGuiWnd*>::iterator it2;
	for( it2 = kWindows.begin(); it2 != kWindows.end(); it2++)
	{
		ZGuiWnd* pkWnd = (*it2).second;
		if( pkWnd != NULL && !pkScene->IsSceneWnd(pkWnd) )
			kWnd.push_back(pkWnd);
	}

	//
	// funktion för att skapa allt, Start
	//
	fprintf(pkFile, "function GUICreate()\n\n"); 

	//
	// printa alla skins
	//

	fprintf(pkFile, "\t-------------------------------------------------------------------------\n");
	fprintf(pkFile, "\t-- Skins.\n");
	fprintf(pkFile, "\t-------------------------------------------------------------------------\n");

	vector<ZGuiSkin*> vkSkins;

	for( list<ZGuiWnd*>::iterator it = kWnd.begin(); it!= kWnd.end(); it++)
	{
		// Hämta alla skinns som tillhör detta fönster.
		vector< pair<ZGuiSkin**, string> >kSkins;
		(*it)->GetWndSkinsDesc(kSkins);

		// Gå igenom alla skins och lägg till dem om de inte redan har lagts till.
		vector< pair<ZGuiSkin**, string> >::iterator itSkin = kSkins.begin(); 
		for( ; itSkin != kSkins.end(); itSkin++)
		{
			ZGuiSkin* pkSkin = (*itSkin->first);

			bool bAddSkin = true;

			for(unsigned int i=0; i<vkSkins.size(); i++)
				if(*pkSkin == *vkSkins[i])
				{
					bAddSkin = false;
					break;
				}

			if(bAddSkin)
			{
				vkSkins.push_back(pkSkin); 
			}
		}

		kSkins.clear(); 
	}

	vector< pair<ZGuiSkin, string> > kSkinTable;

	char name[1024], tex[8][1024], szSkin[1024];
	int r, g, b, r2, g2, b2, bds, tile, trans;

	for(unsigned int i=0; i<vkSkins.size(); i++)
	{
		ZGuiSkin* pkSkin = vkSkins[i];

		r=0, g=0, b=0, r2=0, g2=0, b2=0, bds=0, tile=0, trans=0;

		if(pkSkin->m_iBkTexID != -1)					strcpy(tex[0], m_pkTexMan->GetFileName(pkSkin->m_iBkTexID)); else strcpy(tex[0], "0");
		if(pkSkin->m_iHorzBorderTexID != -1)			strcpy(tex[1], m_pkTexMan->GetFileName(pkSkin->m_iHorzBorderTexID)); else strcpy(tex[1], "0");
		if(pkSkin->m_iVertBorderTexID != -1)			strcpy(tex[2], m_pkTexMan->GetFileName(pkSkin->m_iVertBorderTexID)); else strcpy(tex[2], "0");
		if(pkSkin->m_iBorderCornerTexID != -1)			strcpy(tex[3], m_pkTexMan->GetFileName(pkSkin->m_iBorderCornerTexID)); else strcpy(tex[3], "0");
		if(pkSkin->m_iBkTexAlphaID != -1)				strcpy(tex[4], m_pkTexMan->GetFileName(pkSkin->m_iBkTexAlphaID)); else strcpy(tex[4], "0");
		if(pkSkin->m_iHorzBorderTexAlphaID != -1)		strcpy(tex[5], m_pkTexMan->GetFileName(pkSkin->m_iHorzBorderTexAlphaID)); else strcpy(tex[5], "0");
		if(pkSkin->m_iVertBorderTexAlphaID != -1)		strcpy(tex[6], m_pkTexMan->GetFileName(pkSkin->m_iVertBorderTexAlphaID)); else strcpy(tex[6], "0");
		if(pkSkin->m_iBorderCornerTexAlphaID != -1)		strcpy(tex[7], m_pkTexMan->GetFileName(pkSkin->m_iBorderCornerTexAlphaID)); else strcpy(tex[7], "0");

		// Ta bort sökvägen "/textures/gui/" eftersom alla texturer måste ligga här
		// eller i submappar härifrån.
		// ZGuiApp::GetTexID lägger nämligen på "/data/textures/gui/" när texturerna
		// sedan skall laddas in från skriptfilen.
		for(int j=0; j<8; j++)
		{
			string temp(tex[j]);
			int pos = temp.find("/textures/gui/");
			if(pos != string::npos)
			{
				temp.erase(0,pos+strlen("/textures/gui/")); 
				strcpy(tex[j],temp.c_str());
			}
		}	
		
		r = (int) (pkSkin->m_afBkColor[0] * 255.0f);
		g = (int) (pkSkin->m_afBkColor[1] * 255.0f);
		b = (int) (pkSkin->m_afBkColor[2] * 255.0f);
		r2 = (int) (pkSkin->m_afBorderColor[0] * 255.0f);
		g2 = (int) (pkSkin->m_afBorderColor[1] * 255.0f);
		b2 = (int) (pkSkin->m_afBorderColor[2] * 255.0f);
		bds = (int) pkSkin->m_unBorderSize;
		tile = (int) pkSkin->m_bTileBkSkin;
		trans = (int) pkSkin->m_bTransparent;

		sprintf(name, "Skin%i", 1+i);

		sprintf(szSkin, "\t%s = { tex1=\"%s\", tex2=\"%s\", tex3=\"%s\", tex4=\"%s\", tex1a=\"%s\", tex2a=\"%s\", tex3a=\"%s\", tex4a=\"%s\", bkR=%i, bkG=%i, bkB=%i, borderR=%i, borderG=%i, borderB=%i, bd_size=%i, tile=%i, trans=%i }\n", 
			name, tex[0],tex[1],tex[2],tex[3],tex[4],tex[5],tex[6],tex[7], r, g, b, r2, g2, b2, bds, tile, trans);

		// Separera raderna så att de inte blir så långa.
		string temp(szSkin);
		unsigned int row_break = 120;

		while(1)
		{
			if(temp.length() > row_break)
			{
				int next_space = temp.find_last_of( " ", row_break);
				temp.insert(next_space, "\n\t");
				row_break += 120;
			}
			else
				break;
		}

		fprintf(pkFile, temp.c_str());

		// lägg till skin och namn i en söklista, så att vi senare kan söka reda på rätt skin
		// att jämnföra med
		kSkinTable.push_back( pair<ZGuiSkin, string>( *pkSkin, string(name) ));
	}

	//
	// Printa alla windows
	//

	fprintf(pkFile, "\t-------------------------------------------------------------------------\n");
	fprintf(pkFile, "\t-- Create all windows in correct order (parents before childrens etc.)\n");
	fprintf(pkFile, "\t-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,\n"); 
	fprintf(pkFile, "\t-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox\n");
	fprintf(pkFile, "\t-------------------------------------------------------------------------");

	if(kWnd.empty() == false)
	{
		while(1)
		{		
			ZGuiWnd* pkWnd = kWnd.back();
			ZGuiWnd* pkParent = pkWnd->GetParent();

			if(pkParent == NULL || ParentHaveBeenSaved(pkParent))
			{
				Rect rc;

				int iType = (int) pkScene->GetApp()->GetType(pkWnd); 

				const char* szName = pkScene->GetAlias(pkWnd);
				if(szName == NULL)
					szName = pkWnd->GetName();

				char szParent[75], szLabel[75];

				if(pkWnd->GetParent())
				{
					const char* szAliasParent = pkScene->GetAlias(pkWnd->GetParent());

					if(szAliasParent)
						strcpy(szParent, szAliasParent);
					else
						strcpy(szParent, pkWnd->GetParent()->GetName());
				}
				else
					strcpy(szParent, "");

				if(pkWnd->GetText())
					strcpy(szLabel, pkWnd->GetText());
				else
					strcpy(szLabel, "");

				if(szParent == NULL)
					rc = pkWnd->GetScreenRect();
				else
					rc = pkWnd->GetWndRect();

				fprintf(pkFile, "\n\tCreateWnd(%i,\"%s\",\"%s\",\"%s\",%i,%i,%i,%i,0)\n", iType, 
					szName, szParent, szLabel, rc.Left, rc.Top, rc.Width(), rc.Height());

				m_kSavedWnds.push_back(pkWnd);

				//
				// Printa anrop för att byta texturer
				//

				vector< pair<ZGuiSkin**, string> >kSkins;
				pkWnd->GetWndSkinsDesc(kSkins);

				vector< pair<ZGuiSkin**, string> >::iterator itSkin = kSkins.begin(); 
				for( ; itSkin != kSkins.end(); itSkin++)
				{
					if(*itSkin->first)
					{
						ZGuiSkin kSkin = *(*itSkin->first);

						// leta rätt på namnet på skin'en och printa
						for(unsigned int k=0; k<kSkinTable.size(); k++)
							if(kSkin == kSkinTable[k].first)
							{
								fprintf(pkFile, "\tChangeSkin(\"%s\",\"%s\",\"%s\")\n", 
									szName, kSkinTable[k].second.c_str(), itSkin->second.c_str());
								break;
							}
					}
				}

				kSkins.clear(); 

				kWnd.pop_back();
				if(kWnd.empty())
					break;
			}
			else
			{
				kWnd.sort(RandomSort); // blanda listan så att ett annat fönster hamnar först...
			}
		}
	}
	
	//
	// funktion för att skapa allt, Slut
	//
	fprintf(pkFile, "\nend\n");

	fprintf(pkFile, "\n-- Mistlands GUI Resource file, END\n");

	fclose(pkFile);

	return true;
}

bool Serialization::LoadGUI(const char* szFileName, Scene* pkScene)
{
	ZGuiApp* pkApp = pkScene->GetApp();

	// Börja med att ta bort alla fönster
	map<string, ZGuiWnd*> kWindows;
	m_pkGuiResMan->GetWindows(kWindows);
	map<string, ZGuiWnd*>::iterator it2;
	for( it2 = kWindows.begin(); it2 != kWindows.end(); it2++)
	{
		if(pkScene->IsSceneWnd(it2->second) == false)
		{
			if(it2->second)
				printf("%s\n", it2->second->GetName());

			pkScene->RemoveAlias(it2->second);
			m_pkGui->UnregisterWindow(it2->second);
		}
	}

	printf("Loading file %s\n", szFileName);

	ZFResourceHandle* pkScriptResHandle = new ZFResourceHandle;

	if(!pkScriptResHandle->SetRes(szFileName))
	{
		printf("Failed to load script %s\n", szFileName);
		return false;
	}

	ZFScriptSystem* pkScript = static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));
	
	pkApp->m_pkScriptResHandle = pkScriptResHandle; // OBS! Fulhack eftersom inget automatskript körs

	if(!pkScript->Call(pkScriptResHandle, "GUICreate", 0, 0))
	{
		printf("Failed to call script\n");
		return false;
	}

	pkApp->m_pkScriptResHandle->FreeRes(); 

	return true;
}

string Serialization::ChangeExtension(char *szFileName)
{
	string strFileName(szFileName);

	int extpoint_pos = strFileName.find_last_of('.'); 

	if(extpoint_pos == (int )strFileName.length()-4)
	{
		int size = strFileName.length();
		strFileName.erase(extpoint_pos,size);
		strFileName.append(".lua");
	}
	else
	{
		strFileName.append(".lua");
	}

	return strFileName;
}

bool Serialization::ParentHaveBeenSaved(ZGuiWnd *pkParent)
{
	for(unsigned int i=0; i<m_kSavedWnds.size(); i++)
		if(pkParent == m_kSavedWnds[i]) // om dess parent har blivit skapad
			return true;

	return false;
}

void Serialization::TempSave(Scene* pkScene)
{
	string strTempFileName = "../data/guires_temp.lua";
	SaveGUI((char*)strTempFileName.c_str(), pkScene);
}

ZGuiWnd* Serialization::TempLoad(Scene* pkScene)
{
	string strTempFileName = "../data/guires_temp.lua";

	ZGuiWnd* pkReturnWnd = NULL;

	if(m_pkBasicFS->FileExist(strTempFileName.c_str()))
	{
		

		strTempFileName.erase(0,2);

		// Ladda in på nytt
		if(!LoadGUI(strTempFileName.c_str(), pkScene))
		{
			printf("Failed to save file %s\n", strTempFileName.c_str());
			return NULL;
		}

		// Lägg till standardelement
		map<string, ZGuiWnd*> kWindows;
		m_pkGuiResMan->GetWindows(kWindows);
		for(map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
		{
			if(pkScene->IsSceneWnd(it->second) == false)
			{
				pkScene->AddStandardElements(it->second);
				it->second->Disable();
			}
		}
	}
	else
	{
		printf("Failed to open file %s\n", strTempFileName.c_str());
	}

	return pkReturnWnd;
}