function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/tree3.mad");
			InitParameter("m_fScale","1");
			InitParameter("m_bCanBeInvisible","true");
--		InitProperty("P_Event");
		InitProperty("P_Tcs");
			InitParameter("group",4);		
			InitParameter("polygontest","true");
		
		
		SetParentObject();
		SetReturnObject();
end

function Init()
end
