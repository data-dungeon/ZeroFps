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
	Skin4 = { tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	Skin5 = { tex1="misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin6 = { tex1="misc_button_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin7 = { tex1="final/big_corner.bmp", tex2="0", tex3="0", tex4="0", tex1a="final/big_corner_a.bmp", tex2a="0",
	 tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="wnd_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=25, bkB=150, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=1, trans=0 }
	Skin9 = { tex1="misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=25, bkB=25, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin10 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin11 = { tex1="lb_bk.bmp", tex2="wndborder_left.bmp", tex3="wndborder_top.bmp", tex4="wndborder_corner.bmp",
	 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=8,
	 tile=1, trans=0 }
	Skin12 = { tex1="lb_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin13 = { tex1="lb_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin14 = { tex1="lb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=56, borderG=56, borderB=56, bd_size=1, tile=0, trans=0 }
	Skin15 = { tex1="sb_bk.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255,
	 bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=1, trans=0 }
	Skin16 = { tex1="sb_n.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin17 = { tex1="sb_f.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin18 = { tex1="scrollbar_clicktop_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin19 = { tex1="scrollbar_clicktop_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin20 = { tex1="scrollbar_clickbottom_bn_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin21 = { tex1="scrollbar_clickbottom_bn_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0",
	 tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin22 = { tex1="misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=192,
	 bkG=192, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"ConnectWnd","","",225,97,353,380,0,6,0)
	ChangeSkin("ConnectWnd","Skin4","Window")

	CreateWnd(0,"MLStartWnd","","",0,0,800,600,0,0,3)
	ChangeSkin("MLStartWnd","Skin8","Window")

	CreateWnd(1,"QuitBn","MLStartWnd","Quit",357,374,96,40,0,4,0)
	ChangeSkin("QuitBn","Skin9","Button up")
	ChangeSkin("QuitBn","Skin6","Button down")
	ChangeSkin("QuitBn","Skin5","Button focus")

	CreateWnd(1,"RemoveServerBn","ConnectWnd","Remove",247,61,98,37,0,0,0)
	ChangeSkin("RemoveServerBn","Skin1","Button up")
	ChangeSkin("RemoveServerBn","Skin2","Button down")
	ChangeSkin("RemoveServerBn","Skin3","Button focus")

	CreateWnd(4,"ServerLabel","ConnectWnd","Server List",8,8,226,20,0,0,0)
	ChangeSkin("ServerLabel","Skin10","Label")

	CreateWnd(5,"ServerList","ConnectWnd","",16,43,211,266,0,0,0)
	ChangeSkin("ServerList","Skin11","Listbox")
	ChangeSkin("ServerList","Skin12","Listbox: Button up")
	ChangeSkin("ServerList","Skin13","Listbox: Button down")
	ChangeSkin("ServerList","Skin14","Listbox: Button focus")
	ChangeSkin("ServerList","Skin15","Listbox: Scrollbar")
	ChangeSkin("ServerList","Skin16","Listbox: Scrollbar: Button up")
	ChangeSkin("ServerList","Skin17","Listbox: Scrollbar: Button down")
	ChangeSkin("ServerList","Skin17","Listbox: Scrollbar: Button focus")
	ChangeSkin("ServerList","Skin18","Listbox: Scrollbar: Top: Button up")
	ChangeSkin("ServerList","Skin19","Listbox: Scrollbar: Top: Button down")
	ChangeSkin("ServerList","Skin18","Listbox: Scrollbar: Top: Button focus")
	ChangeSkin("ServerList","Skin20","Listbox: Scrollbar: Bottom: Button up")
	ChangeSkin("ServerList","Skin21","Listbox: Scrollbar: Bottom: Button down")
	ChangeSkin("ServerList","Skin20","Listbox: Scrollbar: Bottom: Button focus")

	CreateWnd(1,"StarNewGameBn","MLStartWnd","Start",355,227,96,40,0,4,0)
	ChangeSkin("StarNewGameBn","Skin22","Button up")
	ChangeSkin("StarNewGameBn","Skin6","Button down")
	ChangeSkin("StarNewGameBn","Skin5","Button focus")

	CreateWnd(1,"AddServerBn","ConnectWnd","Add",247,15,98,37,0,0,0)
	ChangeSkin("AddServerBn","Skin1","Button up")
	ChangeSkin("AddServerBn","Skin2","Button down")
	ChangeSkin("AddServerBn","Skin3","Button focus")

	CreateWnd(1,"CloseServerWndBn","ConnectWnd","Return",245,330,91,37,0,0,0)
	ChangeSkin("CloseServerWndBn","Skin1","Button up")
	ChangeSkin("CloseServerWndBn","Skin2","Button down")
	ChangeSkin("CloseServerWndBn","Skin3","Button focus")

	CreateWnd(1,"ConnectBn","ConnectWnd","Connect",15,330,220,40,0,0,0)
	ChangeSkin("ConnectBn","Skin1","Button up")
	ChangeSkin("ConnectBn","Skin2","Button down")
	ChangeSkin("ConnectBn","Skin3","Button focus")

	CreateWnd(1,"ContinueGameBn","MLStartWnd","Continue",692,548,96,40,0,3,0)
	ChangeSkin("ContinueGameBn","Skin5","Button up")
	ChangeSkin("ContinueGameBn","Skin6","Button down")
	ChangeSkin("ContinueGameBn","Skin5","Button focus")

	CreateWnd(1,"CreditsBn","MLStartWnd","Credits",356,325,96,40,0,4,0)
	ChangeSkin("CreditsBn","Skin5","Button up")
	ChangeSkin("CreditsBn","Skin6","Button down")
	ChangeSkin("CreditsBn","Skin5","Button focus")

	CreateWnd(4,"Label41","MLStartWnd","",309,184,151,150,0,4,0)
	ChangeSkin("Label41","Skin7","Label")

	CreateWnd(1,"LoadNewGameBn","MLStartWnd","Options",355,277,96,40,0,4,0)
	ChangeSkin("LoadNewGameBn","Skin5","Button up")
	ChangeSkin("LoadNewGameBn","Skin6","Button down")
	ChangeSkin("LoadNewGameBn","Skin5","Button focus")

	ShowWnd("ConnectWnd",0,0)

end

-- Mistlands GUI Resource file, END
