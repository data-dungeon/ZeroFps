-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="dm/final/members/abilities.bmp", tex2="0", tex3="0", tex4="0",
	 tex1a="dm/final/members/abilities_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }
	Skin2 = { tex1="dm/final/members/armor.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/members/armor_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }
	Skin3 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin4 = { tex1="dm/final/members/belt.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/members/belt_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0
	 }
	Skin5 = { tex1="dm/final/members/panel.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/members/panel_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }
	Skin6 = { tex1="dm/final/win_tile.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin7 = { tex1="dm/final/members/character.bmp", tex2="0", tex3="0", tex4="0",
	 tex1a="dm/final/members/character_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }
	Skin8 = { tex1="dm/final/members/x-button.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/members/x-button_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }
	Skin9 = { tex1="dm/final/members/x-button_d.bmp", tex2="0", tex3="0", tex4="0",
	 tex1a="dm/final/members/x-button_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }
	Skin10 = { tex1="dm/final/members/cybernetics.bmp", tex2="0", tex3="0", tex4="0",
	 tex1a="dm/final/members/cybernetics_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }
	Skin11 = { tex1="dm/final/members/inventory.bmp", tex2="0", tex3="0", tex4="0",
	 tex1a="dm/final/members/inventory_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }
	Skin12 = { tex1="dm/final/big_corner_270.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/big_corner_270_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }
	Skin13 = { tex1="dm/final/big_corner_180.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/big_corner_180_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }
	Skin14 = { tex1="dm/final/members/right_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin15 = { tex1="dm/final/members/right_arrow_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin16 = { tex1="dm/final/members/left_arrow.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin17 = { tex1="dm/final/members/left_arrow_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin18 = { tex1="dm/final/big_corner.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/big_corner_a.bmp", tex2a="0",
	 tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin19 = { tex1="dm/final/big_corner_90.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/big_corner_90_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }
	Skin20 = { tex1="dm/final/members/weapon.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/members/weapon_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }

	Skin22 = { tex1="lb_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin23 = { tex1="dm/final/members/drop_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/members/drop_button_a.bmp", 
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin24 = { tex1="dm/final/members/drop_button_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/members/drop_button_a.bmp", 
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }



	Skin25 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=50, bkG=50,
	 bkB=153, borderR=0, borderG=0, borderB=0, bd_size=1, tile=0, trans=0 }
	Skin26 = { tex1="lb_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=128, bkG=32,
	 bkB=32, borderR=32, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin27 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=92, bkG=92,
	 bkB=92, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }

	Skin28 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=92, bkG=92,
	 bkB=92, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }


	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"MembersWnd","","",800/2-600/2,600/2-450/2,600,450,0)
	ChangeSkin("MembersWnd","Skin6","Window")




	CreateWnd(4,"MemberName","MembersWnd","",30,335+8,345,20,0)
	ChangeSkin("MemberName","Skin27","Label")

	CreateWnd(4,"MemeberArmor","MembersWnd","",30,335+20+8,345,20,0)
	ChangeSkin("MemeberArmor","Skin27","Label")

	CreateWnd(4,"MemberSpeed","MembersWnd","2",30,335+40+8,345,20,0)
	ChangeSkin("MemberSpeed","Skin27","Label")

	CreateWnd(4,"MemeberWage","MembersWnd","",30,335+60+8,345,20,0)
	ChangeSkin("MemeberWage","Skin27","Label")



	CreateWnd(4,"LevelbarTopic","MembersWnd","",20,220-25,200,20,0)
	ChangeSkin("LevelbarTopic","Skin28","Label")


	CreateWnd(4,"HealthBarTopic","MembersWnd","",20,220,200,20,0)
	ChangeSkin("HealthBarTopic","Skin28","Label")

	CreateWnd(4,"character","MembersWnd","",-54,314,512,128,0)
	ChangeSkin("character","Skin7","Label")

	CreateWnd(1,"MembersCloseBn","MembersWnd","",540,12,32,32,0)
	ChangeSkin("MembersCloseBn","Skin8","Button up")
	ChangeSkin("MembersCloseBn","Skin9","Button down")
	ChangeSkin("MembersCloseBn","Skin9","Button focus")

	CreateWnd(4,"cybernetics","MembersWnd","",263,192,128,128,0)
	ChangeSkin("cybernetics","Skin10","Label")

	CreateWnd(4,"inventory_label","MembersWnd","",355,11,256,256,0)
	ChangeSkin("inventory_label","Skin11","Label")

	CreateWnd(4,"lower_left_corner","MembersWnd","",-18,340,128,128,0)
	ChangeSkin("lower_left_corner","Skin12","Label")

	CreateWnd(4,"lower_right_corner","MembersWnd","",490,340,128,128,0)
	ChangeSkin("lower_right_corner","Skin13","Label")

	CreateWnd(1,"NextMemberBn","MembersWnd","",526,20,16,16,0)
	ChangeSkin("NextMemberBn","Skin14","Button up")
	ChangeSkin("NextMemberBn","Skin15","Button down")
	ChangeSkin("NextMemberBn","Skin15","Button focus")

	CreateWnd(1,"PrevMemberBn","MembersWnd","",54,20,16,16,0)
	ChangeSkin("PrevMemberBn","Skin16","Button up")
	ChangeSkin("PrevMemberBn","Skin17","Button down")
	ChangeSkin("PrevMemberBn","Skin17","Button focus")

	CreateWnd(4,"upper_left_corner","MembersWnd","",-18,-17,128,128,0)
	ChangeSkin("upper_left_corner","Skin18","Label")

	CreateWnd(4,"upper_right_corner","MembersWnd","",490,-17,128,128,0)
	ChangeSkin("upper_right_corner","Skin19","Label")

	CreateWnd(4,"weapon_container","MembersWnd","",12,31,128,128,0)
	ChangeSkin("weapon_container","Skin20","Label")

	CreateWnd(4,"Abilities","MembersWnd","",359,206,256,256,0)
	ChangeSkin("Abilities","Skin1","Label")

	CreateWnd(4,"Armor","MembersWnd","",263,-8,128,256,0)
	ChangeSkin("Armor","Skin2","Label")

	CreateWnd(4,"belt","MembersWnd","",-38,235,256,128,0)
	ChangeSkin("belt","Skin4","Label")

	CreateWnd(4,"CurrentMemberNumberLabel","MembersWnd","",42,12,512,32,0)
	ChangeSkin("CurrentMemberNumberLabel","Skin5","Label")




--	CreateWnd(1,"MembersEquipBn","MembersWnd","Equip",20,156,64,20,0)
--	ChangeSkin("MembersEquipBn","Skin23","Button up")
--	ChangeSkin("MembersEquipBn","Skin24","Button down")
--	ChangeSkin("MembersEquipBn","Skin24","Button focus")

	CreateWnd(1,"MembersDropItemBn","MembersWnd","",20,156,64,32,0)
	ChangeSkin("MembersDropItemBn","Skin23","Button up")
	ChangeSkin("MembersDropItemBn","Skin24","Button down")
	ChangeSkin("MembersDropItemBn","Skin24","Button focus")

	CreateWnd(4,"MemberSelItemBorder","MembersWnd","",0,0,32,32,0)
	ChangeSkin("MemberSelItemBorder","Skin22","Label")


	CreateWnd(4,"LevelbarFront","MembersWnd","",20,220-25,50,20,0)
	ChangeSkin("LevelbarFront","Skin26","Label")

	CreateWnd(4,"LevelbarBK","MembersWnd","",20,220-25,136,20,0)
	ChangeSkin("LevelbarBK","Skin25","Label")



	CreateWnd(4,"HealthBarFront","MembersWnd","",20,220,50,20,0)
	ChangeSkin("HealthBarFront","Skin26","Label")

	CreateWnd(4,"HealthBarBK","MembersWnd","",20,220,136,20,0)
	ChangeSkin("HealthBarBK","Skin25","Label")











end

-- Mistlands GUI Resource file, END
