function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/bunny.mad");
		InitProperty("P_Event");
		InitProperty("P_CharStats");
		InitProperty("P_Ml");
		InitProperty("P_PfPath");
		InitProperty("P_AI");
		
				SetParentObject();
		SetReturnObject();
end

function Init()
	PlayAnim(GetSelfID(), "idle");
	AddAction(GetSelfID(),"Attack");

	SetScriptWhenHit ("/data/script/objects/psystems/t_blood.lua");
end

function FirstRun()

	SetHeartRate(GetSelfID(), 3);
	SetMoveSpeed(3);
	SetReloadTime(3);
end

function Use(action)
	if action == "Attack" then
		AIAttack (GetSelfID(), GetCurrentPCID());
	end
end
	
function HeartBeat()
	if HavePath(GetSelfID()) then
		return
	end

	
	pos = GetObjectPos(GetSelfID());
	pos[1] = pos[1] + Random(8)-16;
	pos[3] = pos[3] + Random(8)-16;
	
	AIMoveTo(GetSelfID(),pos);	
	
end

function OnDeath()
	Delete(GetSelfID());
end