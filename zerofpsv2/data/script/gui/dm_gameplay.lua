-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="dm/portrait3.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin3 = { tex1="black.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="cb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="dm/portrait1.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin6 = { tex1="dm/portrait1b.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin7 = { tex1="dm/portrait2.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="dm/portrait2b.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin9 = { tex1="dm/portrait3.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin10 = { tex1="dm/portrait3b.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin11 = { tex1="dm/portrait4.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin12 = { tex1="dm/portrait4b.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin13 = { tex1="dm/portrait5.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin14 = { tex1="dm/portrait5b.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin15 = { tex1="dm/bn1u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin16 = { tex1="dm/bn1d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin17 = { tex1="dm/bn1f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin18 = { tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=4,
	 tile=1, trans=0 }
	Skin19 = { tex1="dm/gameplay_info_wnd.bmp", tex2="dm/bd_horz.bmp", tex3="dm/bd_vert.bmp", tex4="dm/bd_corner.bmp",
	 tex1a="0", tex2a="dm/bd_horz_a.bmp", tex3a="dm/bd_vert_a.bmp", tex4a="dm/bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin20 = { tex1="dm/gameplay_panel.bmp", tex2="dm/bd_horz.bmp", tex3="dm/bd_vert.bmp", tex4="dm/bd_corner.bmp",
	 tex1a="0", tex2a="dm/bd_horz_a.bmp", tex3a="dm/bd_vert_a.bmp", tex4a="dm/bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin21 = { tex1="dm/weapon1.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin22 = { tex1="dm/rebriefing_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin23 = { tex1="dm/rebriefing_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin24 = { tex1="dm/rebriefing_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin25 = { tex1="dm/pause_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin26 = { tex1="dm/pause_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin27 = { tex1="dm/pause_bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin28 = { tex1="dm/menu_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin29 = { tex1="dm/menu_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin30 = { tex1="dm/menu_bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin31 = { tex1="dm/attack_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin32 = { tex1="dm/attack_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------

	CreateWnd(0,"GamePlayScreen","","",0,0,800,600,0)
	ChangeSkin("GamePlayScreen","Skin2","Window")

	CreateWnd(0,"GamePlayPanelWnd","GamePlayScreen","",7,504,789,92,0)
	ChangeSkin("GamePlayPanelWnd","Skin20","Window")

	CreateWnd(1,"GamePlaySkillBn","GamePlayPanelWnd","Skill",400,18,60,53,0)
	ChangeSkin("GamePlaySkillBn","Skin15","Button up")
	ChangeSkin("GamePlaySkillBn","Skin16","Button down")
	ChangeSkin("GamePlaySkillBn","Skin17","Button focus")

	CreateWnd(1,"GamePlayStopBn","GamePlayPanelWnd","Stop",333,18,60,53,0)
	ChangeSkin("GamePlayStopBn","Skin15","Button up")
	ChangeSkin("GamePlayStopBn","Skin16","Button down")
	ChangeSkin("GamePlayStopBn","Skin17","Button focus")

	CreateWnd(1,"GameplayMoveBn","GamePlayPanelWnd","Move",129,15,60,53,0)
	ChangeSkin("GameplayMoveBn","Skin15","Button up")
	ChangeSkin("GameplayMoveBn","Skin16","Button down")
	ChangeSkin("GameplayMoveBn","Skin17","Button focus")

	CreateWnd(1,"PauseBn","GamePlayInfoWnd","",8,461,32,28,0)
	ChangeSkin("PauseBn","Skin25","Button up")
	ChangeSkin("PauseBn","Skin26","Button down")
	ChangeSkin("PauseBn","Skin27","Button focus")

	CreateWnd(1,"QuickItem1","GamePlayPanelWnd","",479,22,32,32,0)
	ChangeSkin("QuickItem1","Skin15","Button up")
	ChangeSkin("QuickItem1","Skin16","Button down")
	ChangeSkin("QuickItem1","Skin17","Button focus")

	CreateWnd(1,"QuickItem2","GamePlayPanelWnd","",511,22,32,32,0)
	ChangeSkin("QuickItem2","Skin15","Button up")
	ChangeSkin("QuickItem2","Skin16","Button down")
	ChangeSkin("QuickItem2","Skin17","Button focus")

	CreateWnd(1,"QuickItem3","GamePlayPanelWnd","",543,22,32,32,0)
	ChangeSkin("QuickItem3","Skin15","Button up")
	ChangeSkin("QuickItem3","Skin16","Button down")
	ChangeSkin("QuickItem3","Skin17","Button focus")

	CreateWnd(1,"QuickItem4","GamePlayPanelWnd","",575,22,32,32,0)
	ChangeSkin("QuickItem4","Skin15","Button up")
	ChangeSkin("QuickItem4","Skin16","Button down")
	ChangeSkin("QuickItem4","Skin17","Button focus")

	CreateWnd(1,"RebriefingBn","GamePlayPanelWnd","",25,10,51,37,0)
	ChangeSkin("RebriefingBn","Skin22","Button up")
	ChangeSkin("RebriefingBn","Skin23","Button down")
	ChangeSkin("RebriefingBn","Skin24","Button focus")

	CreateWnd(4,"TimeLeftLabel","GamePlayInfoWnd","12:24",50,465,50,20,0)
	ChangeSkin("TimeLeftLabel","Skin2","Label")

	CreateWnd(4,"WeaponIcon","GamePlayPanelWnd","",617,9,64,75,0)
	ChangeSkin("WeaponIcon","Skin21","Label")

	CreateWnd(4,"ActiveCharacterPortrait","GamePlayPanelWnd","",698,0,92,92,0)
	ChangeSkin("ActiveCharacterPortrait","Skin1","Label")

	CreateWnd(4,"AmmoLabel","GamePlayPanelWnd","",647,12,30,20,0)
	ChangeSkin("AmmoLabel","Skin2","Label")

	CreateWnd(4,"Char1LifeFront1","GamePlayScreen","",8,72,32,8,0)
	ChangeSkin("Char1LifeFront1","Skin4","Label")
	CreateWnd(4,"Char1LifeBk1","GamePlayScreen","",8,72,64,8,0)
	ChangeSkin("Char1LifeBk1","Skin3","Label")

	CreateWnd(4,"Char1LifeFront2","GamePlayScreen","",8,155,32,8,0)
	ChangeSkin("Char1LifeFront2","Skin4","Label")
	CreateWnd(4,"Char1LifeBk2","GamePlayScreen","",8,155,64,8,0)
	ChangeSkin("Char1LifeBk2","Skin3","Label")

	CreateWnd(4,"Char1LifeFront3","GamePlayScreen","",8,238,32,8,0)
	ChangeSkin("Char1LifeFront3","Skin4","Label")
	CreateWnd(4,"Char1LifeBk3","GamePlayScreen","",8,238,64,8,0)
	ChangeSkin("Char1LifeBk3","Skin3","Label")

	CreateWnd(4,"Char1LifeFront4","GamePlayScreen","",8,321,32,8,0)
	ChangeSkin("Char1LifeFront4","Skin4","Label")
	CreateWnd(4,"Char1LifeBk4","GamePlayScreen","",8,321,64,8,0)
	ChangeSkin("Char1LifeBk4","Skin3","Label")

	CreateWnd(4,"Char1LifeFront5","GamePlayScreen","",8,403,32,8,0)
	ChangeSkin("Char1LifeFront5","Skin4","Label")
	CreateWnd(4,"Char1LifeBk5","GamePlayScreen","",8,403,64,8,0)
	ChangeSkin("Char1LifeBk5","Skin3","Label")

	CreateWnd(1,"CharPortBn1","GamePlayScreen","",8,8,64,64,0)
	ChangeSkin("CharPortBn1","Skin5","Button up")
	ChangeSkin("CharPortBn1","Skin6","Button down")
	ChangeSkin("CharPortBn1","Skin5","Button focus")

	CreateWnd(1,"CharPortBn2","GamePlayScreen","",8,91,64,64,0)
	ChangeSkin("CharPortBn2","Skin7","Button up")
	ChangeSkin("CharPortBn2","Skin8","Button down")
	ChangeSkin("CharPortBn2","Skin7","Button focus")

	CreateWnd(1,"CharPortBn3","GamePlayScreen","",8,174,64,64,0)
	ChangeSkin("CharPortBn3","Skin9","Button up")
	ChangeSkin("CharPortBn3","Skin10","Button down")
	ChangeSkin("CharPortBn3","Skin9","Button focus")

	CreateWnd(1,"CharPortBn4","GamePlayScreen","",8,257,64,64,0)
	ChangeSkin("CharPortBn4","Skin11","Button up")
	ChangeSkin("CharPortBn4","Skin12","Button down")
	ChangeSkin("CharPortBn4","Skin11","Button focus")

	CreateWnd(1,"CharPortBn5","GamePlayScreen","",8,339,64,64,0)
	ChangeSkin("CharPortBn5","Skin13","Button up")
	ChangeSkin("CharPortBn5","Skin14","Button down")
	ChangeSkin("CharPortBn5","Skin13","Button focus")

	CreateWnd(1,"GamPlayMenuBn","GamePlayPanelWnd","Menu",20,61,64,20,0)
	ChangeSkin("GamPlayMenuBn","Skin28","Button up")
	ChangeSkin("GamPlayMenuBn","Skin29","Button down")
	ChangeSkin("GamPlayMenuBn","Skin30","Button focus")

	CreateWnd(1,"GamePlayAttackBn","GamePlayPanelWnd","",197,16,60,53,0) -- attack button
	ChangeSkin("GamePlayAttackBn","Skin31","Button up")
	ChangeSkin("GamePlayAttackBn","Skin32","Button down")
	ChangeSkin("GamePlayAttackBn","Skin31","Button focus")

	CreateWnd(1,"GamePlayCrunchBn","GamePlayPanelWnd","Crunch",265,17,60,53,0)
	ChangeSkin("GamePlayCrunchBn","Skin15","Button up")
	ChangeSkin("GamePlayCrunchBn","Skin16","Button down")
	ChangeSkin("GamePlayCrunchBn","Skin17","Button focus")

end

-- Mistlands GUI Resource file, END
