-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="delete_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="delete_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin3 = { tex1="delete_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=214, bkG=211,
	 bkB=206, borderR=92, borderG=92, borderB=92, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="lb_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	Skin6 = { tex1="lb_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin7 = { tex1="lb_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="lb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=56, borderG=56, borderB=56, bd_size=1, tile=0, trans=0 }
	Skin9 = { tex1="sb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin10 = { tex1="sb_n.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin11 = { tex1="sb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin12 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin13 = { tex1="treebox_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	Skin14 = { tex1="workbutton_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin15 = { tex1="workbutton_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin16 = { tex1="workbutton_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin17 = { tex1="placeonground_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin18 = { tex1="placeonground_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=0, bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin19 = { tex1="placeonground_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin20 = { tex1="rot_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin21 = { tex1="rot_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin22 = { tex1="rot_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin23 = { tex1="togglelight_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin24 = { tex1="togglelight_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin25 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=128, bkG=128,
	 bkB=128, borderR=92, borderG=92, borderB=92, bd_size=0, tile=0, trans=1 }
	Skin26 = { tex1="tab_bnNext_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin27 = { tex1="tab_bnNext_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin28 = { tex1="tab_bnPrev_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin29 = { tex1="tab_bnPrev_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin30 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=128, bkG=128,
	 bkB=128, borderR=92, borderG=92, borderB=92, bd_size=1, tile=0, trans=0 }
	Skin31 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=214, bkG=211,
	 bkB=206, borderR=0, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }
	Skin32 = { tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=2,
	 tile=1, trans=1 }
	Skin33 = { tex1="0", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=164, bkG=164, bkB=164, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	Skin34 = { tex1="sb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0, rots90deg=2 }

	Skin35 = { tex1="move.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0, rots90deg=2 }
	Skin36 = { tex1="move.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=0,
	 bkB=0, borderR=0, borderG=0, borderB=255, bd_size=0, tile=0, trans=0, rots90deg=2 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-- Parameter 10 (second last) is window alignment to screen or parent window:
	-- 0 = TopLeft, 1 = TopRight, 2 = BottomLeft, 3 = BottomRight, 4 = CenterHorz, 5 = CenterVert, 6 = Center
	-- Parameter 11 (last) is window resize type in relation to screen or parent window:
	-- 0 = None, 1 = MaxSize, 2 = MaxWidth, 3 = MaxHeight
	-------------------------------------------------------------------------

--	local sw = GetScreenWidth()
--	local sh = GetScreenWidth()

	CreateWnd(0,"GuiMainWnd","","",0,0,800,600,0,0,0)
	ChangeSkin("GuiMainWnd","Skin12","Window")

	CreateWnd(1,"OpenWorkTabButton","GuiMainWnd","",760,560,32,32,0,3,0)
	ChangeSkin("OpenWorkTabButton","Skin14","Button up")
	ChangeSkin("OpenWorkTabButton","Skin15","Button down")
	ChangeSkin("OpenWorkTabButton","Skin14","Button focus")

	CreateWnd(2,"ToggleLight","GuiMainWnd","",720,560,32,32,0,3,0)
	ChangeSkin("ToggleLight","Skin23","Checkbox: Button up")
	ChangeSkin("ToggleLight","Skin24","Checkbox: Button down")
	ChangeSkin("ToggleLight","Skin12","Checkbox: Label")

	CreateWnd(0,"vp1","GuiMainWnd","",282,24,250,250,0,0,0)
	ChangeSkin("vp1","Skin32","Window")

	CreateWnd(0,"vp2","GuiMainWnd","",17,25,250,250,0,0,0)
	ChangeSkin("vp2","Skin32","Window")

	CreateWnd(0,"vp3","GuiMainWnd","",18,291,250,250,0,0,0)
	ChangeSkin("vp3","Skin32","Window")

	CreateWnd(0,"vp4","GuiMainWnd","",284,290,250,250,0,0,0)
	ChangeSkin("vp4","Skin32","Window")

	CreateWnd(0,"worktab","GuiMainWnd","",527,288,256,257,0,3,0)
	ChangeSkin("worktab","Skin33","Window")

	CreateWnd(9,"WorkTabWnd","worktab","",0,0,256,256,0,0,0)
	ChangeSkin("WorkTabWnd","Skin25","Tabctrl")
	ChangeSkin("WorkTabWnd","Skin26","Tabctrl: nexttab: Button up")
	ChangeSkin("WorkTabWnd","Skin27","Tabctrl: nexttab: Button down")
	ChangeSkin("WorkTabWnd","Skin26","Tabctrl: nexttab: Button focus")
	ChangeSkin("WorkTabWnd","Skin28","Tabctrl: prevtab: Button up")
	ChangeSkin("WorkTabWnd","Skin29","Tabctrl: prevtab: Button down")
	ChangeSkin("WorkTabWnd","Skin28","Tabctrl: prevtab: Button focus")
	ChangeSkin("WorkTabWnd","Skin30","Tabctrl: UnSeltab")
	ChangeSkin("WorkTabWnd","Skin31","Tabctrl: Seltab")

	CreateWnd(0,"ZonePage","WorkTabWnd","Zone",2,27,252,227,0,0,0)
	ChangeSkin("ZonePage","Skin4","Window")

	CreateWnd(1,"RotateZoneModellButton","ZonePage","",224,12,16,16,0,0,0)
	ChangeSkin("RotateZoneModellButton","Skin20","Button up")
	ChangeSkin("RotateZoneModellButton","Skin21","Button down")
	ChangeSkin("RotateZoneModellButton","Skin22","Button focus")

	CreateWnd(1,"DeleteZoneButton","ZonePage","",224,38,16,16,0,0,0)
	ChangeSkin("DeleteZoneButton","Skin1","Button up")
	ChangeSkin("DeleteZoneButton","Skin2","Button down")
	ChangeSkin("DeleteZoneButton","Skin3","Button focus")

	CreateWnd(2,"DisableFreeZoneBuildBn","ZonePage","",224,64,16,16,0,0,0)
	ChangeSkin("DisableFreeZoneBuildBn","Skin35","Checkbox: Button up")
	ChangeSkin("DisableFreeZoneBuildBn","Skin36","Checkbox: Button down")

	CreateWnd(0,"ObjectPage","WorkTabWnd","Object",2,27,252,227,0,0,0)
	ChangeSkin("ObjectPage","Skin4","Window")

	CreateWnd(11,"ObjectTree","ObjectPage","",10,20,200,200,0,0,0)
	ChangeSkin("ObjectTree","Skin13","Treebox")
	ChangeSkin("ObjectTree","Skin9","Treebox: V.Scrollbar")
	ChangeSkin("ObjectTree","Skin10","Treebox: V.Scrollbar: Button up")
	ChangeSkin("ObjectTree","Skin11","Treebox: V.Scrollbar: Button down")
	ChangeSkin("ObjectTree","Skin11","Treebox: V.Scrollbar: Button focus")
	ChangeSkin("ObjectTree","Skin9","Treebox: H.Scrollbar")
	ChangeSkin("ObjectTree","Skin10","Treebox: H.Scrollbar: Button up")
	ChangeSkin("ObjectTree","Skin11","Treebox: H.Scrollbar: Button down")
	ChangeSkin("ObjectTree","Skin11","Treebox: H.Scrollbar: Button focus")

	CreateWnd(1,"RotateObjectButton","ObjectPage","",224,12,16,16,0,0,0)
	ChangeSkin("RotateObjectButton","Skin20","Button up")
	ChangeSkin("RotateObjectButton","Skin21","Button down")
	ChangeSkin("RotateObjectButton","Skin22","Button focus")

	CreateWnd(1,"DeleteObjectButton","ObjectPage","",224,38,16,16,0,0,0)
	ChangeSkin("DeleteObjectButton","Skin1","Button up")
	ChangeSkin("DeleteObjectButton","Skin2","Button down")
	ChangeSkin("DeleteObjectButton","Skin3","Button focus")

	CreateWnd(2,"PlaceongroundButton","ObjectPage","",224,64,16,16,0,0,0)
	ChangeSkin("PlaceongroundButton","Skin17","Checkbox: Button up")
	ChangeSkin("PlaceongroundButton","Skin18","Checkbox: Button down")

	CreateWnd(11,"ZoneModelTree","ZonePage","",10,20,200,200,0,0,0)
	ChangeSkin("ZoneModelTree","Skin13","Treebox")
	ChangeSkin("ZoneModelTree","Skin9","Treebox: V.Scrollbar")
	ChangeSkin("ZoneModelTree","Skin10","Treebox: V.Scrollbar: Button up")
	ChangeSkin("ZoneModelTree","Skin11","Treebox: V.Scrollbar: Button down")
	ChangeSkin("ZoneModelTree","Skin11","Treebox: V.Scrollbar: Button focus")
	ChangeSkin("ZoneModelTree","Skin9","Treebox: H.Scrollbar")
	ChangeSkin("ZoneModelTree","Skin10","Treebox: H.Scrollbar: Button up")
	ChangeSkin("ZoneModelTree","Skin11","Treebox: H.Scrollbar: Button down")
	ChangeSkin("ZoneModelTree","Skin11","Treebox: H.Scrollbar: Button focus")

	CreateWnd(0,"PropertyPage","WorkTabWnd","Properties",2,27,252,227,0,0,0)
	ChangeSkin("PropertyPage","Skin4","Window")

	CreateWnd(5,"PropertyList","PropertyPage","",10,20,200,200,0,0,0)
	
	CreateWnd(1,"RemovePropertyBn","PropertyPage","-",224,12,16,16,0,0,0)
	ChangeSkin("RemovePropertyBn","Skin11","Button up")
	ChangeSkin("RemovePropertyBn","Skin34","Button down")
	ChangeSkin("RemovePropertyBn","Skin11","Button focus")

	CreateWnd(1,"OpenAllPropertyWnd","PropertyPage","+",224,38,16,16,0,0,0)
	ChangeSkin("OpenAllPropertyWnd","Skin11","Button up")
	ChangeSkin("OpenAllPropertyWnd","Skin34","Button down")
	ChangeSkin("OpenAllPropertyWnd","Skin11","Button focus")

	CreateWnd(1,"OpenEditPropertyWnd","PropertyPage","?",224,64,16,16,0,0,0)
	ChangeSkin("OpenEditPropertyWnd","Skin11","Button up")
	ChangeSkin("OpenEditPropertyWnd","Skin34","Button down")
	ChangeSkin("OpenEditPropertyWnd","Skin11","Button focus")

	CreateWnd(0,"CustomEntitiesPage","WorkTabWnd","Custom",10,20,200,200,0,0,0)
	CreateWnd(11,"CustomEntitiesTree","CustomEntitiesPage","",10,20,200,200,0,0,0)
	ChangeSkin("CustomEntitiesTree","Skin13","Treebox")
	ChangeSkin("CustomEntitiesTree","Skin9","Treebox: V.Scrollbar")
	ChangeSkin("CustomEntitiesTree","Skin10","Treebox: V.Scrollbar: Button up")
	ChangeSkin("CustomEntitiesTree","Skin11","Treebox: V.Scrollbar: Button down")
	ChangeSkin("CustomEntitiesTree","Skin11","Treebox: V.Scrollbar: Button focus")
	ChangeSkin("CustomEntitiesTree","Skin9","Treebox: H.Scrollbar")
	ChangeSkin("CustomEntitiesTree","Skin10","Treebox: H.Scrollbar: Button up")
	ChangeSkin("CustomEntitiesTree","Skin11","Treebox: H.Scrollbar: Button down")
	ChangeSkin("CustomEntitiesTree","Skin11","Treebox: H.Scrollbar: Button focus")

	CreateWnd(1,"AddToScriptBn","CustomEntitiesPage","+",224,12,16,16,0,0,0)
	ChangeSkin("AddToScriptBn","Skin11","Button up")
	ChangeSkin("AddToScriptBn","Skin34","Button down")
	ChangeSkin("AddToScriptBn","Skin11","Button focus")

	CreateWnd(4,"SaveScriptFileNameLabel","CustomEntitiesPage","Enter name (and press Return):",10,180,200,20,0,0,0)
	CreateWnd(10,"SaveScriptFileNameEb","CustomEntitiesPage","",10,200,200,20,0,0,0)
	ChangeSkin("SaveScriptFileNameEb","Skin30","Textbox")
	ShowWnd("SaveScriptFileNameEb",0,0)

	CreateWnd(0,"EnviromentPage","WorkTabWnd","Enviroment",2,27,252,227,0,0,0)
	ChangeSkin("EnviromentPage","Skin4","Window")

	CreateWnd(5,"EnviromentPresetList","EnviromentPage","",10,20,200,200,0,0,0)
	ChangeSkin("EnviromentPresetList","Skin5","Listbox")
	ChangeSkin("EnviromentPresetList","Skin6","Listbox: Button up")
	ChangeSkin("EnviromentPresetList","Skin7","Listbox: Button down")
	ChangeSkin("EnviromentPresetList","Skin8","Listbox: Button focus")
	ChangeSkin("EnviromentPresetList","Skin9","Listbox: Scrollbar")
	ChangeSkin("EnviromentPresetList","Skin10","Listbox: Scrollbar: Button up")
	ChangeSkin("EnviromentPresetList","Skin11","Listbox: Scrollbar: Button down")
	ChangeSkin("EnviromentPresetList","Skin11","Listbox: Scrollbar: Button focus")

	CreateWnd(0,"AddNewProperyWnd","GuiMainWnd","",527,288-257+16,256,227,0,3,0)
	ShowWnd("AddNewProperyWnd",0,0)
	ChangeSkin("AddNewProperyWnd","Skin33","Window")

	CreateWnd(5,"AllPropertyList","AddNewProperyWnd","",10,8,200,200,0,0,0)

	CreateWnd(1,"AddPropertyBn","AddNewProperyWnd","+",224,12,16,16,0,0,0)
	ChangeSkin("AddPropertyBn","Skin11","Button up")
	ChangeSkin("AddPropertyBn","Skin34","Button down")
	ChangeSkin("AddPropertyBn","Skin11","Button focus")

	CreateWnd(0,"EditPropertyWnd","GuiMainWnd","",527,288-257+16,256,227,0,3,0)
	ShowWnd("EditPropertyWnd",0,0)
	ChangeSkin("EditPropertyWnd","Skin33","Window")

	CreateWnd(5,"PropertyValList","EditPropertyWnd","",10,10,236,200-22,0,0,0)
	CreateWnd(10,"PropertyValEb","EditPropertyWnd","",10,198+4,236,20,0,0,0)
	ChangeSkin("PropertyValEb","Skin31","Textbox")


end

-- Mistlands GUI Resource file, END
