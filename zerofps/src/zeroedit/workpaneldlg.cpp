// workpaneldlg.cpp: implementation of the WorkPanelDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "gui.h"
#include "workpaneldlg.h"
#include "resource_id.h"

extern ZeroEdit Editor;

static bool WORKPANELPROC( ZGuiWnd* pkWnd, unsigned int uiMessage, 
						  int iNumberOfParams, void *pkParams ) 
{
	return Editor.m_pkGui->GetDlg("WorkPanelDlg")->DlgProc(pkWnd, uiMessage, 
		iNumberOfParams, pkParams);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WorkPanelDlg::WorkPanelDlg(Gui* pkGui, Input* pkInput, ZGuiWndProc oMainWndProc) 
	: DlgBox(pkGui->GetGUI(), pkInput, oMainWndProc)
{
	m_pkDlgBox = NULL;
	m_pkGuiBuilder = pkGui;
	m_pkEdit = pkGui->m_pkEdit;
	m_iWidth = 224;
	m_iHeight = 224;
}

WorkPanelDlg::~WorkPanelDlg()
{

}

bool WorkPanelDlg::Create(int x, int y, char* szResourceFile, char* szDlgName)
{
	vector<string> akTabNames;
	akTabNames.push_back("Marker");
	akTabNames.push_back("Elevation");
	akTabNames.push_back("Texture");
	akTabNames.push_back("Object");

	m_pkDlgBox = m_pkGuiBuilder->CreateTabbedDialog("WorkPanel",12314124,2321323,
		x,y,m_iWidth,m_iHeight,akTabNames,WORKPANELPROC);

	int iHeight;
	char szText[50];

	ZGuiWnd* pkPage;
	vector<string> vkNames;

	// Create page 1: - Marker
	pkPage = WorkPanel()->GetPage(0);

	// Pensize
	m_pkGuiBuilder->CreateLabel(pkPage, 0, 5, 5, 50, 20, "Pen size");	
	ZGuiSlider* pkPenSizeSlider = m_pkGuiBuilder->CreateSlider(pkPage, 
		ID_PENSIZE_SL, 110,  5, 104, 16, SCF_HORZ|SCF_SLIDER_CENTER, 
		1, 50, m_pkEdit->m_iPencilSize, "PenSizeSlider", "white");
	sprintf(szText, "%i", m_pkEdit->m_iPencilSize);
	pkPenSizeSlider->AddBuddyWindow(m_pkGuiBuilder->CreateTextbox(pkPage, 
		ID_PENSIZE_EB, 70, 5, 30, 16, false, szText, "PenSizeEB"));

	// Drawrate
	int drawrate_pos = (int) 201 - (200.0f * m_pkEdit->m_fDrawRate);
	m_pkGuiBuilder->CreateLabel(pkPage, 0, 5, 25, 50, 20, "Draw rate");	
	ZGuiSlider* pkDrawRateSlider = m_pkGuiBuilder->CreateSlider(pkPage, 
		ID_DRAWRATE_SL, 110,  25, 104, 16, SCF_HORZ|SCF_SLIDER_CENTER, 
		1, 200, drawrate_pos, "DrawRateSlider", "white");
	sprintf(szText, "%i", drawrate_pos);
	pkDrawRateSlider->AddBuddyWindow(m_pkGuiBuilder->CreateTextbox(pkPage, 
		ID_DRAWRATE_EB, 70, 25, 30, 16, false, szText, "DrawRateEB"));

	vkNames.clear();
	vkNames.push_back("FreeFlight");
	vkNames.push_back("Precision");
	
	m_pkGuiBuilder->CreateLabel(pkPage, 0, 5, 45, 50, 20, "Camera mode");
	iHeight = m_pkGuiBuilder->CreateRadiobuttons(pkPage, vkNames, 
		"CameraModeRadioGroup", ID_CAMERAMODE_RADIOGROUP, 5, 65, 16);
	m_pkGuiBuilder->CheckRadioButton("CameraModeRadioGroup", 
		(char*) vkNames[m_pkEdit->m_eCameraMode].c_str());

	// Create page 2: - Elevation tool
	pkPage = WorkPanel()->GetPage(1);

	vkNames.clear();
	vkNames.push_back("Raise");
	vkNames.push_back("Lower");
	vkNames.push_back("Smooth");

	iHeight = m_pkGuiBuilder->CreateRadiobuttons(pkPage, vkNames, 
		"ElevationModeRadioGroup", ID_ELEVATIONMODE_RADIOGROUP, 5, 5, 16);
	m_pkGuiBuilder->CheckRadioButton("ElevationModeRadioGroup", "Raise");

	m_pkGuiBuilder->CreateCheckbox(pkPage, ID_AUTOSMOOTHGROUND_CHB, 5, 5+iHeight, 16, 16, true,
		"Auto-smooth", "AutoSmoothGroundChB");

	m_pkGuiBuilder->CreateLabel(pkPage, 0, 5, 30+iHeight, 150, 20, "Landscape polygon mode");
	vkNames.clear();
	vkNames.push_back("Solid");
	vkNames.push_back("Wireframe");	
	iHeight = m_pkGuiBuilder->CreateRadiobuttons(pkPage, vkNames, 
		"LandscapePolyModeGroup", ID_LANDSCAPEPOLYMODE_RADIOGROUP, 5, 50+iHeight, 16);
	m_pkGuiBuilder->CheckRadioButton("LandscapePolyModeGroup", 
		(char*) vkNames[0].c_str());

	// Create page 3: - Paint terrain
	pkPage = WorkPanel()->GetPage(2);

	m_pkGuiBuilder->CreateLabel(pkPage, 0, 5, 5, 209, 20, "Map");
	ZGuiCombobox* cb = m_pkGuiBuilder->CreateCombobox(pkPage, 
		ID_TERRAINTEXTURE_CB, 50, 5, 150, 20, false);
	m_pkGuiBuilder->Register(cb, "MaskTexCb");
	UpdateMapTexList();
	
	m_pkGuiBuilder->CreateLabel(pkPage, 0, 5,  50, 10, 16, "R");
	m_pkGuiBuilder->CreateLabel(pkPage, 0, 5,  70, 10, 16, "G");
	m_pkGuiBuilder->CreateLabel(pkPage, 0, 5,  90, 10, 16, "B");
	m_pkGuiBuilder->CreateLabel(pkPage, 0, 5, 110, 10, 16, "A");

	m_pkGuiBuilder->CreateSlider(pkPage, ID_SLIDER_R_EB, 75,  50, 104, 16, 
		SCF_HORZ|SCF_SLIDER_BOTTOM,  0, 255, -1, "SliderRBn", "slider_bk_r");
	m_pkGuiBuilder->CreateSlider(pkPage, ID_SLIDER_G_EB, 75,  70, 104, 16, 
		SCF_HORZ|SCF_SLIDER_BOTTOM,  0, 255, -1, "SliderGBn", "slider_bk_g");
	m_pkGuiBuilder->CreateSlider(pkPage, ID_SLIDER_B_EB, 75,  90, 104, 16, 
		SCF_HORZ|SCF_SLIDER_BOTTOM,  0, 255, -1, "SliderBBn", "slider_bk_b");
	m_pkGuiBuilder->CreateSlider(pkPage, ID_SLIDER_A_EB, 75, 110, 104, 16, 
		SCF_HORZ|SCF_SLIDER_BOTTOM, 16, 255, -1, "SliderABn", "slider_bk_a");

	char aszColor[4][50];
	sprintf(aszColor[0], "%i", m_pkEdit->m_iMaskColorR);
	sprintf(aszColor[1], "%i", m_pkEdit->m_iMaskColorG);
	sprintf(aszColor[2], "%i", m_pkEdit->m_iMaskColorB);
	sprintf(aszColor[3], "%i", m_pkEdit->m_iMaskColorA);
	m_pkGuiBuilder->CreateTextbox(pkPage, ID_MASKCOLOR_RED_EB,   25,  50, 30, 16, false, 
		aszColor[0], "MaskColorREB", true); 
	m_pkGuiBuilder->CreateTextbox(pkPage, ID_MASKCOLOR_GREEN_EB, 25,  70, 30, 16, false, 
		aszColor[1], "MaskColorGEB", true); 
	m_pkGuiBuilder->CreateTextbox(pkPage, ID_MASKCOLOR_BLUE_EB,  25,  90, 30, 16, false, 
		aszColor[2], "MaskColorBEB", true);
	m_pkGuiBuilder->CreateTextbox(pkPage, ID_MASKCOLOR_ALPHA_EB, 25, 110, 30, 16, false, 
		aszColor[3], "MaskColorAEB", true);

	((ZGuiSlider*)m_pkGuiMan->Wnd("SliderRBn"))->AddBuddyWindow( m_pkGuiMan->Wnd("MaskColorREB") );
	((ZGuiSlider*)m_pkGuiMan->Wnd("SliderGBn"))->AddBuddyWindow( m_pkGuiMan->Wnd("MaskColorGEB") );
	((ZGuiSlider*)m_pkGuiMan->Wnd("SliderBBn"))->AddBuddyWindow( m_pkGuiMan->Wnd("MaskColorBEB") );
	((ZGuiSlider*)m_pkGuiMan->Wnd("SliderABn"))->AddBuddyWindow( m_pkGuiMan->Wnd("MaskColorAEB") );

	((ZGuiSlider*)m_pkGuiMan->Wnd("SliderRBn"))->SetPos(m_pkEdit->m_iMaskColorR,true);
	((ZGuiSlider*)m_pkGuiMan->Wnd("SliderGBn"))->SetPos(m_pkEdit->m_iMaskColorG,true);
	((ZGuiSlider*)m_pkGuiMan->Wnd("SliderBBn"))->SetPos(m_pkEdit->m_iMaskColorB,true);
	((ZGuiSlider*)m_pkGuiMan->Wnd("SliderABn"))->SetPos(m_pkEdit->m_iMaskColorA,true);

	m_pkGuiBuilder->CreateCheckbox(pkPage, ID_LOCKMASKCOLOR_SLIDER_CH, 5, 135, 16, 16, 
		m_bLockMaskSliders, "Lock", "LockMaskcolorSliderCb");

////
	// Create page 2: - Add object
	pkPage = WorkPanel()->GetPage(3);

	m_pkGuiBuilder->CreateLabel(pkPage, 0, 5, 5, 50, 20, "MAD objects");
	ZGuiListbox* pkMadList = m_pkGuiBuilder->CreateListbox(pkPage, ID_MADOBJECTS_LB, 5, 25, 209, 140);
	m_pkGuiBuilder->Register(pkMadList, "MadFileList");
	
	vkNames.clear(); 
	m_pkEdit->pkZFVFileSystem->ListDir(&vkNames, "data/mad", false);
	for(unsigned int i=0; i<vkNames.size(); i++)
		m_pkGuiBuilder->AddItemToList(pkMadList, false, vkNames[i].c_str(), i, false);

	m_pkGuiBuilder->CreateButton(pkPage, ID_LOADMADFILE_BN, 5, 170, 50, 20, "Change");
	m_pkGuiBuilder->CreateButton(pkPage, ID_CREATEMADFILE_BN, 60, 170, 50, 20, "Create");
	
	return true;
}

bool WorkPanelDlg::DlgProc( ZGuiWnd* pkWnd, unsigned int uiMessage, 
						    int iNumberOfParams,void *pkParams )
{
	Rect rc;

	switch(uiMessage)
	{
	case ZGM_TCN_SELCHANGE:
		int iPage; iPage = ((int*)pkParams)[0];
		switch(iPage)
		{
		case 1: // Elevation tool
			if(m_pkGuiBuilder->IsButtonChecked("Raise", "ElevationModeRadioGroup"))
				m_pkEdit->m_iMode = RAISE;
			else
			if(m_pkGuiBuilder->IsButtonChecked("Lower", "ElevationModeRadioGroup"))
				m_pkEdit->m_iMode = LOWER;
			else
			if(m_pkGuiBuilder->IsButtonChecked("Smooth", "ElevationModeRadioGroup"))
				m_pkEdit->m_iMode = SMOOTH;
			else
				m_pkEdit->m_iMode = RAISE;
			break;
		case 2: // Paint texture
			m_pkEdit->m_iMode = TEXTURE;
			break;
		case 3: // Add object
			m_pkEdit->m_iMode = ADDOBJECT;
			break;
		}
		break;

	case ZGM_SCN_SETPOS:
		switch( ((int*)pkParams)[0] ) // id
		{
		case ID_SLIDER_R_EB:
			if(m_bLockMaskSliders == true)
				MoveColorSliders("SliderRBn");
			break;
		case ID_SLIDER_G_EB:
			if(m_bLockMaskSliders == true)
				MoveColorSliders("SliderGBn");
			break;
		case ID_SLIDER_B_EB:
			if(m_bLockMaskSliders == true)
				MoveColorSliders("SliderBBn");
			break;
		case ID_PENSIZE_SL:
			MoveColorSliders("PenSizeSlider");
			break;
		}
		break;

	case ZGM_CBN_SELENDOK:
		switch(((int*)pkParams)[0])
		{
		case ID_TERRAINTEXTURE_CB:
			m_pkEdit->m_iEditMask = ((int*)pkParams)[1];
			cout << m_pkEdit->m_iEditMask << endl;
			break;
		}
		break;

	case ZGM_COMMAND:
		int iControllID; iControllID = ((int*)pkParams)[0];
		switch(iControllID)
		{
		case ID_ELEVATIONMODE_RADIOGROUP:	// raise
			m_pkEdit->m_iMode = RAISE;
			break;
		case ID_ELEVATIONMODE_RADIOGROUP+1: // lower
			m_pkEdit->m_iMode = LOWER;
			break;
		case ID_ELEVATIONMODE_RADIOGROUP+2: // smooth
			m_pkEdit->m_iMode = SMOOTH;
			break;
		case ID_AUTOSMOOTHGROUND_CHB:
			m_pkEdit->m_iSmooth = m_pkGuiBuilder->IsButtonChecked("AutoSmoothGroundChB");
			break;
		case ID_LOADMADFILE_BN:
			Object* pkCurObject;
			if((pkCurObject=m_pkEdit->m_pkCurentChild) != NULL)
			{
				Property* pkMadProp;
				if((pkMadProp=pkCurObject->GetProperty("MadProperty")) == NULL)
					if((pkMadProp=pkCurObject->AddProperty("MadProperty")) == NULL)
						break;
				ZGuiListitem* pkItem = ((ZGuiListbox*)m_pkGuiMan->Wnd(
					"MadFileList"))->GetSelItem();

				if(pkItem)
				{
					string szMadFile = pkItem->GetText();
					pkMadProp->SetValue("m_kMadFile", "data/mad/" + szMadFile);

					// Plocka bort bollen.
					Property* pkModelProp;
					if((pkModelProp=pkCurObject->GetProperty("ModelProperty")) != NULL)
						pkCurObject->RemoveProperty(pkModelProp);
				}
			}
			break;
		case ID_CREATEMADFILE_BN:
			Object *pkNewObject;
			//pkNewObject = new BallObject();					
			pkNewObject = m_pkEdit->pkObjectMan->CreateObject();	//  new BallObject();					
			pkNewObject->SetPos(m_pkEdit->m_kDrawPos);	
			pkNewObject->SetPos(m_pkEdit->m_kDrawPos);					
			
			pkNewObject->AttachToClosestZone();
			m_pkEdit->m_pkCurentChild=pkNewObject;

			int* pkParams;
			pkParams = new int[1];
			pkParams[0] = (int) ID_LOADMADFILE_BN; // control id
			DlgProc(pkWnd, uiMessage,1,pkParams);
			delete[] pkParams;

			((EditPropertyDlg*)m_pkGuiBuilder->GetDlg("PropertyDlg"))->m_pkCurrentChild = pkNewObject;
			m_pkGuiBuilder->UpdatePropertybox();
			break;

		case ID_CAMERAMODE_RADIOGROUP:
			m_pkEdit->m_eCameraMode = FreeFlight;
			break;

		case ID_LANDSCAPEPOLYMODE_RADIOGROUP:
			m_pkEdit->pkLevelMan->ChangeLandscapeFillMode(FILL);
			break;

		case ID_LANDSCAPEPOLYMODE_RADIOGROUP+1:
			m_pkEdit->pkLevelMan->ChangeLandscapeFillMode(LINE);
			break;

		case ID_CAMERAMODE_RADIOGROUP+1:
			m_pkEdit->m_eCameraMode = Precision;
			break;

		case ID_LOCKMASKCOLOR_SLIDER_CH:
			m_bLockMaskSliders = m_pkGuiBuilder->IsButtonChecked("LockMaskcolorSliderCb");
			if(m_bLockMaskSliders)
			{
				ZGuiSlider* pkArray[] =
				{
					(ZGuiSlider*)m_pkGuiMan->Wnd("SliderRBn"), 
					(ZGuiSlider*)m_pkGuiMan->Wnd("SliderGBn"),
					(ZGuiSlider*)m_pkGuiMan->Wnd("SliderBBn"), 
				};

				ZGuiTextbox* pkTextBoxes[] = 
				{
					(ZGuiTextbox*)m_pkGuiMan->Wnd("MaskColorREB"),
					(ZGuiTextbox*)m_pkGuiMan->Wnd("MaskColorGEB"),
					(ZGuiTextbox*)m_pkGuiMan->Wnd("MaskColorBEB"),
				};

				for(int i=0; i<3; i++)
				{
					pkArray[i]->RemoveAllBuddys();

					for(int j=0; j<3; j++)
						pkArray[i]->AddBuddyWindow(pkTextBoxes[j]);
				}
			}
			else
			{
				((ZGuiSlider*)m_pkGuiMan->Wnd("SliderRBn"))->RemoveAllBuddys();
				((ZGuiSlider*)m_pkGuiMan->Wnd("SliderGBn"))->RemoveAllBuddys();
				((ZGuiSlider*)m_pkGuiMan->Wnd("SliderBBn"))->RemoveAllBuddys();
				((ZGuiSlider*)m_pkGuiMan->Wnd("SliderABn"))->RemoveAllBuddys();

				((ZGuiSlider*)m_pkGuiMan->Wnd("SliderRBn"))->AddBuddyWindow( m_pkGuiMan->Wnd("MaskColorREB") );
				((ZGuiSlider*)m_pkGuiMan->Wnd("SliderGBn"))->AddBuddyWindow( m_pkGuiMan->Wnd("MaskColorGEB") );
				((ZGuiSlider*)m_pkGuiMan->Wnd("SliderBBn"))->AddBuddyWindow( m_pkGuiMan->Wnd("MaskColorBEB") );
				((ZGuiSlider*)m_pkGuiMan->Wnd("SliderABn"))->AddBuddyWindow( m_pkGuiMan->Wnd("MaskColorAEB") );
			}
			break;
		}
		break;

	case ZGM_EN_CHANGE:
		char* szText = NULL;
		int value = -1;
		int id = ((int*)pkParams)[0];
		ZGuiWnd* pkEBox = NULL;

		switch(((int*)pkParams)[0])
		{
		case ID_MASKCOLOR_RED_EB:
			szText = (pkEBox = m_pkGuiMan->Wnd("MaskColorREB"))->GetText(); 
			break;
		case ID_MASKCOLOR_GREEN_EB:
			szText = (pkEBox = m_pkGuiMan->Wnd("MaskColorGEB"))->GetText(); 
			break;
		case ID_MASKCOLOR_BLUE_EB: 
			szText = (pkEBox = m_pkGuiMan->Wnd("MaskColorBEB"))->GetText(); 
			break;
		case ID_MASKCOLOR_ALPHA_EB:
			szText = (pkEBox = m_pkGuiMan->Wnd("MaskColorAEB"))->GetText(); 
			break;
		case ID_DRAWRATE_EB:
			if((szText = (pkEBox = m_pkGuiMan->Wnd("DrawRateEB"))->GetText()))
			{
				bool bBad = false;
				int v = atoi(szText);
				if(v < 0) 
					{ bBad = true; v = 0; } 
				else 
					if(v > 200) 
					{ v = 200; bBad = true; }

				if(bBad)
				{
					char szNewText[50];
					sprintf(szNewText, "%i", v);
					pkEBox->SetText(szNewText);
				}
				v = 200-v;
				m_pkEdit->m_fDrawRate = (float) v / 200;
				szText = NULL;
			}
			break;
		case ID_PENSIZE_EB:
			if((szText = (pkEBox = m_pkGuiMan->Wnd("PenSizeEB"))->GetText()))
			{
				bool bBad = false;
				int v = atoi(szText);
				if(v < 0) 
					{ bBad = true; v = 0; } 
				else 
					if(v > 50) 
					{ v = 50; bBad = true; }

				if(bBad)
				{
					char szNewText[50];
					sprintf(szNewText, "%i", v);
					pkEBox->SetText(szNewText);
				}
				m_pkEdit->m_iPencilSize = v;
				szText = NULL;
			}
			break;
		}

		if(szText == NULL)
			break;

		ZGuiSlider* pkSlider; pkSlider = NULL;
		int *res; res = NULL;
		int max=255, min=0;
		
		switch(id)
		{
			case ID_MASKCOLOR_RED_EB:    
				res = &(m_pkEdit->m_iMaskColorR = atoi(szText)); 
				pkSlider = (ZGuiSlider*) m_pkGuiMan->Wnd("SliderRBn"); 
				break;
			case ID_MASKCOLOR_GREEN_EB:  
				res = &(m_pkEdit->m_iMaskColorG = atoi(szText)); 
				pkSlider = (ZGuiSlider*) m_pkGuiMan->Wnd("SliderGBn"); 
				break;
			case ID_MASKCOLOR_BLUE_EB:   
				res = &(m_pkEdit->m_iMaskColorB = atoi(szText)); 
				pkSlider = (ZGuiSlider*) m_pkGuiMan->Wnd("SliderBBn"); 
				break;
			case ID_MASKCOLOR_ALPHA_EB: 
				res = &(m_pkEdit->m_iMaskColorA = atoi(szText)); 
				pkSlider = (ZGuiSlider*) m_pkGuiMan->Wnd("SliderABn"); min = 16; 
				break;
		}

		if(res != NULL)
		{
			if(pkSlider != NULL)
			{
				pkSlider->GetRange(min,max);
			}

			char* szNewText = NULL;

			if(*res < min)   
			{
				*res = min; szNewText = new char[20]; 
				sprintf(szNewText, "%i", *res);
			}
			if(*res > max) 
			{
				*res = max; szNewText = new char[20]; 
				sprintf(szNewText, "%i", *res);
			}

			if(szNewText != NULL)
				pkEBox->SetText(szNewText);

			if(pkSlider != NULL)
			{
				pkSlider->SetPos(*res, true, false);
			}
		}
		break;
	}
	return true;
}

bool WorkPanelDlg::OnOpen(int x, int y)
{
	bool bSuccess = false;

	if(m_pkDlgBox == NULL)
	{
		bSuccess = Create(m_pkEdit->m_iWidth-m_iWidth,20,NULL,NULL);
	}

	return bSuccess;
}

bool WorkPanelDlg::OnClose(bool bSave)
{
	return true;
}

ZGuiTabCtrl* WorkPanelDlg::WorkPanel()
{
	return (ZGuiTabCtrl*) m_pkDlgBox;
}

void WorkPanelDlg::MoveColorSliders(char *szLeader)
{
	ZGuiSlider* pkSlider = (ZGuiSlider*) m_pkGuiMan->Wnd(szLeader);
	ZGuiButton* pkButton = pkSlider->GetButton();

	int x = pkButton->GetWndRect().Left;
	int y = pkButton->GetWndRect().Top;

	char szText[10];
	sprintf(szText, "%i", pkSlider->GetPos());

//	if(strcmp(szLeader,"SliderRBn") != 0) 
		((ZGuiSlider*)m_pkGuiMan->Wnd(szLeader))->GetButton()->SetPos(x,y);
/*	if(strcmp(szLeader,"SliderGBn") != 0) 
		((ZGuiSlider*)m_pkGuiMan->Wnd("SliderGBn"))->GetButton()->SetPos(x,y);
	if(strcmp(szLeader,"SliderBBn") != 0) 
		((ZGuiSlider*)m_pkGuiMan->Wnd("SliderBBn"))->GetButton()->SetPos(x,y);*/
}

void WorkPanelDlg::UpdateMapTexList()
{
	ZGuiCombobox* pkCB = (ZGuiCombobox*) m_pkGuiBuilder->Get("MaskTexCb");

	pkCB->RemoveAllItems();

	string strFullName, strMapName;

	for(unsigned int i=0; i<m_pkEdit->m_pkMap->m_kSets.size(); i++)
	{
		strFullName = m_pkEdit->m_pkMap->m_kSets[i].m_acMask;

		int size = strFullName.length();  
		int pos = strFullName.find_last_of("/");
		if(string::npos !=pos)
			strMapName = strFullName.substr(pos+1,size-pos);
		else
			strMapName = strFullName;

		pkCB->AddItem((char*)strMapName.c_str(), i);
	}
}