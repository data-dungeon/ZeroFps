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
	Skin4 = { tex1=0, tex2="0", tex3="0", tex4="0", tex1a=0, tex2a="0", tex3a="0", tex4a="0", bkR=0, bkG=0,
	 bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="dm/sell_wnd.bmp", tex2="dm/bd_horz.bmp", tex3="dm/bd_vert.bmp", tex4="dm/bd_corner.bmp",
	 tex1a="0", tex2a="dm/bd_horz_a.bmp", tex3a="dm/bd_vert_a.bmp", tex4a="dm/bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin6 = { tex1="dm/sell_wnd.bmp", tex2="dm/bd_horz.bmp", tex3="dm/bd_vert.bmp", tex4="dm/bd_corner.bmp",
	 tex1a="0", tex2a="dm/bd_horz_a.bmp", tex3a="dm/bd_vert_a.bmp", tex4a="dm/bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin7 = { tex1="dm/next_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="dm/next_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin9 = { tex1="dm/next_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin10 = { tex1="dm/prev_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin11 = { tex1="dm/prev_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin12 = { tex1="dm/prev_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"ItemTransactionWnd","","",80,40,670,475,0)
	ChangeSkin("ItemTransactionWnd","Skin4","Window")

	CreateWnd(4,"ItemAddWnd","ItemTransactionWnd","",40,40,256,348,0)
	ChangeSkin("ItemAddWnd","Skin6","Label")

	CreateWnd(4,"ItemRemoveWnd","ItemTransactionWnd","",380,40,256,348,0)
	ChangeSkin("ItemRemoveWnd","Skin5","Label")

	CreateWnd(1,"RemoveItemBn","ItemTransactionWnd","Sell",380+256/2-70/2,400,70,20,0)
	ChangeSkin("RemoveItemBn","Skin1","Button up")
	ChangeSkin("RemoveItemBn","Skin2","Button down")
	ChangeSkin("RemoveItemBn","Skin3","Button focus")

	CreateWnd(1,"AddItemBn","ItemTransactionWnd","Buy",40+256/2-64/2,400,64,20,0)
	ChangeSkin("AddItemBn","Skin1","Button up")
	ChangeSkin("AddItemBn","Skin2","Button down")
	ChangeSkin("AddItemBn","Skin3","Button focus")

	CreateWnd(1,"ItemTransactionCloseBn","ItemTransactionWnd","Done",320-70,400,140,25,0)
	ChangeSkin("ItemTransactionCloseBn","Skin1","Button up")
	ChangeSkin("ItemTransactionCloseBn","Skin2","Button down")
	ChangeSkin("ItemTransactionCloseBn","Skin3","Button focus")

	CreateWnd(1,"ItemAddTransactionNextPageBn","ItemTransactionWnd","",40+230,40+318,20,20,0)
	ChangeSkin("ItemAddTransactionNextPageBn","Skin7","Button up")
	ChangeSkin("ItemAddTransactionNextPageBn","Skin8","Button down")
	ChangeSkin("ItemAddTransactionNextPageBn","Skin9","Button focus")

	CreateWnd(1,"ItemAddTransactionPrevPageBn","ItemTransactionWnd","",40+4,40+318,20,20,0)
	ChangeSkin("ItemAddTransactionPrevPageBn","Skin10","Button up")
	ChangeSkin("ItemAddTransactionPrevPageBn","Skin11","Button down")
	ChangeSkin("ItemAddTransactionPrevPageBn","Skin12","Button focus")

	CreateWnd(1,"ItemRemoveTransactionNextPageBn","ItemTransactionWnd","",380+230,40+318,20,20,0)
	ChangeSkin("ItemRemoveTransactionNextPageBn","Skin7","Button up")
	ChangeSkin("ItemRemoveTransactionNextPageBn","Skin8","Button down")
	ChangeSkin("ItemRemoveTransactionNextPageBn","Skin9","Button focus")

	CreateWnd(1,"ItemRemoveTransactionPrevPageBn","ItemTransactionWnd","",380+4,40+318,20,20,0)
	ChangeSkin("ItemRemoveTransactionPrevPageBn","Skin10","Button up")
	ChangeSkin("ItemRemoveTransactionPrevPageBn","Skin11","Button down")
	ChangeSkin("ItemRemoveTransactionPrevPageBn","Skin12","Button focus")

	CreateWnd(3,"ActiveContatinerList","ItemTransactionWnd","",320-70,440,140,25,0)
	ClearListbox("ActiveContatinerList")
	AddListItem("ActiveContatinerList", "Backpack")
	AddListItem("ActiveContatinerList", "Armor")
	AddListItem("ActiveContatinerList", "Cybernetics")
	AddListItem("ActiveContatinerList", "Quickitem")
	AddListItem("ActiveContatinerList", "Weapon")



end

-- Mistlands GUI Resource file, END
