function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/fwarrior.mad");			
			InitParameter("m_fScale","0.165");		

		InitProperty("P_PfPath");
		InitProperty("P_Track");
		InitProperty("P_DMCharacter");
		InitProperty("P_ScriptInterface");
		

end

function FirstRun()
	Print("BLUB!!!");	
	SISetHeartRate(SIGetSelfID(),1);
end

function HeartBeat()
	Print("Dudunk");

end