function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/wolf.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_CharStats");

	InitProperty("P_PfPath");
	InitProperty("P_AI");


		SetParentObject();
		SetReturnObject();

	InitObject("/data/script/objects/characters/t_gobrider.lua");
		AttachToParent();
			InitProperty("P_LinkToJoint");
			InitParameter("m_strToJoint","rider");
end

function FirstRun()

	SetHeartRate(GetSelfID(), 3);
	SetMoveSpeed(8.7);
	SetReloadTime(6);

end
function Init()
	PlayAnim(GetSelfID(), "idle")
end

	
function HeartBeat()
	if HavePath(GetSelfID()) then
		return
	end

	
	pos = GetObjectPos(GetSelfID());
	pos[1] = pos[1] + Random(32)-16;
	pos[3] = pos[3] + Random(32)-16;

	AIMoveTo(GetSelfID(),pos);	
end




