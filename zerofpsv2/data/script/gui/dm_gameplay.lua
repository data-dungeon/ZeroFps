-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="backpack_down.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin3 = { tex1="bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin6 = { tex1="black.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin7 = { tex1="cb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=4,
	 tile=1, trans=0 }
	Skin9 = { tex1="cb_d.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp", tex1a="0",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=4, tile=1,
	 trans=0 }
	Skin10 = { tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	Skin11 = { tex1="cb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"GamePlayChar1Wnd","","",8,8,64,73,0)
	ChangeSkin("GamePlayChar1Wnd","Skin8","Window")

	CreateWnd(0,"GamePlayChar2Wnd","","",8,91,64,73,0)
	ChangeSkin("GamePlayChar2Wnd","Skin8","Window")

	CreateWnd(0,"GamePlayChar3Wnd","","",8,174,64,73,0)
	ChangeSkin("GamePlayChar3Wnd","Skin8","Window")

	CreateWnd(0,"GamePlayChar4Wnd","","",8,257,64,73,0)
	ChangeSkin("GamePlayChar4Wnd","Skin8","Window")

	CreateWnd(0,"GamePlayChar5Wnd","","",8,339,64,73,0)
	ChangeSkin("GamePlayChar5Wnd","Skin8","Window")

	CreateWnd(0,"GamePlayPanelWnd","","",7,504,789,92,0)
	ChangeSkin("GamePlayPanelWnd","Skin9","Window")

	CreateWnd(1,"GamePlaySkillBn","GamePlayPanelWnd","Skill",400,18,60,53,0)
	ChangeSkin("GamePlaySkillBn","Skin3","Button up")
	ChangeSkin("GamePlaySkillBn","Skin4","Button down")
	ChangeSkin("GamePlaySkillBn","Skin5","Button focus")

	CreateWnd(1,"GamePlayStopBn","GamePlayPanelWnd","Stop",333,18,60,53,0)
	ChangeSkin("GamePlayStopBn","Skin3","Button up")
	ChangeSkin("GamePlayStopBn","Skin4","Button down")
	ChangeSkin("GamePlayStopBn","Skin5","Button focus")

	CreateWnd(1,"GameplayMoveBn","GamePlayPanelWnd","Move",129,15,60,53,0)
	ChangeSkin("GameplayMoveBn","Skin3","Button up")
	ChangeSkin("GameplayMoveBn","Skin4","Button down")
	ChangeSkin("GameplayMoveBn","Skin5","Button focus")

	CreateWnd(0,"GuiMainWnd","","",0,0,800,600,0)
	ChangeSkin("GuiMainWnd","Skin2","Window")

	CreateWnd(4,"Label358","","",24,104,50,9,0)
	ChangeSkin("Label358","Skin6","Label")

	CreateWnd(4,"Label464","","",41,110,50,9,0)
	ChangeSkin("Label464","Skin6","Label")

	CreateWnd(4,"Label962","","",27,102,50,9,0)
	ChangeSkin("Label962","Skin6","Label")

	CreateWnd(0,"GamePlayInfoWnd","","",13,461,104,28,0)
	ChangeSkin("Nonmame","Skin10","Window")

	CreateWnd(1,"QuickItem1","GamePlayPanelWnd","",479,22,32,32,0)
	ChangeSkin("QuickItem1","Skin3","Button up")
	ChangeSkin("QuickItem1","Skin4","Button down")
	ChangeSkin("QuickItem1","Skin5","Button focus")

	CreateWnd(1,"QuickItem2","GamePlayPanelWnd","",511,22,32,32,0)
	ChangeSkin("QuickItem2","Skin3","Button up")
	ChangeSkin("QuickItem2","Skin4","Button down")
	ChangeSkin("QuickItem2","Skin5","Button focus")

	CreateWnd(1,"QuickItem3","GamePlayPanelWnd","",543,22,32,32,0)
	ChangeSkin("QuickItem3","Skin3","Button up")
	ChangeSkin("QuickItem3","Skin4","Button down")
	ChangeSkin("QuickItem3","Skin5","Button focus")

	CreateWnd(1,"QuickItem4","GamePlayPanelWnd","",575,22,32,32,0)
	ChangeSkin("QuickItem4","Skin3","Button up")
	ChangeSkin("QuickItem4","Skin4","Button down")
	ChangeSkin("QuickItem4","Skin5","Button focus")

	CreateWnd(1,"RebriefingBn","GamePlayPanelWnd","",25,10,51,37,0)
	ChangeSkin("RebriefingBn","Skin3","Button up")
	ChangeSkin("RebriefingBn","Skin4","Button down")
	ChangeSkin("RebriefingBn","Skin5","Button focus")

	CreateWnd(4,"WeaponIcon","GamePlayPanelWnd","",617,9,64,75,0)
	ChangeSkin("WeaponIcon","Skin11","Label")

	CreateWnd(4,"ActiveCharacterPortrait","GamePlayPanelWnd","",692,0,92,92,0)
	ChangeSkin("ActiveCharacterPortrait","Skin1","Label")

	CreateWnd(4,"AmmoLabel","GamePlayPanelWnd","",647,12,30,20,0)
	ChangeSkin("AmmoLabel","Skin2","Label")

	CreateWnd(1,"PauseBn","GamePlayInfoWnd","",4,3,29,23,0)
	ChangeSkin("Button334","Skin3","Button up")
	ChangeSkin("Button334","Skin4","Button down")
	ChangeSkin("Button334","Skin5","Button focus")

	CreateWnd(4,"Char1LifeBk1","GamePlayChar1Wnd","",0,64,50,9,0)
	ChangeSkin("Char1LifeBk1","Skin6","Label")

	CreateWnd(4,"Char1LifeBk2","GamePlayChar2Wnd","",0,64,50,9,0)
	ChangeSkin("Char1LifeBk2","Skin6","Label")

	CreateWnd(4,"Char1LifeBk3","GamePlayChar3Wnd","",0,64,50,9,0)
	ChangeSkin("Char1LifeBk3","Skin6","Label")

	CreateWnd(4,"Char1LifeBk4","GamePlayChar4Wnd","",0,64,50,9,0)
	ChangeSkin("Char1LifeBk4","Skin6","Label")

	CreateWnd(4,"Char1LifeBk5","GamePlayChar5Wnd","",0,64,50,9,0)
	ChangeSkin("Char1LifeBk5","Skin6","Label")

	CreateWnd(4,"Char1LifeFront1","GamePlayChar1Wnd","",13,64,50,8,0)
	ChangeSkin("Char1LifeFront1","Skin7","Label")

	CreateWnd(4,"Char1LifeFront2","GamePlayChar2Wnd","",13,64,50,8,0)
	ChangeSkin("Char1LifeFront2","Skin7","Label")

	CreateWnd(4,"Char1LifeFront3","GamePlayChar3Wnd","",13,64,50,8,0)
	ChangeSkin("Char1LifeFront3","Skin7","Label")

	CreateWnd(4,"Char1LifeFront4","GamePlayChar4Wnd","",13,64,50,8,0)
	ChangeSkin("Char1LifeFront4","Skin7","Label")

	CreateWnd(4,"Char1LifeFront5","GamePlayChar5Wnd","",13,64,50,8,0)
	ChangeSkin("Char1LifeFront5","Skin7","Label")

	CreateWnd(1,"CharPortBn1","GamePlayChar1Wnd","",0,0,64,64,0)
	ChangeSkin("CharPortBn1","Skin3","Button up")
	ChangeSkin("CharPortBn1","Skin4","Button down")
	ChangeSkin("CharPortBn1","Skin5","Button focus")

	CreateWnd(1,"CharPortBn2","GamePlayChar2Wnd","",0,0,64,64,0)
	ChangeSkin("CharPortBn2","Skin3","Button up")
	ChangeSkin("CharPortBn2","Skin4","Button down")
	ChangeSkin("CharPortBn2","Skin5","Button focus")

	CreateWnd(1,"CharPortBn3","GamePlayChar3Wnd","",0,0,64,64,0)
	ChangeSkin("CharPortBn3","Skin3","Button up")
	ChangeSkin("CharPortBn3","Skin4","Button down")
	ChangeSkin("CharPortBn3","Skin5","Button focus")

	CreateWnd(1,"CharPortBn4","GamePlayChar4Wnd","",0,0,64,64,0)
	ChangeSkin("CharPortBn4","Skin3","Button up")
	ChangeSkin("CharPortBn4","Skin4","Button down")
	ChangeSkin("CharPortBn4","Skin5","Button focus")

	CreateWnd(1,"CharPortBn5","GamePlayChar5Wnd","",0,0,64,64,0)
	ChangeSkin("CharPortBn5","Skin3","Button up")
	ChangeSkin("CharPortBn5","Skin4","Button down")
	ChangeSkin("CharPortBn5","Skin5","Button focus")

	CreateWnd(1,"GamPlayMenuBn","GamePlayPanelWnd","Menu",20,61,64,20,0)
	ChangeSkin("GamPlayMenuBn","Skin3","Button up")
	ChangeSkin("GamPlayMenuBn","Skin4","Button down")
	ChangeSkin("GamPlayMenuBn","Skin5","Button focus")

	CreateWnd(1,"GamePlayAttackBn","GamePlayPanelWnd","Attack",197,16,60,53,0)
	ChangeSkin("GamePlayAttackBn","Skin3","Button up")
	ChangeSkin("GamePlayAttackBn","Skin4","Button down")
	ChangeSkin("GamePlayAttackBn","Skin5","Button focus")

	CreateWnd(1,"GamePlayCrunchBn","GamePlayPanelWnd","­Crunch",265,17,60,53,0)
	ChangeSkin("GamePlayCrunchBn","Skin3","Button up")
	ChangeSkin("GamePlayCrunchBn","Skin4","Button down")
	ChangeSkin("GamePlayCrunchBn","Skin5","Button focus")

	CreateWnd(4,"TimeLeftLabel","GamePlayInfoWnd","12:24",42,4,50,20,0)
	ChangeSkin("Label724","Skin2","Label")

end

-- Mistlands GUI Resource file, END
