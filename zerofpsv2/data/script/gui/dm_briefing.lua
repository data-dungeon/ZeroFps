-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin3 = { tex1="bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
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
	Skin9 = { tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"GuiMainWnd","","",0,0,800,600,0)
	ChangeSkin("GuiMainWnd","Skin4","Window")

	CreateWnd(0,"BriefingWnd","","",81,53,667,425,0)
	ChangeSkin("BriefingWnd","Skin9","Window")

	CreateWnd(1,"BriefingAcceptBn","BriefingWnd","Accept",30,387,134,20,0)
	ChangeSkin("BriefingAcceptBn","Skin1","Button up")
	ChangeSkin("BriefingAcceptBn","Skin2","Button down")
	ChangeSkin("BriefingAcceptBn","Skin3","Button focus")

	CreateWnd(1,"BriefingCancelBn","BriefingWnd","Cacel",196,388,134,20,0)
	ChangeSkin("BriefingCancelBn","Skin1","Button up")
	ChangeSkin("BriefingCancelBn","Skin2","Button down")
	ChangeSkin("BriefingCancelBn","Skin3","Button focus")

	CreateWnd(10,"M.Textbox467","BriefingWnd","",8,8,600,340,0)
	ChangeSkin("M.Textbox467","Skin5","Textbox")
	ChangeSkin("M.Textbox467","Skin6","Textbox: Scrollbar")
	ChangeSkin("M.Textbox467","Skin7","Textbox: Scrollbar: Button up")
	ChangeSkin("M.Textbox467","Skin8","Textbox: Scrollbar: Button down")
	ChangeSkin("M.Textbox467","Skin8","Textbox: Scrollbar: Button focus")

end

-- Mistlands GUI Resource file, END
