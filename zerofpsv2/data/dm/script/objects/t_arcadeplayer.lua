function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/cube.mad");
			--InitParameter("m_kMadFile","/data/mad/dm/baldie.mad");			
			InitParameter("m_fScale","1");
			InitParameter("m_iShadowGroup","2");
		
		InitProperty("P_DMGun");				
		InitProperty("P_Track");
--		InitProperty("P_Sound");			
		InitProperty("P_ScriptInterface");
		
		InitProperty("P_ArcadeCharacter");			
		InitProperty("P_Tcs");		
			InitParameter("polygontest","false");
			InitParameter("gravity","true");
			InitParameter("character","true");
			
			InitParameter("radius",-1);
			
			InitParameter("group",1);
			InitParameter("groupflag",0);
			InitParameter("groupflag",1);
			
			InitParameter("leglength",1.1);
			
			InitParameter("walkablegroupflag",0);

			InitParameter("airfriction",0.1);
			InitParameter("mass",1);
			InitParameter("inertia",1);
			InitParameter("bounce",0);
			InitParameter("friction",5);
			
			InitParameter("activemoment","false");
			InitParameter("cantsleep","true");
			
		
end

