function Create()

	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/cube.mad");
			InitParameter("m_fScale",1);
			InitProperty("P_Body");
			InitParameter("m_bPolygonCheck","true");			
end
