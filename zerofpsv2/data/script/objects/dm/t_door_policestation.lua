function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_door.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_DMClickMe");	
		InitProperty("P_ScriptInterface");
		InitProperty("P_Sound");


end


function Click(ID)
end

function Exit()
	PlayAnim(SIGetSelfID(), "open");
	SetNextAnim(SIGetSelfID(), "idle");
end

function Init()
	SetIsHouse (SIGetSelfID(), 1);
end

function FirstRun()
end
