function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/cube.mad");
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
			InitParameter("friction",0.5);
			InitParameter("activemoment","true");
			
		SetParentObject();
		SetReturnObject();
		
end

