function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/cube.mad");
			InitParameter("m_fScale","1");		

		InitProperty("P_Track");
		InitProperty("P_DMHQ");
			InitParameter("active","true");					
		InitProperty("P_ScriptInterface");
		InitProperty("P_DMShop");
			InitParameter("shopname","ZGP Agent Toy Land");
			InitParameter("traderate",1.0);					
		

end

function Init()
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_gun.lua", 300);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_gun.lua", 300);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_shotgun.lua", 800);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_shotgun.lua", 800);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_rifle.lua", 1500);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_rifle.lua", 1500);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_rifle_clip.lua", 100);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_rifle_clip.lua", 100);

	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_mine.lua", 100);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_mine.lua", 100);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_mine.lua", 100);

	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_medkit.lua", 300);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_medkit.lua", 300);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_medkit.lua", 300);

	
end

function FirstRun()

end

function HeartBeat()

end