function Create()
	
	InitObject();
		InitProperty("P_Light");
			InitParameter("Mode",0);
			InitParameter("Quadratic_Atten",0.05);
		InitProperty("P_PSystem");
			InitParameter("PSType", "big_explosion");
		InitProperty("P_ScriptInterface");
		InitProperty("P_Sound");
		SetParentObject();
		SetReturnObject();
end


function Init()
	PlaySound (SIGetSelfID(), "expl.wav");
	Explosion (SIGetSelfID(), 3, 300);
	PanicArea(SIGetSelfID(), 15);
end


function HeartBeat()
	if GetEntityVar(SIGetSelfID(), "activated") == 1 then
		PlaySound (SIGetSelfID(), "beep1.wav");
		AddToEntityVarLua (SIGetSelfID(), "timer", -1);

		if GetEntityVar(SIGetSelfID(), "timer") < 1 then
			Delete ( SIGetSelfID() );
		end
	end
end

function Activate()
	SetEntityVar (SIGetSelfID(), "activated", 1);
	SetEntityVar (SIGetSelfID(), "timer", 10);
	SISetHeartRate(SIGetSelfID(),1);
end