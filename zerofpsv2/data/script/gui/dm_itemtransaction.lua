-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="dm/bn1u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="dm/bn1d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin3 = { tex1="dm/bn1f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1=0, tex2="0", tex3="0", tex4="0", tex1a=0, tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=1 }
	Skin5 = { tex1="dm/sell_wnd.bmp", tex2="dm/bd_horz.bmp", tex3="dm/bd_vert.bmp", tex4="dm/bd_corner.bmp",
	 tex1a="0", tex2a="dm/bd_horz_a.bmp", tex3a="dm/bd_vert_a.bmp", tex4a="dm/bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin6 = { tex1="dm/buy_wnd.bmp", tex2="dm/bd_horz.bmp", tex3="dm/bd_vert.bmp", tex4="dm/bd_corner.bmp",
	 tex1a="0", tex2a="dm/bd_horz_a.bmp", tex3a="dm/bd_vert_a.bmp", tex4a="dm/bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"ItemTransactionWnd","","",80,60,640,480,0)
	ChangeSkin("ItemTransactionWnd","Skin4","Window")

	CreateWnd(0,"ItemAddWnd","ItemTransactionWnd","",40,40,200,345,0)
	ChangeSkin("ItemAddWnd","Skin6","Window")

	CreateWnd(0,"ItemRemoveWnd","ItemTransactionWnd","",380,40,200,345,0)
	ChangeSkin("ItemRemoveWnd","Skin5","Window")

	CreateWnd(1,"RemoveItemBn","ItemRemoveWnd","Sell",60,318,70,20,0)
	ChangeSkin("RemoveItemBn","Skin1","Button up")
	ChangeSkin("RemoveItemBn","Skin2","Button down")
	ChangeSkin("RemoveItemBn","Skin3","Button focus")

	CreateWnd(1,"AddItemBn","ItemAddWnd","Buy",60,317,64,20,0)
	ChangeSkin("AddItemBn","Skin1","Button up")
	ChangeSkin("AddItemBn","Skin2","Button down")
	ChangeSkin("AddItemBn","Skin3","Button focus")

	CreateWnd(1,"ItemTransactionCloseBn","ItemTransactionWnd","Done",320-70,400,140,25,0)
	ChangeSkin("ItemTransactionCloseBn","Skin1","Button up")
	ChangeSkin("ItemTransactionCloseBn","Skin2","Button down")
	ChangeSkin("ItemTransactionCloseBn","Skin3","Button focus")

end

-- Mistlands GUI Resource file, END
