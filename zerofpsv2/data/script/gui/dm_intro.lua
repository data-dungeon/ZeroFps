-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=0, bkG=0,
	 bkB=0, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin2 = { tex1="dm/intro/dvoid.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin3 = { tex1="dm/intro/zerom.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin4 = { tex1="dm/intro/vim.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin5 = { tex1="dm/intro/zeb.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin6 = { tex1="dm/intro/eld.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin7 = { tex1="dm/intro/manfred.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	Skin8 = { tex1="dm/intro/logga.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }

	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"DMIntroWnd","","",0,0,800,600,0)
	ChangeSkin("DMIntroWnd","Skin1","Window")

	CreateWnd(4,"loggaLabel","DMIntroWnd","",800/2-640/2,600/2-332/2,640,332,0)
	ChangeSkin("loggaLabel","Skin8","Label")

	CreateWnd(4,"dvoidLabel","DMIntroWnd","",300,260,256,64,0)
	ChangeSkin("dvoidLabel","Skin2","Label")

	CreateWnd(4,"zeromLabel","DMIntroWnd","",144,172,512,256,0)
	ChangeSkin("zeromLabel","Skin3","Label")

	CreateWnd(4,"vimLabel","DMIntroWnd","",300,260,256,64,0)
	ChangeSkin("vimLabel","Skin4","Label")

	CreateWnd(4,"zebLabel","DMIntroWnd","",300,260,256,64,0)
	ChangeSkin("zebLabel","Skin5","Label")

	CreateWnd(4,"eldLabel","DMIntroWnd","",300,260,256,64,0)
	ChangeSkin("eldLabel","Skin6","Label")

	CreateWnd(4,"manfredLabel","DMIntroWnd","",300,260,256,64,0)
	ChangeSkin("manfredLabel","Skin7","Label")

end

-- Mistlands GUI Resource file, END
