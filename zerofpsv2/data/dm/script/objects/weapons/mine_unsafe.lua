function Create()

	

	InitObject();

		InitProperty("P_Mad");	

			InitParameter("m_kMadFile","/data/mad/dm/dm_grenade.mad");			

			InitParameter("m_fScale","1.0");		

		InitProperty("P_ScriptInterface");

		InitProperty("P_Sound");



end



function Init()

	SetEntityVar(SIGetSelfID(), "Timer", 5);

	SISetHeartRate(SIGetSelfID(), 1);	

end



function FirstRun()

end



function HeartBeat()

	local ID = SIGetSelfID();

	AddToEntityVar(ID, "Timer", -1);



	PlaySound (ID, "computer beep 5.wav");



	if GetEntityVar(ID, "Timer") == 0 then

		RunScript("data/script/objects/t_explosion.lua",ID);		

		Delete(ID);

	end

end

