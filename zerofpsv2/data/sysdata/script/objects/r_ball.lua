function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/physicsball.mad");
			--InitParameter("m_kMadFile","/data/mad/dm/dm_barrel.mad");
			InitParameter("m_fScale",1);
			InitParameter("m_iShadowGroup","2");
--		InitProperty("P_Primitives3D");

		InitProperty("P_Tcs");
			--InitParameter("polygontest","true");
			InitParameter("radius",-1);
			InitParameter("gravity","true");
			InitParameter("group",1);
			InitParameter("groupflag",0);
			InitParameter("groupflag",1);			
			InitParameter("airfriction",0.5);
			InitParameter("mass",1);
			--InitParameter("inertia",0.16666);
			InitParameter("inertia",2);
			InitParameter("bounce",0.8);
			InitParameter("friction",0.8);
--			InitParameter("activemoment","false");
			
		SetParentObject();
		SetReturnObject();
		
end

