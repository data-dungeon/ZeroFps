function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/inn_walls.mad");
			InitParameter("m_fScale",1);
		
--		InitProperty("P_Physic");
--			InitParameter("m_fRadius",8);
--			InitParameter("m_fColShape",3);
		
		InitProperty("P_Tcs");
			InitParameter("polygontest","true");

		
		SetParentObject();
		SetReturnObject();
end

function Init()
end
