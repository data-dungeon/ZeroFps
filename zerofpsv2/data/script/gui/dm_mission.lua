-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="bn1u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
--	Skin2 = { tex1="bn1d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
--	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
--	Skin3 = { tex1="bn1f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
--	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin5 = { tex1="label_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=56, borderG=56, borderB=56, bd_size=1, tile=1, trans=0 }
	Skin6 = { tex1="0", tex2="bd_horz.bmp", tex3="bd_vert.bmp", tex4="bd_corner.bmp",
	 tex1a="0", tex2a="bd_horz_a.bmp", tex3a="bd_vert_a.bmp", tex4a="bd_corner_a.bmp", bkR=0, bkG=0, bkB=0, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin7 = { tex1="sb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }


	Skin8 = { tex1="final/members/up_arrow.bmp", 
	tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin9 = { tex1="final/members/up_arrow_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin2 = { tex1="misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin3 = { tex1="misc_button_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }


	Skin10 = { tex1="final/win_tile.bmp", 
	 tex2="final/window/win_edge_ver.bmp", 
	 tex3="final/window/win_edge_hor.bmp", 
	 tex4="final/window/dm_win_corn.bmp", tex1a="0", 
	 tex2a="final/window/win_edge_ver_a.bmp", 
	 tex3a="final/window/win_edge_hor_a.bmp", 
	 tex4a="final/window/dm_win_corn_a.bmp", bkR=255, bkG=255, bkB=255,
	 borderR=255, borderG=255, borderB=255, bd_size=32, tile=1, trans=0 }

	Skin11 = { tex1="final/members/thumb_bn.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin12 = { tex1="final/members/down_arrow.bmp", 
	tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin13 = { tex1="final/members/down_arrow_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------

	CreateWnd(0,"MissionWnd","","",291,136,287,318,0)
	ChangeSkin("MissionWnd","Skin10","Window")

	CreateWnd(7,"MissionScrollbar","MissionWnd","",248,14,20,260,0)
	ChangeSkin("MissionScrollbar","Skin5","Scrollbar")

	ChangeSkin("MissionScrollbar","Skin11","Scrollbar: Button up")
	ChangeSkin("MissionScrollbar","Skin11","Scrollbar: Button down")
	ChangeSkin("MissionScrollbar","Skin11","Scrollbar: Button focus")

	ChangeSkin("MissionScrollbar","Skin8","Scrollbar: Top: Button up")
	ChangeSkin("MissionScrollbar","Skin9","Scrollbar: Top: Button down")
	ChangeSkin("MissionScrollbar","Skin8","Scrollbar: Top: Button focus")

	ChangeSkin("MissionScrollbar","Skin12","Scrollbar: Bottom: Button up")
	ChangeSkin("MissionScrollbar","Skin13","Scrollbar: Bottom: Button down")
	ChangeSkin("MissionScrollbar","Skin12","Scrollbar: Bottom: Button focus")



	CreateWnd(1,"BriefingBn","MissionWnd","Briefing",28,286,93,20,0)
	ChangeSkin("BriefingBn","Skin2","Button up")
	ChangeSkin("BriefingBn","Skin3","Button down")
	ChangeSkin("BriefingBn","Skin2","Button focus")

	CreateWnd(10,"Mission_A_Eb","MissionWnd","",27,15,220,85,0)
	ChangeSkin("Mission_A_Eb","Skin5","Textbox")

	CreateWnd(10,"Mission_B_Eb","MissionWnd","",27,101,220,85,0)
	ChangeSkin("Mission_B_Eb","Skin5","Textbox")

	CreateWnd(10,"Mission_C_Eb","MissionWnd","",27,187,220,85,0)
	ChangeSkin("Mission_C_Eb","Skin5","Textbox")

	CreateWnd(1,"MissionCancelBn","MissionWnd","Cancel",175,286,93,20,0)
	ChangeSkin("MissionCancelBn","Skin2","Button up")
	ChangeSkin("MissionCancelBn","Skin3","Button down")
	ChangeSkin("MissionCancelBn","Skin2","Button focus")

end

-- Mistlands GUI Resource file, END
