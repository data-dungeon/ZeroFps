function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/physicsball.mad");
			InitParameter("m_fScale",1);
			InitParameter("m_iShadowGroup","2");
--		InitProperty("P_Primitives3D");

		InitProperty("P_Car");
		InitProperty("P_Tcs");
			--InitParameter("polygontest","true");
			InitParameter("radius",-1);
			InitParameter("gravity","true");
			InitParameter("group",1);
			InitParameter("groupflag",0);
			InitParameter("groupflag",1);			
			InitParameter("airfriction",0.01);
			InitParameter("mass",0.5);
			InitParameter("inertia",0.3);
			InitParameter("bounce",0.8);
			InitParameter("friction",0.8);
			InitParameter("activemoment","true");
			
		SetParentObject();
		SetReturnObject();
		
end

