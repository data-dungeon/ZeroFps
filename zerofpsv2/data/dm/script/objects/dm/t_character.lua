function Create()
		InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/member.mad");
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

	--keep near mainAgent
	mainAgentID = GetVar("MainAgent");
	selfID = SIGetSelfID();

	if DistanceTo(selfID, mainAgentID) > 3 then
		Print("Tjoff");
		agent_pos = GetEntityPos(mainAgentID);
		agent_pos[1] = agent_pos[1] + Random(3)-1.5;
		agent_pos[3] = agent_pos[3] + Random(3)-1.5;
		MakePathFind(selfID,agent_pos);
	end
	
end

function Dead()
	Print("IM DEAD!!!");
	Delete(SIGetSelfID());
end