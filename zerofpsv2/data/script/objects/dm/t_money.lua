function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/money.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_DMClickMe");
		InitProperty("P_ScriptInterface");
		InitProperty("P_Sound");


end

function Click()	
	PlaySound (SIGetSelfID(), "coin_short1.wav")
	AddMoney ( Random(100) );
	Delete (SIGetSelfID());
end

function Init()
end

function FirstRun()
end
