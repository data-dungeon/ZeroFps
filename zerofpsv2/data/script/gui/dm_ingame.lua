-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="bn1u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="bn1d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin3 = { tex1="bn1f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin5 = { tex1="label_bk.bmp", tex2="bd_horz.bmp", tex3="bd_vert.bmp", tex4="bd_corner.bmp",
	 tex1a="0", tex2a="bd_horz_a.bmp", tex3a="bd_vert_a.bmp", tex4a="bd_corner_a.bmp", bkR=255, bkG=255, bkB=255, 
	 borderR=255, borderG=255, borderB=255, bd_size=8, tile=0, trans=0 }
	Skin6 = { tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"DMInGameScreen","","",0,0,800,600,0)
	ChangeSkin("DMInGameScreen","Skin4","Window")

	CreateWnd(4,"MoneyLabel","DMInGameScreen","$ 42",20,20,50,20,0)
	ChangeSkin("MoneyLabel","Skin5","Label")

	CreateWnd(0,"InGamePanelWnd","","",8,534,786,64,0)
	ChangeSkin("InGamePanelWnd","Skin5","Window")

	CreateWnd(1,"BribeBn","InGamePanelWnd","Bribe",234,17,66,37,0)
	ChangeSkin("BribeBn","Skin1","Button up")
	ChangeSkin("BribeBn","Skin2","Button down")
	ChangeSkin("BribeBn","Skin3","Button focus")

	CreateWnd(1,"HQBn","InGamePanelWnd","HQ",308,17,66,37,0)
	ChangeSkin("HQBn","Skin1","Button up")
	ChangeSkin("HQBn","Skin2","Button down")
	ChangeSkin("HQBn","Skin3","Button focus")

	CreateWnd(1,"MissionsBn","InGamePanelWnd","Missions",8,15,66,37,0)
	ChangeSkin("MissionsBn","Skin1","Button up")
	ChangeSkin("MissionsBn","Skin2","Button down")
	ChangeSkin("MissionsBn","Skin3","Button focus")

	CreateWnd(1,"ShopBn","InGamePanelWnd","Shop",158,16,66,37,0)
	ChangeSkin("ShopBn","Skin1","Button up")
	ChangeSkin("ShopBn","Skin2","Button down")
	ChangeSkin("ShopBn","Skin3","Button focus")

	CreateWnd(1,"MembersBn","InGamePanelWnd","Members",84,15,66,37,0)
	ChangeSkin("MembersBn","Skin1","Button up")
	ChangeSkin("MembersBn","Skin2","Button down")
	ChangeSkin("MembersBn","Skin3","Button focus")

	CreateWnd(1,"MenuBn","InGamePanelWnd","Menu",713,14,66,37,0)
	ChangeSkin("MenuBn","Skin1","Button up")
	ChangeSkin("MenuBn","Skin2","Button down")
	ChangeSkin("MenuBn","Skin3","Button focus")

	CreateWnd(4,"ReputationLabel","DMInGameScreen","Reputation 12",665,20,120,20,0)
	ChangeSkin("ReputationLabel","Skin5","Label")

end

-- Mistlands GUI Resource file, END
