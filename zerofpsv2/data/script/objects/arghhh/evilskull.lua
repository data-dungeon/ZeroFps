function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/skull.mad");
			InitParameter("m_fScale","0.7");
		InitProperty("P_Event");
		InitProperty("P_Ml");
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Look");
	SetHeartRate(GetSelfID(),1);
	
end

function Action()
	if action == "Look" then		
		MessageCaracter(GetCurrentPCID(),"The human skull stares back at you.");
	end
end


function HeartBeat()
	RunScript("data/script/objects/misc/t_gibb.lua",GetSelfID());	
	
	
	player = GetClosestPlayer();
	
	if player == -1 then
		return
	end
	
	RotateTowards(GetSelfID(),GetObjectPos(player));

end