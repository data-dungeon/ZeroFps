-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
--	Skin1 = { tex1="dm/bn1u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
--	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
--	Skin2 = { tex1="dm/bn1d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
--	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
--	Skin3 = { tex1="dm/bn1f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
--	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
--	Skin4 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
--	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin5 = { tex1="cb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=56, borderG=56, borderB=56, bd_size=1, tile=1, trans=0 }
	Skin6 = { tex1="dm/start_base1.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin7 = { tex1="dm/city2.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }
	Skin8 = { tex1="cb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=56, borderG=56, borderB=56, bd_size=1, tile=0, trans=0 }
	Skin9 = { tex1="cb_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin10 = { tex1="cb_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin11 = { tex1="cb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=56, borderG=56, borderB=56, bd_size=1, tile=0, trans=0 }
	Skin12 = { tex1="sb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin13 = { tex1="sb_n.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin14 = { tex1="sb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }


	Skin1 = { tex1="dm/misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="dm/misc_button_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
--	CreateWnd(0,"GuiMainWnd","","",0,0,800,600,0,0,0)
--	ChangeSkin("GuiMainWnd","Skin4","Window")

	CreateWnd(0,"StartNewGameWnd","","",0,0,800,600,0,0,0)
	ChangeSkin("StartNewGameWnd","Skin7","Window")

	CreateWnd(3,"TeamColorCB","StartNewGameWnd","",660,63,97,20,0,0,0)
	ChangeSkin("TeamColorCB","Skin5","Combobox")
	ChangeSkin("TeamColorCB","Skin8","Combobox: Label")
	ChangeSkin("TeamColorCB","Skin5","Combobox: Listbox")
	ChangeSkin("TeamColorCB","Skin9","Combobox: Listbox: Button up")
	ChangeSkin("TeamColorCB","Skin10","Combobox: Listbox: Button down")
	ChangeSkin("TeamColorCB","Skin11","Combobox: Listbox: Button focus")
	ChangeSkin("TeamColorCB","Skin12","Combobox: Listbox: Scrollbar")
	ChangeSkin("TeamColorCB","Skin13","Combobox: Listbox: Scrollbar: Button up")
	ChangeSkin("TeamColorCB","Skin14","Combobox: Listbox: Scrollbar: Button down")
	ChangeSkin("TeamColorCB","Skin14","Combobox: Listbox: Scrollbar: Button focus")

--	CreateWnd(1,"StartNewGameBack","StartNewGameWnd","Back",654,569,62,20,0,0,0)
--	ChangeSkin("StartNewGameBack","Skin1","Button up")
--	ChangeSkin("StartNewGameBack","Skin2","Button down")
--	ChangeSkin("StartNewGameBack","Skin3","Button focus")

	CreateWnd(4,"ClanLabel","StartNewGameWnd","Clan",611,31,33,20,0,0,0)
	ChangeSkin("ClanLabel","Skin4","Label")

	CreateWnd(10,"ClanNameEB","StartNewGameWnd","",658,30,100,20,0,0,0)
	ChangeSkin("ClanNameEB","Skin5","Textbox")

	CreateWnd(4,"ColorLabel","StartNewGameWnd","Color",610,65,38,20,0,0,0)
	ChangeSkin("ColorLabel","Skin4","Label")

	CreateWnd(1,"SelectNextBaseBn","StartNewGameWnd",">",769,203,20,20,0,0,0)
	ChangeSkin("SelectNextBaseBn","Skin1","Button up")
	ChangeSkin("SelectNextBaseBn","Skin2","Button down")
	ChangeSkin("SelectNextBaseBn","Skin1","Button focus")

	CreateWnd(1,"SelectPrevBaseBn","StartNewGameWnd","<",604,202,20,20,0,0,0)
	ChangeSkin("SelectPrevBaseBn","Skin1","Button up")
	ChangeSkin("SelectPrevBaseBn","Skin2","Button down")
	ChangeSkin("SelectPrevBaseBn","Skin1","Button focus")

	CreateWnd(4,"StarBaseLabel","StartNewGameWnd","Start base",664,127,65,20,0,0,0)
	ChangeSkin("StarBaseLabel","Skin4","Label")

	CreateWnd(4,"StartBaseIconLabel","StartNewGameWnd","",634,151,128,128,0,0,0)
	ChangeSkin("StartBaseIconLabel","Skin6","Label")

	CreateWnd(4,"StartBaseNameLabel","StartNewGameWnd","Old warehouse",634,289,128,20,0,0,0)
	ChangeSkin("StartBaseNameLabel","Skin4","Label")

	CreateWnd(1,"StartNewGameDone","StartNewGameWnd","Done",800-96-8,600-40-8,96,40,0,0,0)
	ChangeSkin("StartNewGameDone","Skin1","Button up")
	ChangeSkin("StartNewGameDone","Skin2","Button down")
	ChangeSkin("StartNewGameDone","Skin1","Button focus")

	AddListItem("TeamColorCB", "Red")
	AddListItem("TeamColorCB", "Green")
	AddListItem("TeamColorCB", "Blue")
	AddListItem("TeamColorCB", "White")
	AddListItem("TeamColorCB", "Black")

end

-- Mistlands GUI Resource file, END
