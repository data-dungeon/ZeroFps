-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="dm/misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="dm/misc_button_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1=0, tex2="0", tex3="0", tex4="0", tex1a=0, tex2a="0", tex3a="0", tex4a="0", bkR=0, bkG=0,
	 bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="dm/sell_wnd.bmp", tex2="dm/bd_horz.bmp", tex3="dm/bd_vert.bmp", tex4="dm/bd_corner.bmp",
	 tex1a="0", tex2a="dm/bd_horz_a.bmp", tex3a="dm/bd_vert_a.bmp", tex4a="dm/bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin6 = { tex1="dm/sell_wnd.bmp", tex2="dm/bd_horz.bmp", tex3="dm/bd_vert.bmp", tex4="dm/bd_corner.bmp",
	 tex1a="0", tex2a="dm/bd_horz_a.bmp", tex3a="dm/bd_vert_a.bmp", tex4a="dm/bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }

	


	Skin7 = { tex1="dm/final/members/right_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="dm/final/members/right_arrow_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin9 = { tex1="dm/final/members/right_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin10 = { tex1="dm/final/members/left_arrow.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin11 = { tex1="dm/final/members/left_arrow_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin12 = { tex1="dm/final/members/left_arrow.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }



	Skin13 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a=0, tex2a="0", tex3a="0", tex4a="0",bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=1, tile=0, trans=0 }



	Skin14 = { tex1="dm/final/win_tile.bmp", 
	 tex2="dm/final/window/win_edge_ver.bmp", 
	 tex3="dm/final/window/win_edge_hor.bmp", 
	 tex4="dm/final/window/dm_win_corn.bmp", tex1a="0", 
	 tex2a="dm/final/window/win_edge_ver_a.bmp", 
	 tex3a="dm/final/window/win_edge_hor_a.bmp", 
	 tex4a="dm/final/window/dm_win_corn_a.bmp", bkR=255, bkG=255, bkB=255,
	 borderR=255, borderG=255, borderB=255, bd_size=32, tile=1, trans=0 }


	Skin15 = { tex1="dm/final/members/x-button.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/members/x-button_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }
	Skin16 = { tex1="dm/final/members/x-button_d.bmp", tex2="0", tex3="0", tex4="0",
	 tex1a="dm/final/members/x-button_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }

	Skin17 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a=0, tex2a="0", tex3a="0", tex4a="0",bkR=128, bkG=128,
	 bkB=128, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"ItemTransactionWnd","","",80,40,670,475,0)
	ChangeSkin("ItemTransactionWnd","Skin14","Window")

	CreateWnd(4,"ItemAddWnd","ItemTransactionWnd","",40,40,256,348,0)
	ChangeSkin("ItemAddWnd","Skin6","Label")

	CreateWnd(4,"ItemRemoveWnd","ItemTransactionWnd","",380,40,256,348,0)
	ChangeSkin("ItemRemoveWnd","Skin5","Label")


	CreateWnd(4,"ItemInfoLabel","ItemTransactionWnd","",40,440,600,20,0)  
	ChangeSkin("ItemInfoLabel","Skin13","Label")


	CreateWnd(4,"ItemAddContainerLabel","ItemTransactionWnd","Shop",40+4+20,40+318,230-20,20,0)
	ChangeSkin("ItemAddContainerLabel","Skin17","Label")


	CreateWnd(4,"ItemRemoveContainerLabel","ItemTransactionWnd","Backpack",380+4+20,40+318,230-20,20,0)
	ChangeSkin("ItemRemoveContainerLabel","Skin17","Label")






	CreateWnd(1,"RemoveItemBn","ItemTransactionWnd","Sell",380+256/2-70/2,400,70,20,0)
	ChangeSkin("RemoveItemBn","Skin1","Button up")
	ChangeSkin("RemoveItemBn","Skin2","Button down")
	ChangeSkin("RemoveItemBn","Skin1","Button focus")

	CreateWnd(1,"AddItemBn","ItemTransactionWnd","Buy",40+256/2-64/2,400,64,20,0)
	ChangeSkin("AddItemBn","Skin1","Button up")
	ChangeSkin("AddItemBn","Skin2","Button down")
	ChangeSkin("AddItemBn","Skin1","Button focus")

	CreateWnd(1,"ItemTransactionCloseBn","ItemTransactionWnd","",670-32,0,32,32,0)
	ChangeSkin("ItemTransactionCloseBn","Skin15","Button up")
	ChangeSkin("ItemTransactionCloseBn","Skin16","Button down")
	ChangeSkin("ItemTransactionCloseBn","Skin16","Button focus")

	CreateWnd(1,"AgentsClose","AgentsWnd","",472,9,32,32,0)
	ChangeSkin("AgentsClose","Skin15","Button up")
	ChangeSkin("AgentsClose","Skin16","Button down")
	ChangeSkin("AgentsClose","Skin15","Button focus")

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


end

-- Mistlands GUI Resource file, END
