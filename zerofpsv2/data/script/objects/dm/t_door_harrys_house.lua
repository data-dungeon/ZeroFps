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
	
end

function Exit()
	-- harry has exited his house and brought with him some new goons
	Baldies = {};

	for i = 0, 5, 1
	do
		Baldies[i] = RunScript("data/script/objects/dm/t_baldie.lua");
		SetState(Baldies[i], 4); --aggro
		local doorpos = GetEntityPos(SIGetSelfID());
		SetObjectPos (Baldies[i], doorpos);
	end

	SetVar("HarryWounded", 1);

	PlayAnim(SIGetSelfID(), "open");
	SetNextAnim(SIGetSelfID(), "idle");

end

function Init()
	SetIsHouse (SIGetSelfID(), 1);
end

function FirstRun()
end
