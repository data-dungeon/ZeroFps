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
DefCBnUSkin = {	tex1="cbn_u.bmp" }
DefCBnDSkin = {	tex1="cbn_d.bmp" }
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
DefCBitemUSkin = { tex1= "cb_u.bmp" }
DefCBitemDSkin = { tex1= "cb_d.bmp" }
DefCBitemFSkin = { tex1= "cb_f.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
DefCBBkSkin = { tex1= "cb_bk.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
DefCBTopItemSkin = { tex1= "cb_topItem.bmp" }
DefTextboxSkin = { tex1= "textbox.bmp", borderR=56, borderG=56, borderB=56, bd_size=1, tile=1 }
DefTreeboxBkSkin = { tex1= "treebox_bk.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
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
LifeBarForegroundSkin = { tex1= "healthbar_forground.bmp", tex1a = "healthbar_forground_a.bmp" }
LifeBarSkin = { bkR=214, bkG=0, bkB=0, borderR=0, borderG=0, borderB=0, bd_size=1, trans=0 }
ManaBarSkin = { bkR=0, bkG=0, bkB=214, borderR=0, borderG=0, borderB=0, bd_size=1, trans=0 }
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
StatsWndSkin	= { tex1= "stats_wnd.bmp" }
MapWndSkin	= { tex1= "map_wnd.bmp"  }
RecordLabelSkin = { tex1= "record_label.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp",  tex4="wndborder_corner.bmp", bd_size=8}
CloseContainerButtonSkinUp = { tex1= "backpackbackbutton_u.bmp"}
CloseContainerButtonSkinDown = { tex1= "backpackbackbutton_d.bmp"}
CloseContainerButtonSkinFocus = { tex1= "backpackbackbutton_f.bmp"}

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

	local w = GetScreenWidth()
	local h = GetScreenHeight()

	-- typ, wnd name, parent namn, label, x, y, w, h, flags
	CreateWnd(Wnd,"MainWnd","","",0,0,w,h,0)

	-- Set main window skin to hollow
	ChangeSkin("MainWnd", "NullSkin", "Window")

	-- Create life bar
	CreateWnd(Label,"LifeBar","MainWnd","",0,h-20,w/2,20,0)
	CreateWnd(Label,"LifeBarProgress","MainWnd","",10, h-20,w/2-20,20,0)
	ChangeSkin("LifeBar", "LifeBarForegroundSkin", "Label")
	ChangeSkin("LifeBarProgress", "ManaBarSkin", "Label")

	-- Create mana bar
	CreateWnd(Label,"ManaBar","MainWnd","",w/2,h-20,w/2,20,0)
	CreateWnd(Label,"ManaBarProgress","MainWnd","",w/2+10,h-20,w/2-20,20,0)
	ChangeSkin("ManaBar", "LifeBarForegroundSkin", "Label")
	ChangeSkin("ManaBarProgress", "LifeBarSkin", "Label")
	
	-- Create map button
	CreateWnd(Button,"MapButton","MainWnd","",w-40,h-60,32,32,0)
	ChangeSkin("MapButton", "MapSkinUp", "Button up")
	ChangeSkin("MapButton", "MapSkinDown", "Button down")
	ChangeSkin("MapButton", "MapSkinFocus", "Button focus")

	-- Create backpack button
	CreateWnd(Button,"BackPackButton","MainWnd","",w-80,h-60,32,32,0)
	ChangeSkin("BackPackButton", "BackPackSkinUp", "Button up")
	ChangeSkin("BackPackButton", "BackPackSkinDown", "Button down")
	ChangeSkin("BackPackButton", "BackPackSkinFocus", "Button focus")

	-- Create stats button
	CreateWnd(Button,"StatsButton","MainWnd","",w-120,h-60,32,32,0)
	ChangeSkin("StatsButton", "StatsSkinUp", "Button up")
	ChangeSkin("StatsButton", "StatsSkinDown", "Button down")
	ChangeSkin("StatsButton", "StatsSkinFocus", "Button focus")
end

function OnClickBackpack()

	local w = GetScreenWidth()
	local h = GetScreenHeight()

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

	local w = GetScreenWidth()
	local h = GetScreenHeight()

	if IsWndVisible("StatsWnd") == 1 then 
		PlayGuiSound("close_window.wav",0)
		CloseWnd("StatsWnd")
	else 
		local wnd_exist = GetWnd("StatsWnd")
		PlayGuiSound("open_window.wav",0)
		
		CreateWnd(Wnd, "StatsWnd", "MainWnd","",8,8,400-8,570-8,0)

		-- Only create controlls ones
		if wnd_exist == -1 then
			
			CreateWnd(Label, "RecordLabel", "StatsWnd", "", 160, 0, 64,24,0)
		        ChangeSkin("RecordLabel", "RecordLabelSkin", "Label")

			CreateWnd(Listbox, "AbilityList", "StatsWnd","",20,50,168,225,ReanOnly)

			ClearListbox("AbilityList")
			AddListItem("AbilityList", "Jump 10")
			AddListItem("AbilityList", "Hide 19")

			CreateWnd(Label,"StyLabel","StatsWnd","Sty",300-50,45,50,20,0)
			CreateWnd(Label,"ConLabel","StatsWnd","Con",300-50,45+25*1,50,20,0)
			CreateWnd(Label,"DexLabel","StatsWnd","Dex",300-50,45+25*2,50,20,0)
			CreateWnd(Label,"IntLabel","StatsWnd","Int",300-50,45+25*3,50,20,0)
			CreateWnd(Label,"PieLabel","StatsWnd","Pie",300-50,45+25*4,50,20,0)

			CreateWnd(Textbox,"StyTextbox","StatsWnd","14",300,45,50,20,ReanOnly)
			CreateWnd(Textbox,"ConTextbox","StatsWnd","10",300,45+25*1,50,20,ReanOnly)
			CreateWnd(Textbox,"DexTextbox","StatsWnd","18",300,45+25*2,50,20,ReanOnly)
			CreateWnd(Textbox,"IntTextbox","StatsWnd","12",300,45+25*3,50,20,ReanOnly)
			CreateWnd(Textbox,"PieTextbox","StatsWnd","4", 300,45+25*4,50,20,ReanOnly)

			SetTextInt("StyTextbox", 12)
			SetTextInt("ConTextbox", 15)
			SetTextInt("DexTextbox", 11)
			SetTextInt("IntTextbox", 18)
			SetTextInt("PieTextbox", 13)

		end 
	end
end

function OnClickMap()

	local w = GetScreenWidth()
	local h = GetScreenHeight()

	if IsWndVisible("MapWnd") == 1 then 
		PlayGuiSound("close_window.wav",0)
		CloseWnd("MapWnd")
	else 
		PlayGuiSound("open_window.wav",0)
		CreateWnd(Wnd, "MapWnd", "MainWnd","",w-400,0,400,400,0)
		ChangeSkin("MapWnd", "MapWndSkin", "Window")
	end
end