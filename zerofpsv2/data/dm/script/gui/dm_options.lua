-- Mistlands GUI Resource file, START

function GUICreate()

	-------------------------------------------------------------------------
	-- Skins.
	-------------------------------------------------------------------------
	Skin1 = { tex1="final/win_tile.bmp", tex2="final/window/win_edge_ver.bmp",
	 tex3="final/window/win_edge_hor.bmp", tex4="final/window/dm_win_corn.bmp", tex1a="0", tex2a="final/window/win_edge_ver_a.bmp",
	 tex3a="final/window/win_edge_hor_a.bmp", tex4a="final/window/dm_win_corn_a.bmp", bkR=255, bkG=255, bkB=255, borderR=255,
	 borderG=255, borderB=255, bd_size=32, tile=1, trans=0 }
	Skin2 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=1 }
	Skin3 = { tex1="0", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=64, bkG=64, bkB=64,
	 borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin4 = { tex1="misc_button_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin5 = { tex1="misc_button_d.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin6 = { tex1="lb_u.bmp", tex2="0", tex3="0", tex4="0", tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
	 bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin7 = { tex1="slider.bmp", tex2="0", tex3="0", tex4="0", tex1a="slider_a.bmp", tex2a="0", tex3a="0", tex4a="0",
	 bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0 }
	Skin8 = { tex1="final/members/x-button.bmp", tex2="0", tex3="0", tex4="0", tex1a="final/members/x-button_a.bmp",
	 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0,
	 trans=0 }
	Skin9 = { tex1="final/members/x-button_d.bmp", tex2="0", tex3="0", tex4="0",
	 tex1a="final/members/x-button_a.bmp", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0,
	 tile=0, trans=0 }
	-------------------------------------------------------------------------
	-- Create all windows in correct order (parents before childrens etc.)
	-- 0 = Wnd, 1 = Button, 2 = Checkbox, 3 = Combobox, 4 = Label, 5 = Listbox, 6 = Radiobutton,
	-- 7 = Scrollbar, 8 = Slider, 9 = TabControl, 10 = Textbox, 11 = Treebox
	-------------------------------------------------------------------------
	CreateWnd(0,"DMOptionsWnd","","",109,188,225,186,0,6,0)
	ChangeSkin("DMOptionsWnd","Skin1","Window")

	CreateWnd(4,"MusicVolumeLabel","DMOptionsWnd","Music volume",10,12,150,23,0,0,0)
	ChangeSkin("MusicVolumeLabel","Skin3","Label")

	CreateWnd(1,"OptionsCloseBn","DMOptionsWnd","",225-32,0,32,32,0,0,0)
	ChangeSkin("OptionsCloseBn","Skin8","Button up")
	ChangeSkin("OptionsCloseBn","Skin9","Button down")
	ChangeSkin("OptionsCloseBn","Skin8","Button focus")

	CreateWnd(4,"SFXVolumeLabel","DMOptionsWnd","SFX Volume",10,69,150,23,0,0,0)
	ChangeSkin("SFXVolumeLabel","Skin3","Label")

	CreateWnd(8,"MusicVolumeSlider","DMOptionsWnd","",12,35,150,32,0,0,0)
	ChangeSkin("MusicVolumeSlider","Skin6","Slider: Label")
	ChangeSkin("MusicVolumeSlider","Skin7","Slider: Button up")
	ChangeSkin("MusicVolumeSlider","Skin7","Slider: Button down")
	ChangeSkin("MusicVolumeSlider","Skin7","Slider: Button focus")

	CreateWnd(8,"SFXVolumeSlider","DMOptionsWnd","",11,91,150,32,0,0,0)
	ChangeSkin("SFXVolumeSlider","Skin6","Slider: Label")
	ChangeSkin("SFXVolumeSlider","Skin7","Slider: Button up")
	ChangeSkin("SFXVolumeSlider","Skin7","Slider: Button down")
	ChangeSkin("SFXVolumeSlider","Skin7","Slider: Button focus")

end

-- Mistlands GUI Resource file, END
