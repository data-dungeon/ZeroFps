function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/cube.mad");
			--InitParameter("m_kMadFile","/data/mad/p_barrel.mad");
			--InitParameter("m_kMadFile","/data/mad/physicsball.mad");
			--InitParameter("m_kMadFile","/data/mad/dm/dm_barrel.mad");
			InitParameter("m_fScale",1);
			InitParameter("m_iShadowGroup","2");

		InitProperty("P_Tcs");
			InitParameter("polygontest","true");
			InitParameter("radius",-1);
			InitParameter("gravity","true");
			InitParameter("group",1);
			InitParameter("groupflag",0);
			InitParameter("groupflag",1);			
			InitParameter("airfriction",0.1);
			InitParameter("mass",1);
			InitParameter("inertia",1);
			InitParameter("bounce",0.0);
			InitParameter("friction",1);
--			InitParameter("activemoment","false");
			
		SetParentObject();
		SetReturnObject();
		
end

