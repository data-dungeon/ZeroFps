-- Global Skins
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

function InitGUI()

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

	--CreateWnd(Wnd,"TestWnd","test",1,-1,100,100,450,450,0)
	--CreateWnd(TabControl,"TabWnd","test",4,1,20,20,250,120,0)
	--AddTabPage(4,"TabPage1")
	--AddTabPage(4,"ddTa2bPage2")

	--CreateWnd(Scrollbar,"sb","",5,1,20,200,250,40,0)
end