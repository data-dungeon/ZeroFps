-- defualt Skins
-- Valid parameters and there default values:
--	tex1=NULL, tex2=NULL, tex3=NULL, tex4=NULL, tex1a=NULL, tex2a=NULL, tex3a=NULL, tex4a=NULL, 
--	bkR=255, bkG=255, bkB=255, borderR=0, borderG=0, borderB=0, bd_size=0, tile=0, trans=0
DefWndSkin = {	tex1="defwnd.bmp", tile=1  }
DefBnUSkin = {	tex1="bn_u.bmp" }
DefBnDSkin = {	tex1="bn_d.bmp" }
DefBnFSkin = {	tex1="bn_f.bmp" }
DefRBnUSkin = {	tex1="rbn_u.bmp", tex1a="rbn_a.bmp" }
DefRBnDSkin = {	tex1="rbn_d.bmp", tex1a="rbn_a.bmp" }
DefCBnUSkin = {	tex1="cbn_u.bmp" }
DefCBnDSkin = {	tex1="cbn_d.bmp" }
DefSBrNSkin = {	tex1="sb_n.bmp" }
DefSBrFSkin = {	tex1="sb_f.bmp" }
DefSBrBkSkin = { tex1="sb_bk.bmp" }
DefSliderSkin = { tex1="slider.bmp", tex1a="slider_a.bmp" }
DefSliderBkSkin = { tex1= "lb_u.bmp" }
DefLBitemUSkin = { tex1= "lb_u.bmp" }
DefLBitemDSkin = { tex1= "lb_d.bmp" }
DefLBitemFSkin = { tex1= "lb_f.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
DefLBBkSkin = { tex1= "lb_bk.bmp", borderR=56, borderG=56, borderB=56, bd_size=1, tile=1 }
DefCBitemUSkin = { tex1= "cb_u.bmp" }
DefCBitemDSkin = { tex1= "cb_d.bmp" }
DefCBitemFSkin = { tex1= "cb_f.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
DefCBBkSkin = { tex1= "cb_bk.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
DefCBTopItemSkin = { tex1= "cb_topItem.bmp" }
DefTextboxSkin = { tex1= "textbox.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
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
MapSkinUp     = { tex1= "map_up.bmp", tex1a = "map_up_a.bmp" }
MapSkinFocus  = { tex1= "map_focus.bmp", tex1a = "map_focus_a.bmp" }
MapSkinDown   = { tex1= "map_down.bmp", tex1a = "map_down_a.bmp" }
BackPackSkinUp     = { tex1= "backpack_up.bmp", tex1a = "backpack_up_a.bmp" }
BackPackSkinFocus  = { tex1= "backpack_focus.bmp", tex1a = "backpack_up_a.bmp" }
BackPackSkinDown   = { tex1= "backpack_down.bmp", tex1a = "backpack_up_a.bmp" }
StatsSkinUp     = { tex1= "stats_up.bmp", tex1a = "stats_up_a.bmp" }
StatsSkinFocus  = { tex1= "stats_focus.bmp", tex1a = "stats_up_a.bmp" }
StatsSkinDown   = { tex1= "stats_down.bmp", tex1a = "stats_up_a.bmp" }
BackPackWndSkin = { tex1= "backpack_wnd.bmp" }
StatsWndSkin	= { tex1= "stats_wnd.bmp" }
MapWndSkin	= { tex1= "map_wnd.bmp", tex1a= "map_wnd_a.bmp" }

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

-- Unique ID numbers for Windows
MainWnd			= 1
LifeBarProgress		= 2
ManabarProgress		= 3
MapButton		= 4
BackpackButton		= 5
StatsButton		= 6
BackPackWnd		= 7
StatisticWnd		= 8
AbilityList		= 9
MapWnd			= 10
StyTextbox		= 11
ConTextbox		= 12
DexTextbox		= 13
IntTextbox		= 14
PieTextbox		= 15

-- Window Flags
MultiLine		= 1
ReanOnly		= 2

function CreateMainWnds()

	local w = GetScreenWidth()
	local h = GetScreenHeight()

	-- typ, idnamn, label, id, parent, x, y, w, h, flags
	CreateWnd(Wnd,"MainWnd","",MainWnd,-1,0,0,w,h,0)

	-- Set main window skin to hollow
	ChangeSkin(MainWnd, "NullSkin", "Window")

	-- Create life bar
	CreateWnd(Label,"LifeBar","",123,MainWnd,0,h-20,w/2,20,0)
	CreateWnd(Label,"LifeBarProgress","",LifeBarProgress,MainWnd,10,h-20,w/2-20,20,0)
	ChangeSkin(123, "LifeBarForegroundSkin", "Label")
	ChangeSkin(LifeBarProgress, "ManaBarSkin", "Label")

	-- Create mana bar
	CreateWnd(Label,"ManaBar","",125,MainWnd,w/2,h-20,w/2,20,0)
	CreateWnd(Label,"ManaBarProgress","",ManabarProgress,MainWnd,w/2+10,h-20,w/2-20,20,0)
	ChangeSkin(125, "LifeBarForegroundSkin", "Label")
	ChangeSkin(ManabarProgress, "LifeBarSkin", "Label")
	
	-- Create map button
	CreateWnd(Button,"MapButton","",MapButton,MainWnd,w-60,h-45,64,32,0)
	ChangeSkin(MapButton, "MapSkinUp", "Button up")
	ChangeSkin(MapButton, "MapSkinDown", "Button down")
	ChangeSkin(MapButton, "MapSkinFocus", "Button focus")

	-- Create backpack button
	CreateWnd(Button,"BackPackButton","",BackpackButton,MainWnd,w-128,h-80,64,64,0)
	ChangeSkin(BackpackButton, "BackPackSkinUp", "Button up")
	ChangeSkin(BackpackButton, "BackPackSkinDown", "Button down")
	ChangeSkin(BackpackButton, "BackPackSkinFocus", "Button focus")

	-- Create stats button
	CreateWnd(Button,"StatsButton","",StatsButton,MainWnd,w-190,h-80,64,64,0)
	ChangeSkin(StatsButton, "StatsSkinUp", "Button up")
	ChangeSkin(StatsButton, "StatsSkinDown", "Button down")
	ChangeSkin(StatsButton, "StatsSkinFocus", "Button focus")
end

function OnClickBackpack()

	local w = GetScreenWidth()
	local h = GetScreenHeight()

	if IsWndVisible("BackPackWnd") == 1 then 
		CloseWnd("BackPackWnd")
	else 
		CreateWnd(Wnd, "BackPackWnd", "",BackPackWnd,MainWnd,w/2-200,h/2-200,400,400,0)
		ChangeSkin(BackPackWnd, "BackPackWndSkin", "Window")
	end
end

function OnClickStats()

	local w = GetScreenWidth()
	local h = GetScreenHeight()

	if IsWndVisible("StatsWnd") == 1 then 
		CloseWnd("StatsWnd")
	else 
		local wnd_exist = GetWnd("StatsWnd")

		CreateWnd(Wnd, "StatsWnd", "",StatisticWnd,MainWnd,0,0,400,570,0)

		-- Only create controlls ones
		if wnd_exist == -1 then
			CreateWnd(Listbox, "AbilityList", "",AbilityList,StatisticWnd,20,50,168,225,0)
			ChangeSkin(StatisticWnd, "StatsWndSkin", "Window")

			ClearListbox("AbilityList")
			AddListItem("AbilityList", "Jump 10")
			AddListItem("AbilityList", "Hide 19")

			CreateWnd(Textbox,"StyTextbox","14",StyTextbox,StatisticWnd,300,45,50,20,ReanOnly)
			CreateWnd(Textbox,"ConTextbox","10",ConTextbox,StatisticWnd,300,45+25*1,50,20,ReanOnly)
			CreateWnd(Textbox,"DexTextbox","18",DexTextbox,StatisticWnd,300,45+25*2,50,20,ReanOnly)
			CreateWnd(Textbox,"IntTextbox","12",IntTextbox,StatisticWnd,300,45+25*3,50,20,ReanOnly)
			CreateWnd(Textbox,"PieTextbox","4", PieTextbox,StatisticWnd, 300,45+25*4,50,20,ReanOnly)

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
		CloseWnd("MapWnd")
	else 
		CreateWnd(Wnd, "MapWnd", "",MapWnd,MainWnd,w-400,0,400,400,0)
		ChangeSkin(MapWnd, "MapWndSkin", "Window")
	end
end