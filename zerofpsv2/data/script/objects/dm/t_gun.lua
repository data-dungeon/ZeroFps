function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_handgun.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_DMItem");	
			InitParameter("name","gun");
			InitParameter("sizex",2);
			InitParameter("sizey",1);
			InitParameter("icon","handgun.bmp");
			InitParameter("type","5");
		InitProperty("P_DMGun");
		InitProperty("P_ScriptInterface");


end

function Init()
	SetGunSound (SIGetSelfID(), "data/sound/9m_pistol.wav");
	SetGunFireRate (SIGetSelfID(), 1);
	SetGunDamage (SIGetSelfID(), 35);
	SetGunRange (SIGetSelfID(), 30);
	SetGunRandom (SIGetSelfID(), 0.1);
	SetGunMaxAmmo (SIGetSelfID(), 70);
	SetGunAmmo (SIGetSelfID(), 70);
end

function FirstRun()
end

function HeartBeat()

end
