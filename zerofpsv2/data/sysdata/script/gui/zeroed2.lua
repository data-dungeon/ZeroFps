-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="0", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp", tex1a="0",
	 tex2a="0", tex3a="0", tex4a="0", bkR=164, bkG=164, bkB=164, borderR=255, borderG=255, borderB=255, bd_size=8, tile=1,
	 trans=0 }
	Skin2 = { tex1="sb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin3 = { tex1="save_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="save_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=0,
	 bkB=0, borderR=0, borderG=0, borderB=255, bd_size=0, tile=0, trans=0 }
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
	Skin11 = { tex1="scrollbar_clicktop_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin12 = { tex1="scrollbar_clicktop_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin13 = { tex1="scrollbar_clickbottom_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin14 = { tex1="scrollbar_clickbottom_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin15 = { tex1="delete_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin16 = { tex1="delete_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin17 = { tex1="delete_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin18 = { tex1="move.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin19 = { tex1="move.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=0,
	 bkB=0, borderR=0, borderG=0, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin20 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin21 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=214, bkG=211,
	 bkB=206, borderR=92, borderG=92, borderB=92, bd_size=0, tile=0, trans=0 }
	Skin22 = { tex1="treebox_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	Skin23 = { tex1="workbutton_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin24 = { tex1="workbutton_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin25 = { tex1="placeonground_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin26 = { tex1="placeonground_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=0, bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin27 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=214, bkG=211,
	 bkB=206, borderR=0, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }
	Skin28 = { tex1="rot_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin29 = { tex1="rot_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin30 = { tex1="rot_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin31 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=128, bkG=128,
	 bkB=128, borderR=92, borderG=92, borderB=92, bd_size=1, tile=0, trans=0 }
	Skin32 = { tex1="togglelight_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin33 = { tex1="togglelight_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin34 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=128, bkG=128,
	 bkB=128, borderR=92, borderG=92, borderB=92, bd_size=0, tile=0, trans=1 }
	Skin35 = { tex1="tab_bnNext_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin36 = { tex1="tab_bnNext_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin37 = { tex1="tab_bnPrev_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin38 = { tex1="tab_bnPrev_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin39 = { tex1="label_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="label_bk_a.bmp", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin40 = { tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=2,
	 tile=1, trans=1 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"AddNewProperyWnd","","",527,47,256,227,0,3,0)
	ChangeSkin("AddNewProperyWnd","Skin1","Window")

	CreateWnd(1,"AddPropertyBn","AddNewProperyWnd","+",224,12,16,16,0,0,0)
	ChangeSkin("AddPropertyBn","Skin2","Button up")
	ChangeSkin("AddPropertyBn","Skin2","Button down")
	ChangeSkin("AddPropertyBn","Skin2","Button focus")

	CreateWnd(5,"AllPropertyList","AddNewProperyWnd","",10,8,200,200,0,0,0)
	ChangeSkin("AllPropertyList","Skin5","Listbox")
	ChangeSkin("AllPropertyList","Skin6","Listbox: Button up")
	ChangeSkin("AllPropertyList","Skin7","Listbox: Button down")
	ChangeSkin("AllPropertyList","Skin8","Listbox: Button focus")
	ChangeSkin("AllPropertyList","Skin9","Listbox: Scrollbar")
	ChangeSkin("AllPropertyList","Skin10","Listbox: Scrollbar: Button up")
	ChangeSkin("AllPropertyList","Skin2","Listbox: Scrollbar: Button down")
	ChangeSkin("AllPropertyList","Skin2","Listbox: Scrollbar: Button focus")
	ChangeSkin("AllPropertyList","Skin11","Listbox: Scrollbar: Top: Button up")
	ChangeSkin("AllPropertyList","Skin12","Listbox: Scrollbar: Top: Button down")
	ChangeSkin("AllPropertyList","Skin11","Listbox: Scrollbar: Top: Button focus")
	ChangeSkin("AllPropertyList","Skin13","Listbox: Scrollbar: Bottom: Button up")
	ChangeSkin("AllPropertyList","Skin14","Listbox: Scrollbar: Bottom: Button down")
	ChangeSkin("AllPropertyList","Skin13","Listbox: Scrollbar: Bottom: Button focus")

	CreateWnd(0,"EditPropertyWnd","","",527,47,256,227,0,3,0)
	ChangeSkin("EditPropertyWnd","Skin1","Window")

	CreateWnd(0,"GuiMainWnd","","",0,0,1024,768,0,0,0)
	ChangeSkin("GuiMainWnd","Skin20","Window")

	CreateWnd(1,"OpenWorkTabButton","GuiMainWnd","",760,560,32,32,0,3,0)
	ChangeSkin("OpenWorkTabButton","Skin23","Button up")
	ChangeSkin("OpenWorkTabButton","Skin24","Button down")
	ChangeSkin("OpenWorkTabButton","Skin23","Button focus")

	CreateWnd(10,"PropertyValEb","EditPropertyWnd","",10,202,236,20,0,0,0)
	ChangeSkin("PropertyValEb","Skin27","Textbox")

	CreateWnd(5,"PropertyValList","EditPropertyWnd","",10,10,236,178,0,0,0)
	ChangeSkin("PropertyValList","Skin5","Listbox")
	ChangeSkin("PropertyValList","Skin6","Listbox: Button up")
	ChangeSkin("PropertyValList","Skin7","Listbox: Button down")
	ChangeSkin("PropertyValList","Skin8","Listbox: Button focus")
	ChangeSkin("PropertyValList","Skin9","Listbox: Scrollbar")
	ChangeSkin("PropertyValList","Skin10","Listbox: Scrollbar: Button up")
	ChangeSkin("PropertyValList","Skin2","Listbox: Scrollbar: Button down")
	ChangeSkin("PropertyValList","Skin2","Listbox: Scrollbar: Button focus")
	ChangeSkin("PropertyValList","Skin11","Listbox: Scrollbar: Top: Button up")
	ChangeSkin("PropertyValList","Skin12","Listbox: Scrollbar: Top: Button down")
	ChangeSkin("PropertyValList","Skin11","Listbox: Scrollbar: Top: Button focus")
	ChangeSkin("PropertyValList","Skin13","Listbox: Scrollbar: Bottom: Button up")
	ChangeSkin("PropertyValList","Skin14","Listbox: Scrollbar: Bottom: Button down")
	ChangeSkin("PropertyValList","Skin13","Listbox: Scrollbar: Bottom: Button focus")

	CreateWnd(2,"ToggleLight","GuiMainWnd","",720,560,32,32,0,3,0)
	ChangeSkin("ToggleLight","Skin32","Checkbox: Button up")
	ChangeSkin("ToggleLight","Skin33","Checkbox: Button down")
	ChangeSkin("ToggleLight","Skin20","Checkbox: Label")

	CreateWnd(4,"ZeroEdInfoLabel","GuiMainWnd","Label500",12,566,699,20,0,2,0)
	ChangeSkin("ZeroEdInfoLabel","Skin39","Label")

	CreateWnd(0,"vp1","","",282,24,250,250,0,0,0)
	ChangeSkin("vp1","Skin40","Window")

	CreateWnd(0,"vp2","","",17,25,250,250,0,0,0)
	ChangeSkin("vp2","Skin40","Window")

	CreateWnd(0,"vp3","","",18,291,250,250,0,0,0)
	ChangeSkin("vp3","Skin40","Window")

	CreateWnd(0,"vp4","","",284,290,250,250,0,0,0)
	ChangeSkin("vp4","Skin40","Window")

	CreateWnd(0,"worktab","","",527,288,256,257,0,3,0)
	ChangeSkin("worktab","Skin1","Window")

	CreateWnd(9,"WorkTabWnd","worktab","",0,0,256,256,0,0,0)
	ChangeSkin("WorkTabWnd","Skin34","Tabctrl")
	ChangeSkin("WorkTabWnd","Skin35","Tabctrl: nexttab: Button up")
	ChangeSkin("WorkTabWnd","Skin36","Tabctrl: nexttab: Button down")
	ChangeSkin("WorkTabWnd","Skin35","Tabctrl: nexttab: Button focus")
	ChangeSkin("WorkTabWnd","Skin37","Tabctrl: prevtab: Button up")
	ChangeSkin("WorkTabWnd","Skin38","Tabctrl: prevtab: Button down")
	ChangeSkin("WorkTabWnd","Skin37","Tabctrl: prevtab: Button focus")
	ChangeSkin("WorkTabWnd","Skin31","Tabctrl: UnSeltab")
	ChangeSkin("WorkTabWnd","Skin27","Tabctrl: Seltab")

	CreateWnd(0,"ZonePage","WorkTabWnd","Zone",2,27,252,227,0,0,0)
	ChangeSkin("ZonePage","Skin21","Window")

	CreateWnd(1,"DeleteZoneButton","ZonePage","",224,38,16,16,0,0,0)
	ChangeSkin("DeleteZoneButton","Skin15","Button up")
	ChangeSkin("DeleteZoneButton","Skin16","Button down")
	ChangeSkin("DeleteZoneButton","Skin17","Button focus")

	CreateWnd(2,"DisableFreeZoneBuildBn","ZonePage","",224,64,16,16,0,0,0)
	ChangeSkin("DisableFreeZoneBuildBn","Skin18","Checkbox: Button up")
	ChangeSkin("DisableFreeZoneBuildBn","Skin19","Checkbox: Button down")
	ChangeSkin("DisableFreeZoneBuildBn","Skin20","Checkbox: Label")

	CreateWnd(0,"ObjectPage","WorkTabWnd","Object",2,27,252,227,0,0,0)
	ChangeSkin("ObjectPage","Skin21","Window")

	CreateWnd(11,"ObjectTree","ObjectPage","",10,20,200,200,0,0,0)
	ChangeSkin("ObjectTree","Skin22","Treebox")
	ChangeSkin("ObjectTree","Skin9","Treebox: V.Scrollbar")
	ChangeSkin("ObjectTree","Skin10","Treebox: V.Scrollbar: Button up")
	ChangeSkin("ObjectTree","Skin2","Treebox: V.Scrollbar: Button down")
	ChangeSkin("ObjectTree","Skin2","Treebox: V.Scrollbar: Button focus")
	ChangeSkin("ObjectTree","Skin11","Treebox: V.Scrollbar: Top: Button up")
	ChangeSkin("ObjectTree","Skin12","Treebox: V.Scrollbar: Top: Button down")
	ChangeSkin("ObjectTree","Skin11","Treebox: V.Scrollbar: Top: Button focus")
	ChangeSkin("ObjectTree","Skin13","Treebox: V.Scrollbar: Bottom: Button up")
	ChangeSkin("ObjectTree","Skin14","Treebox: V.Scrollbar: Bottom: Button down")
	ChangeSkin("ObjectTree","Skin13","Treebox: V.Scrollbar: Bottom: Button focus")
	ChangeSkin("ObjectTree","Skin9","Treebox: H.Scrollbar")
	ChangeSkin("ObjectTree","Skin10","Treebox: H.Scrollbar: Button up")
	ChangeSkin("ObjectTree","Skin2","Treebox: H.Scrollbar: Button down")
	ChangeSkin("ObjectTree","Skin2","Treebox: H.Scrollbar: Button focus")
	ChangeSkin("ObjectTree","Skin11","Treebox: H.Scrollbar: Top: Button up")
	ChangeSkin("ObjectTree","Skin12","Treebox: H.Scrollbar: Top: Button down")
	ChangeSkin("ObjectTree","Skin11","Treebox: H.Scrollbar: Top: Button focus")
	ChangeSkin("ObjectTree","Skin13","Treebox: H.Scrollbar: Bottom: Button up")
	ChangeSkin("ObjectTree","Skin14","Treebox: H.Scrollbar: Bottom: Button down")
	ChangeSkin("ObjectTree","Skin13","Treebox: H.Scrollbar: Bottom: Button focus")

	CreateWnd(2,"PlaceongroundButton","ObjectPage","",224,64,16,16,0,0,0)
	ChangeSkin("PlaceongroundButton","Skin25","Checkbox: Button up")
	ChangeSkin("PlaceongroundButton","Skin26","Checkbox: Button down")
	ChangeSkin("PlaceongroundButton","Skin20","Checkbox: Label")

	CreateWnd(0,"PropertyPage","WorkTabWnd","Properties",2,27,252,227,0,0,0)
	ChangeSkin("PropertyPage","Skin21","Window")

	CreateWnd(1,"RemovePropertyBn","PropertyPage","-",224,12,16,16,0,0,0)
	ChangeSkin("RemovePropertyBn","Skin2","Button up")
	ChangeSkin("RemovePropertyBn","Skin2","Button down")
	ChangeSkin("RemovePropertyBn","Skin2","Button focus")

	CreateWnd(1,"RotateObjectButton","ObjectPage","",224,12,16,16,0,0,0)
	ChangeSkin("RotateObjectButton","Skin28","Button up")
	ChangeSkin("RotateObjectButton","Skin29","Button down")
	ChangeSkin("RotateObjectButton","Skin30","Button focus")

	CreateWnd(1,"RotateZoneModellButton","ZonePage","",224,12,16,16,0,0,0)
	ChangeSkin("RotateZoneModellButton","Skin28","Button up")
	ChangeSkin("RotateZoneModellButton","Skin29","Button down")
	ChangeSkin("RotateZoneModellButton","Skin30","Button focus")

	CreateWnd(10,"SaveScriptFileNameEb","ObjectPage","",10,200,200,20,0,0,0)
	ChangeSkin("SaveScriptFileNameEb","Skin31","Textbox")

	CreateWnd(4,"SaveScriptFileNameLabel","ObjectPage","Enter name (and press Return):",10,180,200,20,0,0,0)
	ChangeSkin("SaveScriptFileNameLabel","Skin20","Label")

	CreateWnd(11,"ZoneModelTree","ZonePage","",10,20,200,200,0,0,0)
	ChangeSkin("ZoneModelTree","Skin22","Treebox")
	ChangeSkin("ZoneModelTree","Skin9","Treebox: V.Scrollbar")
	ChangeSkin("ZoneModelTree","Skin10","Treebox: V.Scrollbar: Button up")
	ChangeSkin("ZoneModelTree","Skin2","Treebox: V.Scrollbar: Button down")
	ChangeSkin("ZoneModelTree","Skin2","Treebox: V.Scrollbar: Button focus")
	ChangeSkin("ZoneModelTree","Skin11","Treebox: V.Scrollbar: Top: Button up")
	ChangeSkin("ZoneModelTree","Skin12","Treebox: V.Scrollbar: Top: Button down")
	ChangeSkin("ZoneModelTree","Skin11","Treebox: V.Scrollbar: Top: Button focus")
	ChangeSkin("ZoneModelTree","Skin13","Treebox: V.Scrollbar: Bottom: Button up")
	ChangeSkin("ZoneModelTree","Skin14","Treebox: V.Scrollbar: Bottom: Button down")
	ChangeSkin("ZoneModelTree","Skin13","Treebox: V.Scrollbar: Bottom: Button focus")
	ChangeSkin("ZoneModelTree","Skin9","Treebox: H.Scrollbar")
	ChangeSkin("ZoneModelTree","Skin10","Treebox: H.Scrollbar: Button up")
	ChangeSkin("ZoneModelTree","Skin2","Treebox: H.Scrollbar: Button down")
	ChangeSkin("ZoneModelTree","Skin2","Treebox: H.Scrollbar: Button focus")
	ChangeSkin("ZoneModelTree","Skin11","Treebox: H.Scrollbar: Top: Button up")
	ChangeSkin("ZoneModelTree","Skin12","Treebox: H.Scrollbar: Top: Button down")
	ChangeSkin("ZoneModelTree","Skin11","Treebox: H.Scrollbar: Top: Button focus")
	ChangeSkin("ZoneModelTree","Skin13","Treebox: H.Scrollbar: Bottom: Button up")
	ChangeSkin("ZoneModelTree","Skin14","Treebox: H.Scrollbar: Bottom: Button down")
	ChangeSkin("ZoneModelTree","Skin13","Treebox: H.Scrollbar: Bottom: Button focus")

	CreateWnd(1,"AddToScriptBn","ObjectPage","",224,90,24,22,0,0,0)
	ChangeSkin("AddToScriptBn","Skin3","Button up")
	ChangeSkin("AddToScriptBn","Skin4","Button down")
	ChangeSkin("AddToScriptBn","Skin3","Button focus")

	CreateWnd(1,"DeleteObjectButton","ObjectPage","",224,38,16,16,0,0,0)
	ChangeSkin("DeleteObjectButton","Skin15","Button up")
	ChangeSkin("DeleteObjectButton","Skin16","Button down")
	ChangeSkin("DeleteObjectButton","Skin17","Button focus")

	CreateWnd(0,"EnviromentPage","WorkTabWnd","Enviroment",2,27,252,227,0,0,0)
	ChangeSkin("EnviromentPage","Skin21","Window")

	CreateWnd(5,"EnviromentPresetList","EnviromentPage","",10,20,200,200,0,0,0)
	ChangeSkin("EnviromentPresetList","Skin5","Listbox")
	ChangeSkin("EnviromentPresetList","Skin6","Listbox: Button up")
	ChangeSkin("EnviromentPresetList","Skin7","Listbox: Button down")
	ChangeSkin("EnviromentPresetList","Skin8","Listbox: Button focus")
	ChangeSkin("EnviromentPresetList","Skin9","Listbox: Scrollbar")
	ChangeSkin("EnviromentPresetList","Skin10","Listbox: Scrollbar: Button up")
	ChangeSkin("EnviromentPresetList","Skin2","Listbox: Scrollbar: Button down")
	ChangeSkin("EnviromentPresetList","Skin2","Listbox: Scrollbar: Button focus")
	ChangeSkin("EnviromentPresetList","Skin11","Listbox: Scrollbar: Top: Button up")
	ChangeSkin("EnviromentPresetList","Skin12","Listbox: Scrollbar: Top: Button down")
	ChangeSkin("EnviromentPresetList","Skin11","Listbox: Scrollbar: Top: Button focus")
	ChangeSkin("EnviromentPresetList","Skin13","Listbox: Scrollbar: Bottom: Button up")
	ChangeSkin("EnviromentPresetList","Skin14","Listbox: Scrollbar: Bottom: Button down")
	ChangeSkin("EnviromentPresetList","Skin13","Listbox: Scrollbar: Bottom: Button focus")

	CreateWnd(1,"OpenAllPropertyWnd","PropertyPage","+",224,38,16,16,0,0,0)
	ChangeSkin("OpenAllPropertyWnd","Skin2","Button up")
	ChangeSkin("OpenAllPropertyWnd","Skin2","Button down")
	ChangeSkin("OpenAllPropertyWnd","Skin2","Button focus")

	CreateWnd(1,"OpenEditPropertyWnd","PropertyPage","?",224,64,16,16,0,0,0)
	ChangeSkin("OpenEditPropertyWnd","Skin2","Button up")
	ChangeSkin("OpenEditPropertyWnd","Skin2","Button down")
	ChangeSkin("OpenEditPropertyWnd","Skin2","Button focus")

	CreateWnd(5,"PropertyList","PropertyPage","",10,20,200,200,0,0,0)
	ChangeSkin("PropertyList","Skin5","Listbox")
	ChangeSkin("PropertyList","Skin6","Listbox: Button up")
	ChangeSkin("PropertyList","Skin7","Listbox: Button down")
	ChangeSkin("PropertyList","Skin8","Listbox: Button focus")
	ChangeSkin("PropertyList","Skin9","Listbox: Scrollbar")
	ChangeSkin("PropertyList","Skin10","Listbox: Scrollbar: Button up")
	ChangeSkin("PropertyList","Skin2","Listbox: Scrollbar: Button down")
	ChangeSkin("PropertyList","Skin2","Listbox: Scrollbar: Button focus")
	ChangeSkin("PropertyList","Skin11","Listbox: Scrollbar: Top: Button up")
	ChangeSkin("PropertyList","Skin12","Listbox: Scrollbar: Top: Button down")
	ChangeSkin("PropertyList","Skin11","Listbox: Scrollbar: Top: Button focus")
	ChangeSkin("PropertyList","Skin13","Listbox: Scrollbar: Bottom: Button up")
	ChangeSkin("PropertyList","Skin14","Listbox: Scrollbar: Bottom: Button down")
	ChangeSkin("PropertyList","Skin13","Listbox: Scrollbar: Bottom: Button focus")

end

-- Mistlands GUI Resource file, END
