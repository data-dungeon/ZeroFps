function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_machinegun.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_DMItem");	
			InitParameter("name","rifle");
			InitParameter("sizex",3);
			InitParameter("sizey",2);
			InitParameter("icon","machinegun.bmp");
			InitParameter("type","5");
		InitProperty("P_DMGun");
		InitProperty("P_ScriptInterface");


end

function Init()
	SetGunSound (SIGetSelfID(), "data/sound/auto_gun.wav");
	SetGunFireRate (SIGetSelfID(), 18);
	SetGunDamage (SIGetSelfID(), 15);
	SetGunRange (SIGetSelfID(), 25);
	SetGunRandom (SIGetSelfID(), 5);
	SetGunMaxAmmo (SIGetSelfID(), 200);
	SetGunAmmo (SIGetSelfID(), 200);
	SetGunShootAnim (SIGetSelfID(), "fire_rifle");
	SetGunBurstLenght(SIGetSelfID(), 0.85);
end

function FirstRun()
end

function HeartBeat()

end