function Create()
	
	InitObject();
		InitProperty("P_Event");
		InitProperty("P_Mad");							InitParameter("m_kMadFile","/data/mad/citizen_man.mad");
			InitParameter("m_fScale","1");
		SetParentObject();
		SetReturnObject();
end

function Init()
	PlayAnim(GetSelfID(), "walk");
end


