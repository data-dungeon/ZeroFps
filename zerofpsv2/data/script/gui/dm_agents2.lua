-- Mistlands GUI Resource file, START


function GUICreate()

	local AgentPrice = 400 --  Pris på en agent i spelet

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="dm/label_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="dm/misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin3 = { tex1="dm/misc_button_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="dm/misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="dm/portraits/portrait5.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin6 = { tex1="dm/portraits/portrait5b.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin7 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }


	Skin8 = { tex1="dm/final/win_tile.bmp", 
	 tex2="dm/final/window/win_edge_ver.bmp", 
	 tex3="dm/final/window/win_edge_hor.bmp", 
	 tex4="dm/final/window/dm_win_corn.bmp", tex1a="0", 
	 tex2a="dm/final/window/win_edge_ver_a.bmp", 
	 tex3a="dm/final/window/win_edge_hor_a.bmp", 
	 tex4a="dm/final/window/dm_win_corn_a.bmp", bkR=255, bkG=255, bkB=255,
	 borderR=255, borderG=255, borderB=255, bd_size=32, tile=1, trans=0 }


	Skin9 = { tex1="dm/final/members/right_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin10 = { tex1="dm/final/members/right_arrow_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin11 = { tex1="dm/final/members/right_button.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin12 = { tex1="dm/final/members/left_arrow.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin13 = { tex1="dm/final/members/left_arrow_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin14 = { tex1="dm/final/members/left_arrow.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }


	Skin15 = { tex1="dm/final/members/x-button.bmp", tex2="0", tex3="0", tex4="0", tex1a="dm/final/members/x-button_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }
	Skin16 = { tex1="dm/final/members/x-button_d.bmp", tex2="0", tex3="0", tex4="0",
	 tex1a="dm/final/members/x-button_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"AgentsWnd","","",800/2-512/2,600/2-530/2,512,530,0)
	ChangeSkin("AgentsWnd","Skin8","Window")

	CreateWnd(4,"CurrentAgentToHireLabel","AgentsWnd","Agent for hire 1/40 - Mr Smith",32,214,449,16,0)
	ChangeSkin("CurrentAgentToHireLabel","Skin1","Label")

	CreateWnd(1,"FireAgentBn","AgentsWnd","Fire",388,150,80,33,0)
	ChangeSkin("FireAgentBn","Skin2","Button up")
	ChangeSkin("FireAgentBn","Skin3","Button down")
	ChangeSkin("FireAgentBn","Skin4","Button focus")

	CreateWnd(0,"GuiMainWnd","","",0,0,800,600,0)
	ChangeSkin("GuiMainWnd","Skin7","Window")

	HireBnText = "Hire ("..AgentPrice..")"

	CreateWnd(1,"HireAgentBn","AgentsWnd",HireBnText,512/2-120/2,315,120,33,0)
	ChangeSkin("HireAgentBn","Skin2","Button up")
	ChangeSkin("HireAgentBn","Skin3","Button down")
	ChangeSkin("HireAgentBn","Skin4","Button focus")

	CreateWnd(1,"NextAgentInHQ","AgentsWnd","",482,51,16,16,0)
	ChangeSkin("NextAgentInHQ","Skin9","Button up")
	ChangeSkin("NextAgentInHQ","Skin10","Button down")
	ChangeSkin("NextAgentInHQ","Skin11","Button focus")

	CreateWnd(1,"NextAgentToHireBn","AgentsWnd","",482,214,16,16,0)
	ChangeSkin("NextAgentToHireBn","Skin9","Button up")
	ChangeSkin("NextAgentToHireBn","Skin10","Button down")
	ChangeSkin("NextAgentToHireBn","Skin11","Button focus")

	CreateWnd(1,"PrevAgentInHQBn","AgentsWnd","",15,51,16,16,0)
	ChangeSkin("PrevAgentInHQBn","Skin12","Button up")
	ChangeSkin("PrevAgentInHQBn","Skin13","Button down")
	ChangeSkin("PrevAgentInHQBn","Skin14","Button focus")

	CreateWnd(1,"PrevAgentToHireBn","AgentsWnd","",15,214,16,16,0)
	ChangeSkin("PrevAgentToHireBn","Skin12","Button up")
	ChangeSkin("PrevAgentToHireBn","Skin13","Button down")
	ChangeSkin("PrevAgentToHireBn","Skin14","Button focus")

	CreateWnd(1,"SendOutAgentBn","AgentsWnd","Send out",48,149,80,33,0)
	ChangeSkin("SendOutAgentBn","Skin2","Button up")
	ChangeSkin("SendOutAgentBn","Skin3","Button down")
	ChangeSkin("SendOutAgentBn","Skin4","Button focus")

	CreateWnd(4,"AgentInFocusMemberAim","AgentsWnd","Aim:",15,424,481,20,0)
	ChangeSkin("AgentInFocusMemberAim","Skin1","Label")

	CreateWnd(4,"AgentInFocusMemberArmor","AgentsWnd","Armor:",15,464,481,20,0)
	ChangeSkin("AgentInFocusMemberArmor","Skin1","Label")

	CreateWnd(4,"AgentInFocusMemberHP","AgentsWnd","HP:",15,404,481,20,0)
	ChangeSkin("AgentInFocusMemberHP","Skin1","Label")

	CreateWnd(4,"AgentInFocusMemberInfoTxt","AgentsWnd","SELECTED AGENT",15,362,481,20,0)
	ChangeSkin("AgentInFocusMemberInfoTxt","Skin1","Label")

	CreateWnd(4,"AgentInFocusMemberLevel","AgentsWnd","Level:",15,504,481,20,0)
	ChangeSkin("AgentInFocusMemberLevel","Skin1","Label")

	CreateWnd(4,"AgentInFocusMemberName","AgentsWnd","Name:",15,384,481,20,0)
	ChangeSkin("AgentInFocusMemberName","Skin1","Label")

	CreateWnd(4,"AgentInFocusMemberSpeed","AgentsWnd","Speed:",15,444,481,20,0)
	ChangeSkin("AgentInFocusMemberSpeed","Skin1","Label")

	CreateWnd(4,"AgentInFocusMemberWage","AgentsWnd","Wage:",15,484,481,20,0)
	ChangeSkin("AgentInFocusMemberWage","Skin1","Label")

	CreateWnd(4,"AgentInHQLabel","AgentsWnd","Your agent 1/10 - Mr Smith",32,51,449,16,0)
	ChangeSkin("AgentInHQLabel","Skin1","Label")

	CreateWnd(1,"AgentsClose","AgentsWnd","",472,9,32,32,0)
	ChangeSkin("AgentsClose","Skin15","Button up")
	ChangeSkin("AgentsClose","Skin16","Button down")
	ChangeSkin("AgentsClose","Skin15","Button focus")

	CreateWnd(1,"AgentsEquip","AgentsWnd","Storeroom",275,150,80,33,0)
	ChangeSkin("AgentsEquip","Skin2","Button up")
	ChangeSkin("AgentsEquip","Skin3","Button down")
	ChangeSkin("AgentsEquip","Skin4","Button focus")

	CreateWnd(2,"AgentsInHQBn1","AgentsWnd","",15,74,64,64,0)
	ChangeSkin("AgentsInHQBn1","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsInHQBn1","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsInHQBn1","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsInHQBn2","AgentsWnd","",85,73,64,64,0)
	ChangeSkin("AgentsInHQBn2","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsInHQBn2","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsInHQBn2","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsInHQBn3","AgentsWnd","",154,73,64,64,0)
	ChangeSkin("AgentsInHQBn3","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsInHQBn3","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsInHQBn3","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsInHQBn4","AgentsWnd","",223,73,64,64,0)
	ChangeSkin("AgentsInHQBn4","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsInHQBn4","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsInHQBn4","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsInHQBn5","AgentsWnd","",293,73,64,64,0)
	ChangeSkin("AgentsInHQBn5","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsInHQBn5","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsInHQBn5","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsInHQBn6","AgentsWnd","",363,73,64,64,0)
	ChangeSkin("AgentsInHQBn6","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsInHQBn6","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsInHQBn6","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsInHQBn7","AgentsWnd","",433,73,64,64,0)
	ChangeSkin("AgentsInHQBn7","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsInHQBn7","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsInHQBn7","Skin7","Checkbox: Label")

	CreateWnd(4,"AgentsMoneyLabel","AgentsWnd","Money:",14,15,274,20,0)
	ChangeSkin("AgentsMoneyLabel","Skin1","Label")

	CreateWnd(4,"AgentsReputaionLabel","AgentsWnd","Reputation: ",288,15,200,20,0)
	ChangeSkin("AgentsReputaionLabel","Skin1","Label")

	CreateWnd(1,"AgentsShop","AgentsWnd","Shop",161,150,80,33,0)
	ChangeSkin("AgentsShop","Skin2","Button up")
	ChangeSkin("AgentsShop","Skin3","Button down")
	ChangeSkin("AgentsShop","Skin4","Button focus")

	CreateWnd(2,"AgentsToHireBn1","AgentsWnd","",15,238,64,64,0)
	ChangeSkin("AgentsToHireBn1","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsToHireBn1","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsToHireBn1","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsToHireBn2","AgentsWnd","",84,238,64,64,0)
	ChangeSkin("AgentsToHireBn2","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsToHireBn2","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsToHireBn2","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsToHireBn3","AgentsWnd","",154,238,64,64,0)
	ChangeSkin("AgentsToHireBn3","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsToHireBn3","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsToHireBn3","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsToHireBn4","AgentsWnd","",224,238,64,64,0)
	ChangeSkin("AgentsToHireBn4","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsToHireBn4","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsToHireBn4","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsToHireBn5","AgentsWnd","",294,238,64,64,0)
	ChangeSkin("AgentsToHireBn5","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsToHireBn5","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsToHireBn5","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsToHireBn6","AgentsWnd","",363,238,64,64,0)
	ChangeSkin("AgentsToHireBn6","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsToHireBn6","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsToHireBn6","Skin7","Checkbox: Label")

	CreateWnd(2,"AgentsToHireBn7","AgentsWnd","",433,238,64,64,0)
	ChangeSkin("AgentsToHireBn7","Skin5","Checkbox: Button up")
	ChangeSkin("AgentsToHireBn7","Skin6","Checkbox: Button down")
	ChangeSkin("AgentsToHireBn7","Skin7","Checkbox: Label")

end

-- Mistlands GUI Resource file, END
