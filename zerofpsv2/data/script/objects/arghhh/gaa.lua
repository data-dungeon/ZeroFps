timer  = {};

function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/lavaball.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Ml");
		InitProperty("P_PfPath");
		
		InitProperty("P_Tcs");
			InitParameter("radius",-1);
		SetParentObject();
		SetReturnObject();
end

function Init()
	HaveHeartbeat(GetSelfID());
	timer[GetSelfID()] = 0;

	SetLocalDouble(GetSelfID() , "GuldFiskLevel", 12.5);

	fisklevel = GetLocalDouble(GetSelfID() , "GuldFiskLevel");

	Print("FiskNivån är ", fisklevel);
end

function Use(action)
end

function Update1S()
--	timer[GetSelfID()] = timer[GetSelfID()] + 1;
--	if timer[GetSelfID()] >= 3 then
		pos = GetObjectPos(GetSelfID());
		pos[1] = pos[1] + Random(30)-15;
		pos[3] = pos[3] + Random(30)-15;
		--Print("Pos is:",pos[1]," ",pos[2]," ",pos[3]);
		MakePathFind(GetSelfID(),pos);
		timer[GetSelfID()] = 0;
--	end
end