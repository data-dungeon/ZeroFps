-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------

	Skin1 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=0, bkG=0,
	 bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin3 = { tex1="dm/hq_inventory_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="dm/hq_inventory_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="dm/hq_inventory_bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }


	Skin6 = { tex1="dm/hq_members_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin7 = { tex1="dm/hq_members_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="dm/hq_members_bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }


	Skin9 = { tex1="dm/hq_missions_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin10 = { tex1="dm/hq_missions_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin11 = { tex1="dm/hq_missions_bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }


	Skin12 = { tex1="dm/hq_hire_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin13 = { tex1="dm/hq_hire_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin14 = { tex1="dm/hq_hire_bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }


	Skin15 = { tex1="dm/hq_close_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin16 = { tex1="dm/hq_close_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin17 = { tex1="dm/hq_close_bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }


	Skin18 = { tex1="dm/bn1u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin19 = { tex1="dm/bn1d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin20 = { tex1="dm/bn1f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }



	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------

	CreateWnd(0,"HQWnd","","",200,150,400,300,0)
	ChangeSkin("HQWnd","Skin1","Window")

	CreateWnd(1,"HQMissionBn","HQWnd","Mission",400/2-200/2,64,200,25,0)
	ChangeSkin("HQMissionBn","Skin18","Button up")
	ChangeSkin("HQMissionBn","Skin19","Button down")
	ChangeSkin("HQMissionBn","Skin20","Button focus")

--	CreateWnd(1,"HQInventoryBn","HQWnd","Shop",400/2-200/2,64+30,200,25,0)
--	ChangeSkin("HQInventoryBn","Skin18","Button up")
--	ChangeSkin("HQInventoryBn","Skin19","Button down")
--	ChangeSkin("HQInventoryBn","Skin20","Button focus")

--	CreateWnd(1,"HQMembersBn","HQWnd","Members",400/2-200/2,64+60,200,25,0)
--	ChangeSkin("HQMembersBn","Skin18","Button up")
--	ChangeSkin("HQMembersBn","Skin19","Button down")
--	ChangeSkin("HQMembersBn","Skin20","Button focus")

	CreateWnd(1,"HQHireBn","HQWnd","Agents",400/2-200/2,64+30,200,25,0)
	ChangeSkin("HQHireBn","Skin18","Button up")
	ChangeSkin("HQHireBn","Skin19","Button down")
	ChangeSkin("HQHireBn","Skin20","Button focus")

	CreateWnd(1,"HQCloseBn","HQWnd","Continue",400/2-200/2,64+60,200,25,0)
	ChangeSkin("HQCloseBn","Skin18","Button up")
	ChangeSkin("HQCloseBn","Skin19","Button down")
	ChangeSkin("HQCloseBn","Skin20","Button focus")

end

-- Mistlands GUI Resource file, END
