-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin2 = { tex1="prev_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin3 = { tex1="prev_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="prev_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="portraits/portrait1.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin6 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=199, bkG=178,
	 bkB=153, borderR=0, borderG=0, borderB=0, bd_size=1, tile=0, trans=0 }
	Skin7 = { tex1="lb_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="lb_bk.bmp", tex2="bd_horz.bmp", tex3="bd_vert.bmp", tex4="bd_corner.bmp",
	 tex1a="0", tex2a="bd_horz_a.bmp", tex3a="bd_vert_a.bmp", tex4a="bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin9 = { tex1="lb_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin10 = { tex1="lb_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin11 = { tex1="lb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=56, borderG=56, borderB=56, bd_size=1, tile=0, trans=0 }
	Skin12 = { tex1="sb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin13 = { tex1="sb_n.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin14 = { tex1="sb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin15 = { tex1="member.bmp", tex2="bd_horz.bmp", tex3="bd_vert.bmp", tex4="bd_corner.bmp",
	 tex1a="0", tex2a="bd_horz_a.bmp", tex3a="bd_vert_a.bmp", tex4a="bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin16 = { tex1="next_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin17 = { tex1="next_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin18 = { tex1="next_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin19 = { tex1="delete_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin20 = { tex1="delete_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin21 = { tex1="delete_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }


	Skin22 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=4, tile=0, trans=1 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
--	CreateWnd(0,"GuiMainWnd","","",0,0,800,600,0)
--	ChangeSkin("GuiMainWnd","Skin1","Window")

	CreateWnd(0,"MembersWnd","","",65,75,688,432,0)
	ChangeSkin("MembersWnd","Skin15","Window")

	CreateWnd(1,"MembersCloseBn","MembersWnd","",660,8,20,20,0)
	ChangeSkin("MembersCloseBn","Skin19","Button up")
	ChangeSkin("MembersCloseBn","Skin20","Button down")
	ChangeSkin("MembersCloseBn","Skin21","Button focus")

	CreateWnd(4,"LevelbarTopic","MembersWnd","Level : 3",137,364,200,20,0)
	ChangeSkin("LevelbarTopic","Skin1","Label")

	CreateWnd(1,"NextMemberBn","MembersWnd","",668,412,20,20,0)
	ChangeSkin("NextMemberBn","Skin16","Button up")
	ChangeSkin("NextMemberBn","Skin17","Button down")
	ChangeSkin("NextMemberBn","Skin18","Button focus")

	CreateWnd(1,"PrevMemberBn","MembersWnd","",0,412,20,20,0)
	ChangeSkin("PrevMemberBn","Skin2","Button up")
	ChangeSkin("PrevMemberBn","Skin3","Button down")
	ChangeSkin("PrevMemberBn","Skin4","Button focus")

	CreateWnd(4,"CurrentMemberNumberLabel","MembersWnd","",688/2-100/2,412,300,20,0)
	ChangeSkin("CurrentMemberNumberLabel","Skin1","Label")

	CreateWnd(4,"Label169","MembersWnd","Name:",135,310,40,20,0)
	ChangeSkin("Label169","Skin1","Label")

	CreateWnd(4,"Label358","MembersWnd","Armour:",319,308,50,20,0)
	ChangeSkin("Label358","Skin1","Label")

	CreateWnd(4,"Label391","MembersWnd","Skills",481,302,140,20,0)
	ChangeSkin("Label391","Skin1","Label")

	CreateWnd(4,"Label464","MembersWnd","Wage:",319,361,50,20,0)
	ChangeSkin("Label464","Skin1","Label")

	CreateWnd(4,"Label478","MembersWnd","Life:",135,335,40,20,0)
	ChangeSkin("Label478","Skin1","Label")

	CreateWnd(4,"MemberNameField","MembersWnd","Elite guy",175,310,140,20,0)
	ChangeSkin("MemberNameField","Skin1","Label")

	CreateWnd(4,"MemberIcon","MembersWnd","",18,309,92,92,0)
	ChangeSkin("MemberIcon","Skin5","Label")

	CreateWnd(4,"Label962","MembersWnd","Speed:",319,335,50,20,0)
	ChangeSkin("Label962","Skin1","Label")

	CreateWnd(4,"LevelbarFront","MembersWnd","",137,364,50,20,0)
	ChangeSkin("LevelbarFront","Skin7","Label")

	CreateWnd(4,"LevelbarBK","MembersWnd","",137,364,165,20,0)
	ChangeSkin("LevelbarBK","Skin6","Label")

	CreateWnd(5,"MemberSkillsLB","MembersWnd","",486,333,182,61,0)
	ChangeSkin("MemberSkillsLB","Skin8","Listbox")
	ChangeSkin("MemberSkillsLB","Skin9","Listbox: Button up")
	ChangeSkin("MemberSkillsLB","Skin10","Listbox: Button down")
	ChangeSkin("MemberSkillsLB","Skin11","Listbox: Button focus")
	ChangeSkin("MemberSkillsLB","Skin12","Listbox: Scrollbar")
	ChangeSkin("MemberSkillsLB","Skin13","Listbox: Scrollbar: Button up")
	ChangeSkin("MemberSkillsLB","Skin14","Listbox: Scrollbar: Button down")
	ChangeSkin("MemberSkillsLB","Skin14","Listbox: Scrollbar: Button focus")

	CreateWnd(4,"MemberLifeField","MembersWnd","25/25",176,333,140,20,0)
	ChangeSkin("MemberLifeField","Skin1","Label")
	
	CreateWnd(4,"MembersArmourField","MembersWnd","= 33",378,308,50,20,0)
	ChangeSkin("MembersArmourField","Skin1","Label")

	CreateWnd(4,"MemberSpeedField","MembersWnd","=22",378,336,50,20,0)
	ChangeSkin("MemberSpeedField","Skin1","Label")

	CreateWnd(4,"MemberWageField","MembersWnd","722%",378,359,50,20,0)
	ChangeSkin("MemberWageField","Skin1","Label")

	CreateWnd(1,"MembersEquipBn","MembersWnd","Equip",72,256,64,20,0)
	ChangeSkin("MembersEquipBn","Skin13","Button up")
	ChangeSkin("MembersEquipBn","Skin14","Button down")
	ChangeSkin("MembersEquipBn","Skin14","Button focus")

	CreateWnd(1,"MembersDropItemBn","MembersWnd","Drop",72,256,64,20,0)
	ChangeSkin("MembersDropItemBn","Skin13","Button up")
	ChangeSkin("MembersDropItemBn","Skin14","Button down")
	ChangeSkin("MembersDropItemBn","Skin14","Button focus")

	CreateWnd(4,"MemberSelItemBorder","MembersWnd","",0,0,32,32,0)
	ChangeSkin("MemberSelItemBorder","Skin22","Label")




end

-- Mistlands GUI Resource file, END
