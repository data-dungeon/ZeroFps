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
	if Money() > 19 then
		PlaySound (SIGetSelfID(), "dooropen.wav");
		AddMoney (-20);
		Heal(ID);
		PlayAnim(SIGetSelfID(), "open");
		SetNextAnim(SIGetSelfID(), "idle");
		SwallowPlayer (SIGetSelfID(), ID, 5);
	else
		PlaySound (SIGetSelfID(), "door_closed.wav");
	end
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
