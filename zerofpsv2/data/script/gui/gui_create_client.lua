-- defualt Skins
-- Valid parameters and there default values:
--	tex1=NULL, tex2=NULL, tex3=NULL, tex4=NULL, tex1a=NULL, tex2a=NULL, tex3a=NULL, tex4a=NULL, 
--	bkR=255, bkG=255, bkB=255, borderR=0, borderG=0, borderB=0, bd_size=0, tile=0, trans=0
DefWndSkin = {	tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp",  tex4="wndborder_corner.bmp", bd_size=8, tile=1  }
DefBnUSkin = {	tex1="bn_u.bmp" }
DefBnDSkin = {	tex1="bn_d.bmp" }
DefBnFSkin = {	tex1="bn_f.bmp" }
DefRBnUSkin = {	tex1="rbn_u.bmp", tex1a="rbn_a.bmp" }
DefRBnDSkin = {	tex1="rbn_d.bmp", tex1a="rbn_a.bmp" }
DefCBnUSkin = {	tex1="cbn_u.bmp", tile=1 }
DefCBnDSkin = {	tex1="cbn_d.bmp", tile=1 }
DefSBrNSkin = {	tex1="sb_n.bmp" }
DefSBrFSkin = {	tex1="sb_f.bmp" }
DefSBrScrollUpSkin_u = { tex1="scrollbar_clicktop_bn_u.bmp" }
DefSBrScrollUpSkin_d = { tex1="scrollbar_clicktop_bn_d.bmp" }
DefSBrScrollDownSkin_u = { tex1="scrollbar_clickbottom_bn_u.bmp" }
DefSBrScrollDownSkin_d = { tex1="scrollbar_clickbottom_bn_d.bmp" }
DefSBrBkSkin = { tex1="sb_bk.bmp", tile=1 }
DefSliderSkin = { tex1="slider.bmp", tex1a="slider_a.bmp" }
DefSliderBkSkin = { tex1= "lb_u.bmp" }
DefLBitemUSkin = { tex1= "lb_u.bmp" }
DefLBitemDSkin = { tex1= "lb_d.bmp" }
DefLBitemFSkin = { tex1= "lb_f.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
DefLBBkSkin = { tex1= "lb_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp",  tex4="wndborder_corner.bmp", bd_size=8, tile=1 }
DefCBitemUSkin = { tex1= "cb_u.bmp", tile=1 }
DefCBitemDSkin = { tex1= "cb_d.bmp", tile=1 }
DefCBitemFSkin = { tex1= "cb_f.bmp", borderR=56, borderG=56, borderB=56, bd_size=1, tile=1 }
DefCBBkSkin = { tex1= "cb_bk.bmp", borderR=56, borderG=56, borderB=56, bd_size=1, tile=1 }
DefCBTopItemSkin = { tex1= "sb_bk.bmp", borderR=56, borderG=56, borderB=56, bd_size=1, tile=1, tile=1 }
DefTextboxSkin = { tex1= "textbox.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
DefTreeboxBkSkin = { tex1= "treebox_bk.bmp", borderR=56, borderG=56, borderB=56, bd_size=1, tile=1 }
DefTreeNodeChildSkin = { tex1= "tn_c.bmp" }
DefTreeNodeParentClosedSkin = { tex1= "tn_pc.bmp" }
DefTreeNodeParentOpenSkin = { tex1= "tn_po.bmp" }
DefTabCtrlBkSkin = { bkR=128, bkG=128, bkB=128, borderR=92, borderG=92, borderB=92, bd_size=1 }
DefTabBnPrevUSkin = { tex1= "tab_bnPrev_u.bmp" }
DefTabBnPrevDSkin = { tex1= "tab_bnPrev_d.bmp" }
DefTabBnNextUSkin = { tex1= "tab_bnNext_u.bmp" }
DefTabBnNextDSkin = { tex1= "tab_bnNext_d.bmp" }
DefTabPageBackSkin = { bkR=128, bkG=128, bkB=128, borderR=92, borderG=92, borderB=92, bd_size=1 }
DefTabPageFrontSkin = { bkR=214, bkG=211, bkB=206 }

-- custom skins
NullSkin  = { trans=1 }
LifeBarSkin = { tex1 = "progress_bar_bk.bmp", tex1a= "progress_bar_bk_a.bmp", tile=1 }
ManaBarSkin = { tex1 = "progress_bar_bk.bmp", tex1a= "progress_bar_bk_a.bmp", tile=1 }
MapSkinUp     = { tex1= "map_up.bmp" }
MapSkinFocus  = { tex1= "map_focus.bmp" }
MapSkinDown   = { tex1= "map_down.bmp" }
BackPackSkinUp     = { tex1= "backpack_up.bmp" }
BackPackSkinFocus  = { tex1= "backpack_focus.bmp" }
BackPackSkinDown   = { tex1= "backpack_down.bmp" }
StatsSkinUp     = { tex1= "stats_up.bmp" }
StatsSkinFocus  = { tex1= "stats_focus.bmp" }
StatsSkinDown   = { tex1= "stats_down.bmp" }
BackPackWndSkin = { tex1= "backpack_wnd.bmp" }
StatsWndSkin	= { tex1= "character_screen.bmp" }
MapWndSkin	= { tex1= "map_wnd.bmp"  }
RecordLabelSkin = { tex1= "record_label.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp",  tex4="wndborder_corner.bmp", bd_size=8}
CloseContainerButtonSkinUp = { tex1= "backpackbackbutton_u.bmp"}
CloseContainerButtonSkinDown = { tex1= "backpackbackbutton_d.bmp"}
CloseContainerButtonSkinFocus = { tex1= "backpackbackbutton_f.bmp"}
PanelBkSkin = { tex1= "panel_bk.bmp", tex1a = "panel_bk_a.bmp" }
ToggleInputBoxBnUp = { tex1= "toggle_input_up.bmp"}
ToggleInputBoxBnDown = { tex1= "toggle_input_down.bmp"}
ToggleInputBoxBnFocus = { tex1= "toggle_input_focus.bmp"}
ToggleInfoBoxBnUp = { tex1= "toggle_infobox_up.bmp"}
ToggleInfoBoxBnDown = { tex1= "toggle_infobox_down.bmp"}
ToggleInfoBoxBnFocus = { tex1= "toggle_infobox_focus.bmp"}
SelectSpellSkinUp = { tex1= "magicbutton_u.bmp"}
SelectSpellSkinDown = { tex1= "magicbutton_d.bmp"}
SelectSpellSkinFocus = { tex1= "magicbutton_f.bmp"}
SelectSkillSkinUp = { tex1= "skillbutton_u.bmp"}
SelectSkillSkinDown = { tex1= "skillbutton_d.bmp"}
SelectSkillSkinFocus = { tex1= "skillbutton_f.bmp"}
InputWndSkin = { tex1= "input_wnd.bmp", tex1a= "input_wnd_a.bmp"}
SendInputBoxBnSkinUp = { tex1= "sendinputboxbn_u.bmp" }
SendInputBoxBnSkinDown = { tex1= "sendinputboxbn_d.bmp" }
SendInputBoxBnSkinFocus = { tex1= "sendinputboxbn_f.bmp" }
InfoBoxWndSkin = { tex1="info_box.bmp", tex1a="info_box_a.bmp" }
IntroScreenSkin = { tex1="splash_screen.bmp" }

-- Constants to use to identify controlls
Wnd		= 0
Button		= 1
Checkbox	= 2
Combobox	= 3
Label		= 4
Listbox		= 5
Radiobutton	= 6
Scrollbar	= 7
Slider		= 8
TabControl	= 9
Textbox		= 10
Treebox		= 11

-- Window Flags
MultiLine		= 1
ReanOnly		= 2

function CreateMainWnds()

	local w = 800 --GetScreenWidth()
	local h = 600 --GetScreenHeight()

	-- typ, wnd name, parent namn, label, x, y, w, h, flags
	CreateWnd(Wnd,"MainWnd","","",0,0,w,h,0)

	-- Set main window skin to hollow
	ChangeSkin("MainWnd", "NullSkin", "Window")
end

function CreatePlayerPanel()

	local w = 800 --GetScreenWidth()
	local h = 600 --GetScreenHeight()

	local pmax = w/2-15

	-- Create panel bar
	CreateWnd(Wnd,"PanelBkWnd","MainWnd","",0, h-110+2, w+2, 110, 0)
	ChangeSkin("PanelBkWnd", "PanelBkSkin", "Window")

	-- Create mana bar
	CreateWnd(Label,"ManaBarProgress","PanelBkWnd","",10, 94,pmax/2,8,0) 
	ChangeSkin("ManaBarProgress", "ManaBarSkin", "Label")

	-- Create life bar
	CreateWnd(Label,"LifeBarProgress","PanelBkWnd","",w/2+10,94,pmax/2,8,0)
	ChangeSkin("LifeBarProgress", "LifeBarSkin", "Label")
	
	-- Create map button
	CreateWnd(Button,"MapButton","PanelBkWnd","",w-40,56,32,32,0)
	ChangeSkin("MapButton", "MapSkinUp", "Button up")
	ChangeSkin("MapButton", "MapSkinDown", "Button down")
	ChangeSkin("MapButton", "MapSkinFocus", "Button focus")

	-- Create backpack button
	CreateWnd(Button,"BackPackButton","PanelBkWnd","",w-80,56,32,32,0)
	ChangeSkin("BackPackButton", "BackPackSkinUp", "Button up")
	ChangeSkin("BackPackButton", "BackPackSkinDown", "Button down")
	ChangeSkin("BackPackButton", "BackPackSkinFocus", "Button focus")

	-- Create stats button
	CreateWnd(Button,"StatsButton","PanelBkWnd","",w-120,56,32,32,0)
	ChangeSkin("StatsButton", "StatsSkinUp", "Button up")
	ChangeSkin("StatsButton", "StatsSkinDown", "Button down")
	ChangeSkin("StatsButton", "StatsSkinFocus", "Button focus")

	-- Select spell button
	CreateWnd(Button,"SelectSpellBn","PanelBkWnd","",w-160,56,32,32,0)
	ChangeSkin("SelectSpellBn", "SelectSpellSkinUp", "Button up")
	ChangeSkin("SelectSpellBn", "SelectSpellSkinDown", "Button down")
	ChangeSkin("SelectSpellBn", "SelectSpellSkinFocus", "Button focus")

	-- Select skill button
	CreateWnd(Button,"SelectSkillBn","PanelBkWnd","",w-200,56,32,32,0)
	ChangeSkin("SelectSkillBn", "SelectSkillSkinUp", "Button up")
	ChangeSkin("SelectSkillBn", "SelectSkillSkinDown", "Button down")
	ChangeSkin("SelectSkillBn", "SelectSkillSkinFocus", "Button focus")

	-- Toogle input button
	CreateWnd(Button,"ToggleInputBoxBn","PanelBkWnd","",w-240,56,32,32,0)
	ChangeSkin("ToggleInputBoxBn", "ToggleInputBoxBnUp", "Button up")
	ChangeSkin("ToggleInputBoxBn", "ToggleInputBoxBnDown", "Button down")
	ChangeSkin("ToggleInputBoxBn", "ToggleInputBoxBnFocus", "Button focus")

	-- Toogle infobox button
	CreateWnd(Button,"ToggleInfoBoxBn","PanelBkWnd","",w-280,56,32,32,0)
	ChangeSkin("ToggleInfoBoxBn", "ToggleInfoBoxBnUp", "Button up")
	ChangeSkin("ToggleInfoBoxBn", "ToggleInfoBoxBnDown", "Button down")
	ChangeSkin("ToggleInfoBoxBn", "ToggleInfoBoxBnFocus", "Button focus")

	-- Create infobox
	CreateWnd(Wnd,"InfoBoxWnd","PanelBkWnd","",0, 0, w/2+23, 91, 0)
	ChangeSkin("InfoBoxWnd", "InfoBoxWndSkin", "Window")   

	CreateWnd(Textbox, "InfoBox", "InfoBoxWnd", "", 10, 10, w/2-30, 76, MultiLine)

	-- Create input wnd
	CreateWnd(Wnd, "InputWnd", "PanelBkWnd", "", w/2+35, 19, w/2-50, 32, 0)
	ChangeSkin("InputWnd", "InputWndSkin", "Window")
	ShowWnd("InputWnd", 0, 0)
	
	CreateWnd(Textbox, "InputBox", "InputWnd", "", 10, 8, w/2-92, 16, 0);

	CreateWnd(Button, "SendInputBoxBn", "InputWnd", "", w/2-92+12, 6, 20, 20, 0);
	ChangeSkin("SendInputBoxBn", "SendInputBoxBnSkinUp", "Button up")
	ChangeSkin("SendInputBoxBn", "SendInputBoxBnSkinDown", "Button down")
	ChangeSkin("SendInputBoxBn", "SendInputBoxBnSkinUp", "Button focus")   

end

function OnClickBackpack()

	local w = 800 --GetScreenWidth()
	local h = 600 --GetScreenHeight()

	if IsWndVisible("BackPackWnd") == 1 then 
		PlayGuiSound("close_window.wav",0)
		CloseWnd("BackPackWnd")
	else 
		PlayGuiSound("open_window.wav",0)

		local wnd_exist = GetWnd("BackPackWnd")

		CreateWnd(Wnd, "BackPackWnd", "MainWnd","",0,0,512,288,0)

		if wnd_exist == -1 then
			ChangeSkin("BackPackWnd", "BackPackWndSkin", "Window")

			CreateWnd(Textbox,"GoldTextbox","BackPackWnd","",32,258,64,20,ReanOnly)
			SetTextInt("GoldTextbox", 12457)

			CreateWnd(Textbox,"CurrentWeightTextbox","BackPackWnd","",128,258,32,20,ReanOnly)
			SetTextInt("CurrentWeightTextbox", 123)

			CreateWnd(Textbox,"MaxWeightTextbox","BackPackWnd","",174,258,32,20,ReanOnly)
			SetTextInt("MaxWeightTextbox", 512)

			CreateWnd(Textbox,"ArmorClassBackpackTextbox","BackPackWnd","",238,258,32,20,ReanOnly)
			SetTextInt("ArmorClassBackpackTextbox", -19)

			CreateWnd(Textbox,"CurrentHPBackpackTextbox","BackPackWnd","",308,258,32,20,ReanOnly)
			SetTextInt("CurrentHPBackpackTextbox", 298)

			CreateWnd(Textbox,"MaxHPBackpackTextbox","BackPackWnd","",356,258,32,20,ReanOnly)
			SetTextInt("MaxHPBackpackTextbox", 512)

			CreateWnd(Button,"BackPackBackButton","BackPackWnd","",472,259,28,19,0)
			ChangeSkin("BackPackBackButton", "CloseContainerButtonSkinUp", "Button up")
			ChangeSkin("BackPackBackButton", "CloseContainerButtonSkinDown", "Button down")
			ChangeSkin("BackPackBackButton", "CloseContainerButtonSkinFocus", "Button focus")

			CreateWnd(Scrollbar, "SlotListScrollbar", "BackPackWnd", "", 496, 8, 8, 240, 0)
		end
	end
end

function OnClickStats()



	local w = 800 --GetScreenWidth()
	local h = 600 --GetScreenHeight()

	if IsWndVisible("StatsWnd") == 1 then 
		PlayGuiSound("close_window.wav",0)
		CloseWnd("StatsWnd")
	else 
		local wnd_exist = GetWnd("StatsWnd")
		PlayGuiSound("open_window.wav",0)
		
		CreateWnd(Wnd, "StatsWnd", "MainWnd","",0,288,512,283,0)

		-- Only create controlls ones
		if wnd_exist == -1 then
			
			ChangeSkin("StatsWnd", "StatsWndSkin", "Window")

			CreateWnd(Listbox,"SkillsListbox","StatsWnd","",144,24,187,251,0)
			CreateWnd(Listbox,"AttackDefListbox","StatsWnd","",339,134,167,142,0)

			CreateWnd(Label,"CharacterPortrait","StatsWnd","",8,8,128,128,0)

			CreateWnd(Label,"CharacterNameLabel","StatsWnd","",339,24,165,18,0)
			CreateWnd(Label,"CharacterRaceLabel","StatsWnd","",339,66,165,18,0)
			CreateWnd(Label,"CharacterSexLabel","StatsWnd","",427,92,77,16,0)

			CreateWnd(Label,"CharacterAttribSTYLabel","StatsWnd","",76,144,60,18,0)
			CreateWnd(Label,"CharacterAttribCONLabel","StatsWnd","",76,144+18*1,60,18,0)
			CreateWnd(Label,"CharacterAttribDEXLabel","StatsWnd","",76,144+18*2,60,18,0)
			CreateWnd(Label,"CharacterAttribINTLabel","StatsWnd","",76,144+18*3,60,18,0)
			CreateWnd(Label,"CharacterAttribPIELabel","StatsWnd","",76,144+18*4,60,18,0)

			CreateWnd(Label,"CharacterCounterHPLabel","StatsWnd","",76,240+18*0,60,18,0)
			CreateWnd(Label,"CharacterCounterMPLabel","StatsWnd","",76,240+18*1,60,18,0)
		end 
	end
end

function OnClickMap()

	local w = 800 --GetScreenWidth()
	local h = 600 --GetScreenHeight()

	if IsWndVisible("MapWnd") == 1 then 
		PlayGuiSound("close_window.wav",0)
		CloseWnd("MapWnd")
	else 
		PlayGuiSound("open_window.wav",0)
		CreateWnd(Wnd, "MapWnd", "MainWnd","",w-400,0,400,400,0)
		ChangeSkin("MapWnd", "MapWndSkin", "Window")
	end
end

function OnClickToggleInput()
	if IsWndVisible("InputWnd") == 1 then 
		PlayGuiSound("open_window2.wav",0)
		ShowWnd("InputWnd", 0, 0)
	else 
		PlayGuiSound("close_window2.wav",0)
		ShowWnd("InputWnd", 1, 0)
	end
end

function OnClickToggleInfoBox()
	if IsWndVisible("InfoBoxWnd") == 1 then 
		PlayGuiSound("open_window2.wav",0)
		ShowWnd("InfoBoxWnd", 0, 0)
	else 
		PlayGuiSound("close_window2.wav",0)
		ShowWnd("InfoBoxWnd", 1, 0)
	end
end

function CreateIntroScene()

	local w = 800 --GetScreenWidth()
	local h = 600 --GetScreenHeight()

	--PlayGuiSound("grotta2_mono.wav",0)

	CreateWnd(Wnd,"IntroWnd","MainWnd","",0,0,w,h,0)
	ChangeSkin("IntroWnd", "IntroScreenSkin", "Window")

	CreateWnd(Button, "OpenConnectButton", "IntroWnd", "Connect", w/2-170/2, h/2-20/2, 170, 20, 0);
	ChangeSkin("OpenConnectButton", "DefBnUSkin", "Button up")
	ChangeSkin("OpenConnectButton", "DefBnDSkin", "Button down")
	ChangeSkin("OpenConnectButton", "DefBnFSkin", "Button focus")
end

function OnClickConnect()
	
	local w = 800 --GetScreenWidth()
	local h = 600 --GetScreenHeight()

	if IsWndVisible("IPNumbersComboBox") == 0 then 

		CreateWnd(Label, "PresetLabel", "IntroWnd", "Presets", w/2-150/2-75, h/2-20/2+30, 170, 20, 0)
		CreateWnd(Combobox, "IPNumbersComboBox", "IntroWnd", "192.168.0.127", w/2-150/2+25, h/2-20/2+30, 170, 20, 0)

		CreateWnd(Label, "IPLabel", "IntroWnd", "Server IP", w/2-150/2-75, h/2-20/2+52, 150, 20, 0)
		CreateWnd(Textbox, "IPNumberEditbox", "IntroWnd", "", w/2-150/2+25, h/2-20/2+52, 170, 20, 0)

		CreateWnd(Label, "UserNameLabel", "IntroWnd", "Name", w/2-150/2-75, h/2-20/2+82, 170, 20, 0)
		CreateWnd(Textbox, "UserEditbox", "IntroWnd", "Anonymus", w/2-150/2+25, h/2-20/2+82, 170, 20, 0)

		CreateWnd(Label, "PasswordLabel", "IntroWnd", "Password", w/2-150/2-75, h/2-20/2+104, 170, 20, 0)
		CreateWnd(Textbox, "PasswordEditbox", "IntroWnd", "", w/2-150/2+25, h/2-20/2+104, 170, 20, 0)

		CreateWnd(Button, "ConnectToServerButton", "IntroWnd", "OK", w/2-50/2+30, h/2-20/2+130, 50, 20, 0)

	else
		ShowWnd("PresetLabel", 0, 0)
		ShowWnd("IPNumbersComboBox", 0, 0)
		ShowWnd("IPLabel", 0, 0)
		ShowWnd("IPNumberEditbox", 0, 1)
		ShowWnd("UserNameLabel", 0, 0)
		ShowWnd("UserEditbox", 0, 0)
		ShowWnd("ConnectToServerButton", 0, 0)
		ShowWnd("PasswordLabel", 0, 0)
		ShowWnd("PasswordEditbox", 0, 0)
	end

end

function TestTableApa( apa )
	Print( "olle\n\n\n" );
end