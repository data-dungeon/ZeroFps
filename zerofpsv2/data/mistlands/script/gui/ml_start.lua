-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="misc_button_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin3 = { tex1="wnd_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=0,
	 bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin4 = { tex1="misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=192,
	 bkG=192, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"MLStartWnd","","",0,0,800,600,0,0,3)
	ChangeSkin("DMStartWnd","Skin3","Window")

	CreateWnd(1,"LoadNewGameBn","MLStartWnd","Load",352,280,96,40,0,4,0)
	ChangeSkin("LoadNewGameBn","Skin1","Button up")
	ChangeSkin("LoadNewGameBn","Skin2","Button down")
	ChangeSkin("LoadNewGameBn","Skin1","Button focus")

	CreateWnd(1,"QuitBn","MLStartWnd","Quit",352,426,96,40,0,4,0)
	ChangeSkin("QuitBn","Skin1","Button up")
	ChangeSkin("QuitBn","Skin2","Button down")
	ChangeSkin("QuitBn","Skin1","Button focus")

	CreateWnd(1,"SaveNewGameBn","MLStartWnd","Save",352,330,96,40,0,4,0)
	ChangeSkin("SaveNewGameBn","Skin1","Button up")
	ChangeSkin("SaveNewGameBn","Skin2","Button down")
	ChangeSkin("SaveNewGameBn","Skin1","Button focus")

	CreateWnd(1,"StarNewGameBn","MLStartWnd","New",352,230,96,40,0,4,0)
	ChangeSkin("StarNewGameBn","Skin4","Button up")
	ChangeSkin("StarNewGameBn","Skin2","Button down")
	ChangeSkin("StarNewGameBn","Skin1","Button focus")

	CreateWnd(1,"ContinueGameBn","MLStartWnd","Continue",675,480,96,40,0,4,0)
	ChangeSkin("ContinueGameBn","Skin1","Button up")
	ChangeSkin("ContinueGameBn","Skin2","Button down")
	ChangeSkin("ContinueGameBn","Skin1","Button focus")

	CreateWnd(1,"CreditsBn","MLStartWnd","Credits",352,377,96,40,0,4,0)
	ChangeSkin("CreditsBn","Skin1","Button up")
	ChangeSkin("CreditsBn","Skin2","Button down")
	ChangeSkin("CreditsBn","Skin1","Button focus")

end

-- Mistlands GUI Resource file, END
