function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/member.mad");	
--			InitParameter("m_fScale","0.165");		

		InitProperty("P_DMGun");
		InitProperty("P_PfPath");
		InitProperty("P_Track");
		InitProperty("P_DMCharacter");
			InitParameter("team",0)
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");

end

function FirstRun()
	SISetHeartRate(SIGetSelfID(),1);
end

function HeartBeat()
--	Print("Dudunk");

end

function Dead()
	Print("IM DEAD!!!");
	Delete(SIGetSelfID());
end
