function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/chest.mad");
			InitParameter("m_fScale","1");
		InitProperty("P_Event");
		InitProperty("P_Container");
		InitProperty("P_Ml");

		SetParentObject();
		SetReturnObject();
end

function Init()
	
	SetContainerSize ("50");
	AddAction(GetSelfID(),"Open");

	id_apple = RunScript("data/script/objects/misc/t_apple.lua")
	id_meat = RunScript("data/script/objects/misc/t_meat.lua")
	
	PutInContainer( GetSelfID(), id_apple )
	PutInContainer( GetSelfID(), id_meat )
	
end
