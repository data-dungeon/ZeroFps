function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/iron_bar.mad");			
			InitParameter("m_fScale","0.01");		
		InitProperty("P_DMItem");	
			InitParameter("name","Iron bar");
			InitParameter("sizex",3);
			InitParameter("sizey",1);
			InitParameter("icon","handgun.bmp");
			InitParameter("type","5");
		InitProperty("P_DMGun");
		InitProperty("P_ScriptInterface");


end

function Init()
	SetGunSound (SIGetSelfID(), "data/sound/hit.wav");
	SetGunFireRate (SIGetSelfID(), 1);
	SetGunDamage (SIGetSelfID(), 10);
	SetGunRange (SIGetSelfID(), 2);
	SetGunRandom (SIGetSelfID(), 1);
	SetGunMaxAmmo (SIGetSelfID(), 9999999);
	SetGunAmmo (SIGetSelfID(), 9999999);
	SetGunShootAnim (SIGetSelfID(), "unarmed");
end

function FirstRun()
end

function HeartBeat()

end
