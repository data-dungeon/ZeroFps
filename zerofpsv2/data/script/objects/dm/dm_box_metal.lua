function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dmbox.mad");
			InitParameter("m_fScale","1.0");
		InitProperty("P_Event");
		InitProperty("P_Item");
		
		InitProperty("P_Tcs");
			InitParameter("group",4);		
			InitParameter("polygontest","true");
			InitParameter("static","true");			
		
		SetParentObject();
		SetReturnObject();
end

