function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/inn_roof.mad");
			InitParameter("m_fScale",1);
			InitParameter("m_bCanBeInvisible","true");
		SetParentObject();
		SetReturnObject();
end

function Init()
	SetDrawingOrder(10);
end
