-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
--	Skin1 = { tex1="bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
--	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
--	Skin2 = { tex1="bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
--	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
--	Skin3 = { tex1="bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
--	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin1 = { tex1="dm/misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="dm/misc_button_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

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

	Skin25 = { tex1="dm/label_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/label_bk_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0
	 }

	Skin26 = { tex1="dm/final/win_tile.bmp", 
	 tex2="dm/final/window/win_edge_ver.bmp", 
	 tex3="dm/final/window/win_edge_hor.bmp", 
	 tex4="dm/final/window/dm_win_corn.bmp", tex1a="0", 
	 tex2a="dm/final/window/win_edge_ver_a.bmp", 
	 tex3a="dm/final/window/win_edge_hor_a.bmp", 
	 tex4a="dm/final/window/dm_win_corn_a.bmp", bkR=255, bkG=255, bkB=255,
	 borderR=255, borderG=255, borderB=255, bd_size=32, tile=1, trans=0 }

	Skin27 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=92, bkG=92,
	 bkB=92, borderR=56, borderG=56, borderB=56, bd_size=1, tile=1, trans=0 }

	Skin28 = { tex1="dm/active_agent_border.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/active_agent_border_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=0, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

	Skin29 = { tex1="dm/pause_u.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/miscbn_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

	Skin30 = { tex1="dm/pause_d.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/miscbn_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

	Skin31 = { tex1="dm/map.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/map_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

	Skin32 = { tex1="dm/map_icons/agent_map_icon.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/map_icons/agent_map_icon_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

	Skin33 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=128, bkG=128,
	 bkB=255, borderR=56, borderG=56, borderB=56, bd_size=2, tile=1, trans=0 }

	Skin34 = { tex1="sb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin35 = { tex1="sb_n.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin36 = { tex1="sb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }



	Skin37 = { tex1="dm/menubn_u.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/miscbn_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

	Skin38 = { tex1="dm/menubn_d.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/miscbn_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }


	Skin39 = { tex1="dm/helpbn_u.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/miscbn_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

	Skin40 = { tex1="dm/helpbn_d.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/miscbn_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }


	Skin41 = { tex1="dm/mapbn_u.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/miscbn_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

	Skin42 = { tex1="dm/mapbn_d.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/miscbn_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }



	Skin43 = { tex1="dm/hqbn_u.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/hqbn_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

	Skin44 = { tex1="dm/hqbn_d.bmp", tex2="0", tex3="0", tex4="0", 
	 tex1a="dm/hqbn_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }


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

	CreateWnd(4,"GamePlayInfoLabel","GamePlayScreen","Return to base! Mission done.",150,470,400,22,0)
	ChangeSkin("GamePlayInfoLabel","Skin25","Label")



	--
	--   quickitems..
	--
	--

	CreateWnd(1,"GamePlayDlgQuickItem1","item_bar","",145,30,32,32,0)
	ChangeSkin("GamePlayDlgQuickItem1","Skin16","Button up")
	ChangeSkin("GamePlayDlgQuickItem1","Skin2","Button down")
	ChangeSkin("GamePlayDlgQuickItem1","Skin3","Button focus")
	
	CreateWnd(1,"GamePlayDlgQuickItem2","item_bar","",181,30,32,32,0)
	ChangeSkin("GamePlayDlgQuickItem2","Skin16","Button up")
	ChangeSkin("GamePlayDlgQuickItem2","Skin2","Button down")
	ChangeSkin("GamePlayDlgQuickItem2","Skin3","Button focus")

	CreateWnd(1,"GamePlayDlgQuickItem3","item_bar","",145,66,32,32,0)
	ChangeSkin("GamePlayDlgQuickItem3","Skin16","Button up")
	ChangeSkin("GamePlayDlgQuickItem3","Skin2","Button down")
	ChangeSkin("GamePlayDlgQuickItem3","Skin3","Button focus")

	CreateWnd(1,"GamePlayDlgQuickItem4","item_bar","",181,66,32,32,0)
	ChangeSkin("GamePlayDlgQuickItem4","Skin16","Button up")
	ChangeSkin("GamePlayDlgQuickItem4","Skin2","Button down")
	ChangeSkin("GamePlayDlgQuickItem4","Skin3","Button focus")

	CreateWnd(1,"GamePlayDlgWeaponBn","item_bar","",248,32,96,64,0)
	ChangeSkin("GamePlayDlgWeaponBn","Skin17","Button up")
	ChangeSkin("GamePlayDlgWeaponBn","Skin2","Button down")
	ChangeSkin("GamePlayDlgWeaponBn","Skin3","Button focus")

	

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
	-- Misc buttons
	--

	CreateWnd(1,"GamPlayMenuBn","wind_map","",133,183,24,24,0)
	ChangeSkin("GamPlayMenuBn","Skin37","Button up")
	ChangeSkin("GamPlayMenuBn","Skin38","Button down")
	ChangeSkin("GamPlayMenuBn","Skin37","Button focus")

	CreateWnd(1,"HelpBn","wind_map","",140,160,24,24,0)
	ChangeSkin("HelpBn","Skin39","Button up")
	ChangeSkin("HelpBn","Skin40","Button down")
	ChangeSkin("HelpBn","Skin39","Button focus")

	CreateWnd(1,"MapBn","wind_map","",157,142,24,24,0)
	ChangeSkin("MapBn","Skin41","Button up")
	ChangeSkin("MapBn","Skin42","Button down")
	ChangeSkin("MapBn","Skin41","Button focus")

	CreateWnd(1,"PauseBn","wind_map","",180,133,24,24,0)
	ChangeSkin("PauseBn","Skin29","Button up")
	ChangeSkin("PauseBn","Skin30","Button down")
	ChangeSkin("PauseBn","Skin29","Button focus")

	CreateWnd(1,"HQBn","wind_map","",160,161,92,92,0)
	ChangeSkin("HQBn","Skin43","Button up")
	ChangeSkin("HQBn","Skin44","Button down")
	ChangeSkin("HQBn","Skin43","Button focus")

	--
	--

	--  karakt�rer..


	CreateWnd(1,"CharPortBn1","panel_char","",12,64-4,64,64,0)
	ChangeSkin("CharPortBn1","Skin8","Button up")
	ChangeSkin("CharPortBn1","Skin2","Button down")
	ChangeSkin("CharPortBn1","Skin3","Button focus")

	CreateWnd(4,"Char1LifeBk1","panel_char","",13,127-3,16,16,0)
	ChangeSkin("Char1LifeBk1","Skin18","Label")

	CreateWnd(1,"CharPortBn2","panel_char","",12,145-4,64,64,0)
	ChangeSkin("CharPortBn2","Skin8","Button up")
	ChangeSkin("CharPortBn2","Skin2","Button down")
	ChangeSkin("CharPortBn2","Skin3","Button focus")

	CreateWnd(4,"Char1LifeBk2","panel_char","",12,208-3,64,16,0)
	ChangeSkin("Char1LifeBk2","Skin18","Label")

	CreateWnd(1,"CharPortBn3","panel_char","",12,226-4,64,64,0)
	ChangeSkin("CharPortBn3","Skin8","Button up")
	ChangeSkin("CharPortBn3","Skin2","Button down")
	ChangeSkin("CharPortBn3","Skin3","Button focus")

	CreateWnd(4,"Char1LifeBk3","panel_char","",12,289-3,48,16,0)
	ChangeSkin("Char1LifeBk3","Skin18","Label")

	CreateWnd(1,"CharPortBn4","panel_char","",12,307-4,64,64,0)
	ChangeSkin("CharPortBn4","Skin8","Button up")
	ChangeSkin("CharPortBn4","Skin2","Button down")
	ChangeSkin("CharPortBn4","Skin3","Button focus")

	CreateWnd(4,"Char1LifeBk4","panel_char","",12,370-3,64,16,0)
	ChangeSkin("Char1LifeBk4","Skin18","Label")

	CreateWnd(1,"CharPortBn5","panel_char","",12,388-4,64,64,0)
	ChangeSkin("CharPortBn5","Skin8","Button up")
	ChangeSkin("CharPortBn5","Skin2","Button down")
	ChangeSkin("CharPortBn5","Skin3","Button focus")
	
	CreateWnd(4,"Char1LifeBk5","panel_char","",12,451-3,32,16,0)
	ChangeSkin("Char1LifeBk5","Skin18","Label")








--	CreateWnd(1,"HelpBn","wind_map","?",160+16,200-35-35,24,24,0)
--	ChangeSkin("HelpBn","Skin1","Button up")
--	ChangeSkin("HelpBn","Skin2","Button down")
--	ChangeSkin("HelpBn","Skin1","Button focus")

--	CreateWnd(1,"HQBn","wind_map","HQ",160,200-35,32,32,0)
--	ChangeSkin("HQBn","Skin1","Button up")
--	ChangeSkin("HQBn","Skin2","Button down")
--	ChangeSkin("HQBn","Skin1","Button focus")

--	CreateWnd(1,"PauseBn","wind_map","",160+35,200-35,32,32,0)
--	ChangeSkin("PauseBn","Skin29","Button up")
--	ChangeSkin("PauseBn","Skin30","Button down")
--	ChangeSkin("PauseBn","Skin29","Button focus")

--	CreateWnd(1,"MapBn","wind_map","Map",160,200,68,20,0)
--	ChangeSkin("MapBn","Skin1","Button up")
--	ChangeSkin("MapBn","Skin2","Button down")
--	ChangeSkin("MapBn","Skin1","Button focus")

--	CreateWnd(1,"GamPlayMenuBn","wind_map","Menu",160,200+24,68,20,0)
--	ChangeSkin("GamPlayMenuBn","Skin1","Button up")
--	ChangeSkin("GamPlayMenuBn","Skin2","Button down")
--	ChangeSkin("GamPlayMenuBn","Skin1","Button focus")



	CreateWnd(1,"ActiveCharacterPortraitBn","wind_char","",-4+8+4,128+24,96,96,0)
	ChangeSkin("ActiveCharacterPortraitBn","Skin1","Button up")
	ChangeSkin("ActiveCharacterPortraitBn","Skin2","Button down")
	ChangeSkin("ActiveCharacterPortraitBn","Skin3","Button focus")

	CreateWnd(4,"ActiveCharacterPortraitLabel","wind_char","",-4+8+4,128+24,96,96,0)
	ChangeSkin("ActiveCharacterPortraitLabel","Skin28","Label")




	CreateWnd(0,"MissionInfoWnd","GamePlayScreen","",800/2-256/2,600/2-256/2,256,256,0)
	ChangeSkin("MissionInfoWnd","Skin26","Window")

	CreateWnd(10,"MissionInfoLabel","MissionInfoWnd","",10,10,235,190,0)
	ChangeSkin("MissionInfoLabel","Skin27","Textbox")

	CreateWnd(1,"MissionInfoOK","MissionInfoWnd","OK",256/2-96/2,256-40,96,40,0)
	ChangeSkin("MissionInfoOK","Skin1","Button up")
	ChangeSkin("MissionInfoOK","Skin2","Button down")
	ChangeSkin("MissionInfoOK","Skin1","Button focus")


	CreateWnd(0,"MapWnd","GamePlayScreen","",200,0,600,448,0)
	ChangeSkin("MapWnd","Skin31","Window")

	
	CreateWnd(4,"AgentMapIcon1","MapWnd","",0,0,16,16,0)
	ChangeSkin("AgentMapIcon1","Skin32","Label")

	CreateWnd(4,"AgentMapIcon2","MapWnd","",0,0,16,16,0)
	ChangeSkin("AgentMapIcon2","Skin32","Label")

	CreateWnd(4,"AgentMapIcon3","MapWnd","",0,0,16,16,0)
	ChangeSkin("AgentMapIcon3","Skin32","Label")

	CreateWnd(4,"AgentMapIcon4","MapWnd","",0,0,16,16,0)
	ChangeSkin("AgentMapIcon4","Skin32","Label")

	CreateWnd(4,"AgentMapIcon5","MapWnd","",0,0,16,16,0)
	ChangeSkin("AgentMapIcon5","Skin32","Label")


	CreateWnd(0,"HelpWnd","GamePlayScreen","",150,32,512,380,0)
	ChangeSkin("HelpWnd","Skin26","Window")

	CreateWnd(10,"HelpWnd","GamePlayScreen","",150,32,512,380,0)
	ChangeSkin("HelpWnd","Skin26","Window")

	CreateWnd(10,"HelpTextEb","HelpWnd","",0,0,512-16,380-64,0)
	ChangeSkin("HelpTextEb","Skin33","Textbox")

	CreateWnd(1,"HelpTextOK","HelpWnd","OK",512/2-96/2,380-40,96,40,0)
	ChangeSkin("HelpTextOK","Skin1","Button up")
	ChangeSkin("HelpTextOK","Skin2","Button down")
	ChangeSkin("HelpTextOK","Skin1","Button focus")


	HelpText = [[The Minimoog Voyager is a true analog monophonic synthesizer (probably the most expensive one of its kind) and is 
		based entirely on the original classic Minimoog Model D, which was produced by Moog Music, Inc. from 1970 to 1982. It 
		introduces many new features, including a three dimensional touch pad, MIDI I/O, extensive patching facilities, patch storage, 
		and more! It's cased in a beautiful, solid hardwood cabinet with a multi-position hinged control panel.
		Just like the original, the Voyager has three analog wide-range voltage controlled oscillators, one noise source, as well as 
		one audio preamplifier for externally-applied audio signals.\n\nTwo resonant Moog-style filters are on-board and function in dual 
		lowpass or lowpass/highpass modes and can be modulated by one of the two ADSR envelopes, the LFO, or external control. All 
		knobs and switches can have their settings stored into one of 128 Program memories. Program Banks as well as the Voyager's 
		Operating System can be uploaded/downloaded via MIDI SysEx. Centered on its front panel is the ominous, black 3-D (XYZ) Touch 
		Surface which provides three continuously-variable control signals which are derived from the up-down and left-right position 
		of the player's finger, and the amount of area with which the player makes contact to the surface.\n\n
		There's also a 44-note keyboard, this time with velocity and aftertouch sensitivity. Also on-board are Pitch and Mod wheels, 
		Glide/Portamento, and a series of 1/4" CV and/or Pedal jacks lining the top/rear of the front panel. The Mod Wheel can be sent 
		to control all oscillators, or just VCO2, VCO3, the filter cutoff, or LFO rate. The Mod can gets its source from the LFO 
		(triangle/square/sample-n-hold waveforms), VCO3, or the external Mod2 input. A Mixer Module gives you a five-input mixer for 
		combining various audio sources prior to filtering between the oscillators, noise and external audio input signals.\n\n
		The new Minimoog Voyager is a first class synth; every bit as elegant in look as sound and quality. It is highly streamlined 
		and offers all the functionality of a modern day (mono)synth with the classic, warm, unforgettable Moog sound. It WILL replace 
		your original Minimoog (if you're lucky enough to have one). 
		Signature models will become prized possessions for decades to come, and the Perfo]]


	SetTextString("HelpTextEb", HelpText)
	
end

-- Mistlands GUI Resource file, END
