function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/gun.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_DMItem");	
			InitParameter("name","gun");
			InitParameter("sizex",1);
			InitParameter("sizey",2);
			InitParameter("icon","stol.bmp");
			InitParameter("type","5");
		InitProperty("P_DMGun");
		InitProperty("P_ScriptInterface");


end

function Init()
	SetGunSound (SIGetSelfID(), "data/sound/9m_pistol.wav");
	SetGunFireRate (SIGetSelfID(), 1);
	SetGunDamage (SIGetSelfID(), 15);
	SetGunRange (SIGetSelfID(), 30);
	SetGunRandom (SIGetSelfID(), 0.1);
	SetGunMaxAmmo (SIGetSelfID(), 7);
	SetGunAmmo (SIGetSelfID(), 7);
end

function FirstRun()
end

function HeartBeat()

end
