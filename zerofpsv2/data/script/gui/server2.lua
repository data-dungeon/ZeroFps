-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="placeonground_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin2 = { tex1="placeonground_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin3 = { tex1="placeonground_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="rot_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="rot_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin6 = { tex1="rot_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin7 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=214, bkG=211,
	 bkB=206, borderR=92, borderG=92, borderB=92, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="lb_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	Skin9 = { tex1="lb_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin10 = { tex1="lb_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin11 = { tex1="lb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=56, borderG=56, borderB=56, bd_size=1, tile=0, trans=0 }
	Skin12 = { tex1="sb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin13 = { tex1="sb_n.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin14 = { tex1="sb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin15 = { tex1="treebox_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	Skin16 = { tex1="delete_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin17 = { tex1="delete_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin18 = { tex1="bn_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin19 = { tex1="delete_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin20 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=128, bkG=128,
	 bkB=128, borderR=92, borderG=92, borderB=92, bd_size=1, tile=0, trans=0 }
	Skin21 = { tex1="tab_bnNext_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin22 = { tex1="tab_bnNext_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin23 = { tex1="tab_bnPrev_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin24 = { tex1="tab_bnPrev_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin25 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=214, bkG=211,
	 bkB=206, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin26 = { tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"worktab","","",114,137,256,269,0)
	ChangeSkin("worktab","Skin26","Window")

	CreateWnd(9,"WorkTabWnd","worktab","",0,0,256,256,0)
	ChangeSkin("WorkTabWnd","Skin20","Tabctrl")
	ChangeSkin("WorkTabWnd","Skin21","Tabctrl: nexttab: Button up")
	ChangeSkin("WorkTabWnd","Skin22","Tabctrl: nexttab: Button down")
	ChangeSkin("WorkTabWnd","Skin21","Tabctrl: nexttab: Button focus")
	ChangeSkin("WorkTabWnd","Skin23","Tabctrl: prevtab: Button up")
	ChangeSkin("WorkTabWnd","Skin24","Tabctrl: prevtab: Button down")
	ChangeSkin("WorkTabWnd","Skin23","Tabctrl: prevtab: Button focus")
	ChangeSkin("WorkTabWnd","Skin20","Tabctrl: UnSeltab")
	ChangeSkin("WorkTabWnd","Skin25","Tabctrl: Seltab")

	CreateWnd(0,"ZonePage","WorkTabWnd","Zones",2,27,252,227,0)
	ChangeSkin("ZonePage","Skin7","Window")

	CreateWnd(1,"DeleteZoneButton","ZonePage","",224,38,16,16,0)
	ChangeSkin("DeleteZoneButton","Skin4","Button up")
	ChangeSkin("DeleteZoneButton","Skin5","Button down")
	ChangeSkin("DeleteZoneButton","Skin6","Button focus")

	CreateWnd(0,"ObjectPage","WorkTabWnd","Object",2,27,252,227,0)
	ChangeSkin("ObjectPage","Skin7","Window")

	CreateWnd(11,"ObjectTree","ObjectPage","",10,20,200,200,0)
	ChangeSkin("ObjectTree","Skin15","Treebox")
	ChangeSkin("ObjectTree","Skin12","Treebox: V.Scrollbar")
	ChangeSkin("ObjectTree","Skin13","Treebox: V.Scrollbar: Button up")
	ChangeSkin("ObjectTree","Skin14","Treebox: V.Scrollbar: Button down")
	ChangeSkin("ObjectTree","Skin14","Treebox: V.Scrollbar: Button focus")
	ChangeSkin("ObjectTree","Skin12","Treebox: H.Scrollbar")
	ChangeSkin("ObjectTree","Skin13","Treebox: H.Scrollbar: Button up")
	ChangeSkin("ObjectTree","Skin14","Treebox: H.Scrollbar: Button down")
	ChangeSkin("ObjectTree","Skin14","Treebox: H.Scrollbar: Button focus")

	CreateWnd(1,"PlaceongroundButton","ObjectPage","",224,12,16,16,0)
	ChangeSkin("PlaceongroundButton","Skin16","Button up")
	ChangeSkin("PlaceongroundButton","Skin17","Button down")
	ChangeSkin("PlaceongroundButton","Skin18","Button focus")

	CreateWnd(1,"RotateZoneModellButton","ZonePage","",224,12,16,16,0)
	ChangeSkin("RotateZoneModellButton","Skin16","Button up")
	ChangeSkin("RotateZoneModellButton","Skin19","Button down")
	ChangeSkin("RotateZoneModellButton","Skin17","Button focus")

	CreateWnd(11,"ZoneModelTree","ZonePage","",10,20,200,200,0)
	ChangeSkin("ZoneModelTree","Skin15","Treebox")
	ChangeSkin("ZoneModelTree","Skin12","Treebox: V.Scrollbar")
	ChangeSkin("ZoneModelTree","Skin13","Treebox: V.Scrollbar: Button up")
	ChangeSkin("ZoneModelTree","Skin14","Treebox: V.Scrollbar: Button down")
	ChangeSkin("ZoneModelTree","Skin14","Treebox: V.Scrollbar: Button focus")
	ChangeSkin("ZoneModelTree","Skin12","Treebox: H.Scrollbar")
	ChangeSkin("ZoneModelTree","Skin13","Treebox: H.Scrollbar: Button up")
	ChangeSkin("ZoneModelTree","Skin14","Treebox: H.Scrollbar: Button down")
	ChangeSkin("ZoneModelTree","Skin14","Treebox: H.Scrollbar: Button focus")

	CreateWnd(1,"DeleteObjectButton	","ObjectPage","",224,38,16,16,0)
	ChangeSkin("DeleteObjectButton	","Skin1","Button up")
	ChangeSkin("DeleteObjectButton	","Skin2","Button down")
	ChangeSkin("DeleteObjectButton	","Skin3","Button focus")

	CreateWnd(0,"EnviromentPage","WorkTabWnd","Enviroment",2,27,252,227,0)
	ChangeSkin("EnviromentPage","Skin7","Window")

	CreateWnd(5,"EnviromentPresetList","EnviromentPage","",10,9,200,200,0)
	ChangeSkin("EnviromentPresetList","Skin8","Listbox")
	ChangeSkin("EnviromentPresetList","Skin9","Listbox: Button up")
	ChangeSkin("EnviromentPresetList","Skin10","Listbox: Button down")
	ChangeSkin("EnviromentPresetList","Skin11","Listbox: Button focus")
	ChangeSkin("EnviromentPresetList","Skin12","Listbox: Scrollbar")
	ChangeSkin("EnviromentPresetList","Skin13","Listbox: Scrollbar: Button up")
	ChangeSkin("EnviromentPresetList","Skin14","Listbox: Scrollbar: Button down")
	ChangeSkin("EnviromentPresetList","Skin14","Listbox: Scrollbar: Button focus")

end

-- Mistlands GUI Resource file, END
