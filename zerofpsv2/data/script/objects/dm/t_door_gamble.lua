function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/door_gamble.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_DMClickMe");	
		InitProperty("P_ScriptInterface");
		InitProperty("P_Sound");


end


function Click(ID)
	if Money() > 50 then
		PlaySound (SIGetSelfID(), "dooropen.wav");
		PlaySound (SIGetSelfID(), "dice2.wav");
		PlayAnim(SIGetSelfID(), "open");
		SetNextAnim(SIGetSelfID(), "idle");
		SwallowPlayer (SIGetSelfID(), ID, 4);

		if Random(30) > 15 then
			AddMoney(150);
			PlaySound (SIGetSelfID(), "coin_short1.wav");
		end
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
