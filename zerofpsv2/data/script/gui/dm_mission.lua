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
	Skin6 = { tex1="0", tex2="dm/bd_horz.bmp", tex3="dm/bd_vert.bmp", tex4="dm/bd_corner.bmp",
	 tex1a="0", tex2a="dm/bd_horz_a.bmp", tex3a="dm/bd_vert_a.bmp", tex4a="dm/bd_corner_a.bmp", bkR=0, bkG=0, bkB=0, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin7 = { tex1="sb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin8 = { tex1="sb_n.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin9 = { tex1="sb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"GuiMainWnd","","",0,0,800,600,0)
	ChangeSkin("GuiMainWnd","Skin4","Window")

	CreateWnd(0,"MissionWnd","","",291,136,287,318,0)
	ChangeSkin("MissionWnd","Skin6","Window")

	CreateWnd(7,"V.Scrollbar169","MissionWnd","",248,14,20,260,0)
	ChangeSkin("V.Scrollbar169","Skin7","Scrollbar")
	ChangeSkin("V.Scrollbar169","Skin8","Scrollbar: Button up")
	ChangeSkin("V.Scrollbar169","Skin9","Scrollbar: Button down")
	ChangeSkin("V.Scrollbar169","Skin9","Scrollbar: Button focus")

	CreateWnd(1,"BriefingBn","MissionWnd","Briefing",28,286,93,20,0)
	ChangeSkin("BriefingBn","Skin1","Button up")
	ChangeSkin("BriefingBn","Skin2","Button down")
	ChangeSkin("BriefingBn","Skin3","Button focus")

	CreateWnd(10,"Mission_A_Eb","MissionWnd","",27,101,220,85,0)
	ChangeSkin("Mission_A_Eb","Skin5","Textbox")

	CreateWnd(10,"Mission_B_Eb","MissionWnd","",27,187,220,85,0)
	ChangeSkin("Mission_B_Eb","Skin5","Textbox")

	CreateWnd(10,"Mission_C_Eb","MissionWnd","",27,15,220,85,0)
	ChangeSkin("Mission_C_Eb","Skin5","Textbox")

	CreateWnd(1,"MissionCancelBn","MissionWnd","Cancel",175,286,93,20,0)
	ChangeSkin("MissionCancelBn","Skin1","Button up")
	ChangeSkin("MissionCancelBn","Skin2","Button down")
	ChangeSkin("MissionCancelBn","Skin3","Button focus")

end

-- Mistlands GUI Resource file, END
