function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/dm/dm_car.mad");
			InitParameter("m_fScale","0.5");
			InitParameter("m_iShadowGroup","2");

		InitProperty("P_Car");
		InitProperty("P_Tcs");
			InitParameter("radius",-1);
			InitParameter("polygontest","true");
			InitParameter("gravity","true");
			InitParameter("group",1);
			InitParameter("groupflag",0);
			InitParameter("groupflag",1);
			InitParameter("inertia",1);
			InitParameter("airfriction",1);
			
			InitParameter("mass",1);
			InitParameter("inertia",0.2);
			InitParameter("bounce",0.0);
			InitParameter("friction",1);
--			InitParameter("activemoment","false");
			

		SetParentObject();
		SetReturnObject();
end

