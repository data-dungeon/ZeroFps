-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
--	Skin1 = { tex1="bn1u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
--	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
--	Skin2 = { tex1="bn1d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
--	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
--	Skin3 = { tex1="bn1f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
--	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
--	Skin4 = { tex1="bn1u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
--	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin1 = { tex1="misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="misc_button_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin5 = { tex1="menu_screen.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, 
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

	y = 230

	CreateWnd(1,"StarNewGameBn","DMStartWnd","New",800/2-96/2,y+50*0,96,40,0)
	ChangeSkin("StarNewGameBn","Skin1","Button up")
	ChangeSkin("StarNewGameBn","Skin2","Button down")
	ChangeSkin("StarNewGameBn","Skin1","Button focus")

	CreateWnd(1,"LoadNewGameBn","DMStartWnd","Load",800/2-96/2,y+50*1,96,40,0)
	ChangeSkin("LoadNewGameBn","Skin1","Button up")
	ChangeSkin("LoadNewGameBn","Skin2","Button down")
	ChangeSkin("LoadNewGameBn","Skin1","Button focus")

	CreateWnd(1,"SaveNewGameBn","DMStartWnd","Save",800/2-96/2,y+50*2,96,40,0)
	ChangeSkin("SaveNewGameBn","Skin1","Button up")
	ChangeSkin("SaveNewGameBn","Skin2","Button down")
	ChangeSkin("SaveNewGameBn","Skin1","Button focus")

	CreateWnd(1,"OptionsBn","DMStartWnd","Options",800/2-96/2,y+50*3,96,40,0)
	ChangeSkin("OptionsBn","Skin1","Button up")
	ChangeSkin("OptionsBn","Skin2","Button down")
	ChangeSkin("OptionsBn","Skin1","Button focus")

	CreateWnd(1,"CreditsBn","DMStartWnd","Credits",800/2-96/2,y+50*4,96,40,0)
	ChangeSkin("CreditsBn","Skin1","Button up")
	ChangeSkin("CreditsBn","Skin2","Button down")
	ChangeSkin("CreditsBn","Skin1","Button focus")

	CreateWnd(1,"QuitBn","DMStartWnd","Quit",800/2-96/2,y+50*5,96,40,0)
	ChangeSkin("QuitBn","Skin1","Button up")
	ChangeSkin("QuitBn","Skin2","Button down")
	ChangeSkin("QuitBn","Skin1","Button focus")

	CreateWnd(1,"ContinueGameBn","DMStartWnd","Continue",800-125,y+50*5,96,40,0)
	ChangeSkin("ContinueGameBn","Skin1","Button up")
	ChangeSkin("ContinueGameBn","Skin2","Button down")
	ChangeSkin("ContinueGameBn","Skin1","Button focus")

end

-- Mistlands GUI Resource file, END
