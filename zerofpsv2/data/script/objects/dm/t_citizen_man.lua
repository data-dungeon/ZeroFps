function Create()
	
	InitObject();
		InitProperty("P_Event");
		InitProperty("P_Mad");													InitParameter("m_kMadFile","/data/mad/citizen_man.mad");
			InitParameter("m_fScale","1");
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");
		InitProperty("P_DMCharacter");
		InitProperty("P_DMGun");
		SetParentObject();
		SetReturnObject();

end

function FirstRun()
	SISetHeartRate(SIGetSelfID(),2);
end

function Init()
	SISetHeartRate(SIGetSelfID(),2);
end

function HeartBeat()
	iClosest = GetDMCharacterClosest(SIGetSelfID());


	if iClosest > 0 then
		FireAtObject (SIGetSelfID(), iClosest);
	end		
end
