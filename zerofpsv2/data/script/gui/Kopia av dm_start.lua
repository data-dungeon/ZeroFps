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
	Skin4 = { tex1="dm/bn1u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="dm/city1.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, 
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin6 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"DMStartWnd","","",0,0,800,600,0)
	ChangeSkin("DMStartWnd","Skin5","Window")

--	CreateWnd(0,"GuiMainWnd","","",0,0,800,600,0)
--	ChangeSkin("GuiMainWnd","Skin6","Window")

	CreateWnd(1,"LoadNewGameBn","DMStartWnd","Load",365,122,70,20,0)
	ChangeSkin("LoadNewGameBn","Skin4","Button up")
	ChangeSkin("LoadNewGameBn","Skin2","Button down")
	ChangeSkin("LoadNewGameBn","Skin3","Button focus")

	CreateWnd(1,"OptionsBn","DMStartWnd","Options",365,176,70,20,0)
	ChangeSkin("OptionsBn","Skin4","Button up")
	ChangeSkin("OptionsBn","Skin2","Button down")
	ChangeSkin("OptionsBn","Skin3","Button focus")

	CreateWnd(1,"QuitBn","DMStartWnd","Quit",365,233,70,20,0)
	ChangeSkin("QuitBn","Skin4","Button up")
	ChangeSkin("QuitBn","Skin2","Button down")
	ChangeSkin("QuitBn","Skin3","Button focus")

	CreateWnd(1,"StarNewGameBn","DMStartWnd","New",365,96,70,20,0)
	ChangeSkin("StarNewGameBn","Skin4","Button up")
	ChangeSkin("StarNewGameBn","Skin2","Button down")
	ChangeSkin("StarNewGameBn","Skin3","Button focus")

	CreateWnd(1,"SaveNewGameBn","DMStartWnd","Save",365,148,70,20,0)
	ChangeSkin("SaveNewGameBn","Skin1","Button up")
	ChangeSkin("SaveNewGameBn","Skin2","Button down")
	ChangeSkin("SaveNewGameBn","Skin3","Button focus")

	CreateWnd(1,"CreditsBn","DMStartWnd","Credits",365,205,70,20,0)
	ChangeSkin("CreditsBn","Skin4","Button up")
	ChangeSkin("CreditsBn","Skin2","Button down")
	ChangeSkin("CreditsBn","Skin3","Button focus")


	CreateWnd(1,"ContinueGameBn","DMStartWnd","Continue",365,260,70,20,0)
	ChangeSkin("ContinueGameBn","Skin4","Button up")
	ChangeSkin("ContinueGameBn","Skin2","Button down")
	ChangeSkin("ContinueGameBn","Skin3","Button focus")

end

-- Mistlands GUI Resource file, END
