function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/rifle.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_DMItem");	
			InitParameter("name","rifle");
			InitParameter("sizex",2);
			InitParameter("sizey",2);
			InitParameter("icon","stol.bmp");
			InitParameter("type","5");
		InitProperty("P_DMGun");
		InitProperty("P_ScriptInterface");


end

function Init()
	SetGunSound (SIGetSelfID(), "data/sound/auto_gun.wav");
	SetGunFireRate (SIGetSelfID(), 20);
	SetGunBurstLenght (SIGetSelfID(), 1);
	SetGunDamage (SIGetSelfID(), 15);
	SetGunRange (SIGetSelfID(), 25);
	SetGunRandom (SIGetSelfID(), 0.3);
	SetGunMaxAmmo (SIGetSelfID(), 100);
	SetGunAmmo (SIGetSelfID(), 100);
end

function FirstRun()
end

function HeartBeat()

end