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
	Skin4 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=0, bkG=0,
	 bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin5 = { tex1="dm/final/action_bar.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/action_bar_a.bmp", tex2a="0",
	 tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin6 = { tex1="dm/final/action_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/action_button_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }
	Skin7 = { tex1="dm/final/character_bar_button.bmp", tex2="0", tex3="0", tex4="0",
	 tex1a="dm/final/character_bar_button_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }
	Skin8 = { tex1="dm/final/portrait.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin9 = { tex1="dm/final/item_bar.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/item_bar_a.bmp", tex2a="0",
	 tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin10 = { tex1="dm/final/item_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/item_button_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0
	 }
--	Skin11 = { tex1="dm/final/character_bar.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp",
--	 tex4="wndborder_corner.bmp", tex1a="dm/final/character_bar_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255,
--	 borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }

	Skin11 = { tex1="dm/final/character_bar.bmp", tex2="0", tex3="0",
	 tex4="0", tex1a="dm/final/character_bar_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255,
	 borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin12 = { tex1="dm/final/skill_bar.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/skill_bar_a.bmp", tex2a="0",
	 tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin13 = { tex1="dm/final/skill_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/skill_button_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0
	 }
	Skin14 = { tex1="dm/final/corner_char_blob.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp",
	 tex4="wndborder_corner.bmp", tex1a="dm/final/corner_char_blob_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255,
	 borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }

	Skin15 = { tex1="dm/final/corner_piece.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp",
	 tex4="wndborder_corner.bmp", tex1a="dm/final/corner_piece_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255,
	 borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin16 = { tex1="dm/final/quick_item.bmp", tex2="0", tex3="0",
	 tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255,
	 borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	
	Skin17 = { tex1="dm/final/weapon.bmp", tex2="0", tex3="0",
	 tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255,
	 borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin18 = { tex1="dm/final/healthbar.bmp", tex2="0", tex3="0",
	 tex4="0", tex1a="dm/final/healthbar_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255,
	 borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }

	Skin19 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }



	Skin20 = { tex1="dm/final/action_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/action_button_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=0, bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }

	Skin21 = { tex1="dm/final/skill_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/skill_button_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=0, bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0
	 }

	Skin22 = { tex1="dm/final/character_bar_button.bmp", tex2="0", tex3="0", tex4="0",
	 tex1a="dm/final/character_bar_button_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=0, bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }

	Skin23 = { tex1="dm/final/item_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/item_button_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=0, bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0
	 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"GamePlayScreen","","",0,0,800,600,0)
	ChangeSkin("GamePlayScreen","Skin4","Window")

	CreateWnd(0,"panel_char","GamePlayScreen","",-1,-10,128,512,0)
	ChangeSkin("panel_char","Skin11","Window")

	CreateWnd(0,"action_bar","GamePlayScreen","",42,512,256,64,0)
	ChangeSkin("action_bar","Skin5","Window")

	CreateWnd(1,"action_button","action_bar","",238,-5,32,64,0)
	ChangeSkin("action_button","Skin6","Button up")
	ChangeSkin("action_button","Skin20","Button down")
	ChangeSkin("action_button","Skin6","Button focus")

	CreateWnd(0,"item_bar","GamePlayScreen","",298,499,512,128,0)
	ChangeSkin("item_bar","Skin9","Window")

	CreateWnd(1,"item_button","item_bar","",85,-25,64,128,0)
	ChangeSkin("item_button","Skin10","Button up")
	ChangeSkin("item_button","Skin23","Button down")
	ChangeSkin("item_button","Skin10","Button focus")



	--
	--   quickitems..
	--
	--

	CreateWnd(1,"quickitem_1","item_bar","",145,30,32,32,0)
	ChangeSkin("quickitem_1","Skin16","Button up")
	ChangeSkin("quickitem_1","Skin2","Button down")
	ChangeSkin("quickitem_1","Skin3","Button focus")
	
	CreateWnd(1,"quickitem_2","item_bar","",181,30,32,32,0)
	ChangeSkin("quickitem_2","Skin16","Button up")
	ChangeSkin("quickitem_2","Skin2","Button down")
	ChangeSkin("quickitem_2","Skin3","Button focus")

	CreateWnd(1,"quickitem_3","item_bar","",145,66,32,32,0)
	ChangeSkin("quickitem_3","Skin16","Button up")
	ChangeSkin("quickitem_3","Skin2","Button down")
	ChangeSkin("quickitem_3","Skin3","Button focus")

	CreateWnd(1,"quickitem_4","item_bar","",181,66,32,32,0)
	ChangeSkin("quickitem_4","Skin16","Button up")
	ChangeSkin("quickitem_4","Skin2","Button down")
	ChangeSkin("quickitem_4","Skin3","Button focus")

	CreateWnd(1,"weapon","item_bar","",248,32,96,64,0)
	ChangeSkin("weapon","Skin17","Button up")
	ChangeSkin("weapon","Skin2","Button down")
	ChangeSkin("weapon","Skin3","Button focus")

	

	CreateWnd(0,"skill_bar","GamePlayScreen","",-51,552,512,64,0)
	ChangeSkin("skill_bar","Skin12","Window")

	CreateWnd(1,"skill_button","skill_bar","",400,-14,64,64,0)
	ChangeSkin("skill_button","Skin13","Button up")
	ChangeSkin("skill_button","Skin21","Button down")
	ChangeSkin("skill_button","Skin13","Button focus")


	CreateWnd(0,"wind_char","GamePlayScreen","",-1,346,256,256,0)
	ChangeSkin("wind_char","Skin14","Window")

	CreateWnd(0,"wind_map","GamePlayScreen","",546,346,256,256,0)
	ChangeSkin("wind_map","Skin15","Window")

	CreateWnd(1,"char_panel_button","panel_char","",0,11,128,64,0)
	ChangeSkin("char_panel_button","Skin7","Button up")
	ChangeSkin("char_panel_button","Skin22","Button down")
	ChangeSkin("char_panel_button","Skin7","Button focus")

	CreateWnd(1,"item_button_ext","wind_map","",77,127,64,128,0)
	ChangeSkin("item_button_ext","Skin10","Button up")
	ChangeSkin("item_button_ext","Skin23","Button down")
	ChangeSkin("item_button_ext","Skin10","Button focus")
	


	--
	--  Char window knapparna

	CreateWnd(1,"char_panel_button_ext","wind_char","",0,99,128,64,0)
	ChangeSkin("char_panel_button_ext","Skin7","Button up")
	ChangeSkin("char_panel_button_ext","Skin22","Button down")
	ChangeSkin("char_panel_button_ext","Skin7","Button focus")
	
	CreateWnd(1,"action_button_ext","wind_char","",135,160,32,64,0)
	ChangeSkin("action_button_ext","Skin6","Button up")
	ChangeSkin("action_button_ext","Skin20","Button down")
	ChangeSkin("action_button_ext","Skin6","Button focus")

	CreateWnd(1,"skill_button_ext","wind_char","",105,192,64,64,0)
	ChangeSkin("skill_button_ext","Skin13","Button up")
	ChangeSkin("skill_button_ext","Skin21","Button down")
	ChangeSkin("skill_button_ext","Skin13","Button focus")

	--
	--

	--  karaktärer..


	CreateWnd(1,"CharPortBn1","panel_char","",13,64,64,64,0)
	ChangeSkin("CharPortBn1","Skin8","Button up")
	ChangeSkin("CharPortBn1","Skin2","Button down")
	ChangeSkin("CharPortBn1","Skin3","Button focus")

	CreateWnd(4,"Char1LifeBk1","panel_char","",13,127,16,16,0)
	ChangeSkin("Char1LifeBk1","Skin18","Label")

	CreateWnd(1,"CharPortBn2","panel_char","",13,145,64,64,0)
	ChangeSkin("CharPortBn2","Skin8","Button up")
	ChangeSkin("CharPortBn2","Skin2","Button down")
	ChangeSkin("CharPortBn2","Skin3","Button focus")

	CreateWnd(4,"Char1LifeBk2","panel_char","",12,208,64,16,0)
	ChangeSkin("Char1LifeBk2","Skin18","Label")

	CreateWnd(1,"CharPortBn3","panel_char","",18,226,64,64,0)
	ChangeSkin("CharPortBn3","Skin8","Button up")
	ChangeSkin("CharPortBn3","Skin2","Button down")
	ChangeSkin("CharPortBn3","Skin3","Button focus")

	CreateWnd(4,"Char1LifeBk3","panel_char","",12,289,48,16,0)
	ChangeSkin("Char1LifeBk3","Skin18","Label")

	CreateWnd(1,"CharPortBn4","panel_char","",18,307,64,64,0)
	ChangeSkin("CharPortBn4","Skin8","Button up")
	ChangeSkin("CharPortBn4","Skin2","Button down")
	ChangeSkin("CharPortBn4","Skin3","Button focus")

	CreateWnd(4,"Char1LifeBk4","panel_char","",12,370,64,16,0)
	ChangeSkin("Char1LifeBk4","Skin18","Label")

	CreateWnd(1,"CharPortBn5","panel_char","",18,388,64,64,0)
	ChangeSkin("CharPortBn5","Skin8","Button up")
	ChangeSkin("CharPortBn5","Skin2","Button down")
	ChangeSkin("CharPortBn5","Skin3","Button focus")
	
	CreateWnd(4,"Char1LifeBk5","panel_char","",12,451,32,16,0)
	ChangeSkin("Char1LifeBk5","Skin18","Label")










	CreateWnd(1,"HQBn","wind_map","HQ",160,200-35,32,32,0)
	ChangeSkin("HQBn","Skin1","Button up")
	ChangeSkin("HQBn","Skin2","Button down")
	ChangeSkin("HQBn","Skin3","Button focus")


	CreateWnd(1,"GamPlayMenuBn","wind_map","Menu",160,200,68,20,0)
	ChangeSkin("GamPlayMenuBn","Skin1","Button up")
	ChangeSkin("GamPlayMenuBn","Skin2","Button down")
	ChangeSkin("GamPlayMenuBn","Skin3","Button focus")


	CreateWnd(1,"PauseBn","wind_map","(p)",160+35,200-35,32,32,0)
	ChangeSkin("PauseBn","Skin1","Button up")
	ChangeSkin("PauseBn","Skin2","Button down")
	ChangeSkin("PauseBn","Skin3","Button focus")




	CreateWnd(1,"ActiveCharacterPortraitBn","wind_char","",15,150,92,92,0)
	ChangeSkin("ActiveCharacterPortraitBn","Skin1","Button up")
	ChangeSkin("ActiveCharacterPortraitBn","Skin2","Button down")
	ChangeSkin("ActiveCharacterPortraitBn","Skin3","Button focus")





--	CreateWnd(0,"fps_wnd","","",2,2,50,20,0)
--	ChangeSkin("fps_wnd","Skin4","Window")

--	CreateWnd(4,"fps_label","fps_wnd","fps: 240",0,0,50,20,0)
--	ChangeSkin("fps_label","Skin19","Label")



end

-- Mistlands GUI Resource file, END
