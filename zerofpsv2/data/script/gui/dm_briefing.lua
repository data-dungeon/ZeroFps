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
	Skin4 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin5 = { tex1="cb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=56, borderG=56, borderB=56, bd_size=1, tile=1, trans=0 }
	Skin6 = { tex1="sb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin7 = { tex1="sb_n.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="sb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin9 = { tex1="0", tex2="dm/bd_horz.bmp", tex3="dm/bd_vert.bmp", tex4="dm/bd_corner.bmp",
	 tex1a="0", tex2a="dm/bd_horz_a.bmp", tex3a="dm/bd_vert_a.bmp", tex4a="dm/bd_corner_a.bmp", bkR=0, bkG=0, bkB=0, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"GuiMainWnd","","",0,0,800,600,0)
	ChangeSkin("GuiMainWnd","Skin4","Window")

	CreateWnd(0,"BriefingWnd","","",81,73,667,415,0)
	ChangeSkin("BriefingWnd","Skin9","Window")

	CreateWnd(1,"BriefingAcceptBn","BriefingWnd","Accept",30,387,134,20,0)
	ChangeSkin("BriefingAcceptBn","Skin1","Button up")
	ChangeSkin("BriefingAcceptBn","Skin2","Button down")
	ChangeSkin("BriefingAcceptBn","Skin3","Button focus")

	CreateWnd(1,"BriefingCancelBn","BriefingWnd","Cacel",196,388,134,20,0)
	ChangeSkin("BriefingCancelBn","Skin1","Button up")
	ChangeSkin("BriefingCancelBn","Skin2","Button down")
	ChangeSkin("BriefingCancelBn","Skin3","Button focus")

	CreateWnd(10,"MissionDetailEb","BriefingWnd","",8,8,600,360,0)
	ChangeSkin("MissionDetailEb","Skin5","Textbox")
	ChangeSkin("MissionDetailEb","Skin6","Textbox: Scrollbar")
	ChangeSkin("MissionDetailEb","Skin7","Textbox: Scrollbar: Button up")
	ChangeSkin("MissionDetailEb","Skin8","Textbox: Scrollbar: Button down")
	ChangeSkin("MissionDetailEb","Skin8","Textbox: Scrollbar: Button focus")

end

-- Mistlands GUI Resource file, END
