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
DefLBitemUSkin = { tex1= "lb_d.bmp" }
DefLBitemDSkin = { tex1= "lb_d.bmp" }
DefLBitemFSkin = { tex1= "lb_f.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
DefLBBkSkin = { tex1= "lb_bk.bmp", borderR=56, borderG=56, borderB=56, bd_size=1 }
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
BackPackWndSkin = { tex1= "backpack_wnd.bmp", bd_size = 3 }
BackPackWndSkin = { tex1= "backpack_wnd.bmp", bd_size = 3 }
StatsWndSkin	= { tex1= "stats_wnd.bmp", bd_size = 3 }
MapWndSkin	= { tex1= "map_wnd.bmp", tex1a= "map_wnd_a.bmp" }


function CreateMainWnds()

	-- Constants to use to identify controlls
	local Wnd		= 0
	local Button		= 1
	local Checkbox		= 2
	local Combobox		= 3
	local Label		= 4
	local Listbox		= 5
	local Radiobutton	= 6
	local Scrollbar		= 7
	local Slider		= 8
	local TabControl	= 9
	local Textbox		= 10
	local Treebox		= 11

	local MainWnd		= 1;
	local LifeBarProgress   = 2;
	local ManabarProgress   = 3;
	local MapButton		= 4;
	local BackpackButton	= 5;
	local StatsButton	= 6;

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
	CreateWnd(Button,"StatsButton","",StatsButton,MainWnd,w-190,h-90,64,64,0)
	ChangeSkin(StatsButton, "StatsSkinUp", "Button up")
	ChangeSkin(StatsButton, "StatsSkinDown", "Button down")
	ChangeSkin(StatsButton, "StatsSkinFocus", "Button focus")
end

function OnClickBackpack()

	local w = GetScreenWidth()
	local h = GetScreenHeight()

	local Wnd	= 0

	local MainWnd	  = 1
	local BackPackWnd = 100

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

	local Wnd	= 0

	local MainWnd	  = 1
	local StatisticWnd = 101

	if IsWndVisible("StatsWnd") == 1 then 
		CloseWnd("StatsWnd")
	else 
		CreateWnd(Wnd, "StatsWnd", "",StatisticWnd,MainWnd,3,3,400,570,0)
		ChangeSkin(StatisticWnd, "StatsWndSkin", "Window")
	end
end

function OnClickMap()

	local w = GetScreenWidth()
	local h = GetScreenHeight()

	local Wnd	= 0

	local MainWnd	  = 1
	local MapWnd	= 102

	if IsWndVisible("MapWnd") == 1 then 
		CloseWnd("MapWnd")
	else 
		CreateWnd(Wnd, "MapWnd", "",MapWnd,MainWnd,w-400,0,400,400,0)
		ChangeSkin(MapWnd, "MapWndSkin", "Window")
	end
end