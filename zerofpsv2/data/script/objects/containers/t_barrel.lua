function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/barrel.mad");
			InitParameter("m_fScale","1");
		InitProperty("P_Event");
		InitProperty("P_Item");
		SetParentObject();
		SetReturnObject();
end

function Init()
	SetContainerSize ("15");
end
