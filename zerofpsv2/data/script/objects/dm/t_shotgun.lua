function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/shotgun.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_DMItem");	
			InitParameter("name","shotgun");
			InitParameter("sizex",3);
			InitParameter("sizey",2);
			InitParameter("icon","shotgun.bmp");
			InitParameter("type","5");
		InitProperty("P_DMGun");
		InitProperty("P_ScriptInterface");


end

function Init()
	SetGunSound (SIGetSelfID(), "data/sound/12ga_shotgun.wav");
	SetGunFireRate (SIGetSelfID(), 0.5);
	SetGunBulletsPerAmmo(SIGetSelfID(), 10);
	SetGunDamage (SIGetSelfID(), 15);
	SetGunRange (SIGetSelfID(), 10);
	SetGunRandom (SIGetSelfID(), 20);
	SetGunMaxAmmo (SIGetSelfID(), 10);
	SetGunAmmo (SIGetSelfID(), 10);
	SetGunShootAnim(SIGetSelfID(), "fire_shotgun");
end

function FirstRun()
end

function HeartBeat()

end