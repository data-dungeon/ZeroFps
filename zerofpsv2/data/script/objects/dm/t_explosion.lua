function Create()
	
	InitObject();
		InitProperty("P_Light");
			InitParameter("Mode",0);
			InitParameter("Quadratic_Atten",0.05);
		InitProperty("P_PSystem");
			InitParameter("PSType", "explosion");
		InitProperty("P_ScriptInterface");
		InitProperty("P_Sound");
		SetParentObject();
		SetReturnObject();
end


function Init()
	PlaySound (SIGetSelfID(), "expl.wav");
	Explosion (SIGetSelfID(), 3, 300);
	PanicArea(SIGetSelfID(), 10);
end