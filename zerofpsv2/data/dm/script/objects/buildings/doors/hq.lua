function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_door.mad");
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

	
end

function FirstRun()
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/gun.lua", 300);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/gun.lua", 300);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/shotgun_clip.lua", 50);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/handgun_clip.lua", 25);	
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/shotgun.lua", 800);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/shotgun.lua", 800);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/rifle.lua", 1500);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/rifle.lua", 1500);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/rifle_clip.lua", 100);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/rifle_clip.lua", 100);

	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/mine.lua", 100);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/mine.lua", 100);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/weapons/mine.lua", 100);

	AddItemToShop(SIGetSelfID(), "/data/script/objects/pickup gears/medkit.lua", 300);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/pickup gears/medkit.lua", 300);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/pickup gears/medkit.lua", 300);

	AddItemToShop(SIGetSelfID(), "/data/script/objects/armours/armour.lua", 1700);
	AddItemToShop(SIGetSelfID(), "/data/script/objects/armours/armour.lua", 1700);
end

function HeartBeat()

end