function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/wolf.mad");
--			InitParameter("m_kMadFile","/data/mad/lavaball.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_CharStats");
		InitProperty("P_PfPath");	
			InitParameter("Tilt","true");
		InitProperty("P_Ml");				
		SetParentObject();
		SetReturnObject();
end

function Init()
	PlayAnim(GetSelfID(), "static")

	--Using realy slow heartrate for wild animals;
	SetHeartRate(GetSelfID(),10);
end

function HeartBeat()
	--if HavePath(GetSelfID()) then
	--	return;
	--end

	--pos = GetObjectPos(GetSelfID());
	--pos[1] = pos[1] + Random(32)-16;
	--pos[3] = pos[3] + Random(32)-16;
	
	--MakePathFind(GetSelfID(),pos);
end