function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			--InitParameter("m_kMadFile","/data/mad/cube.mad");
			InitParameter("m_kMadFile","/data/mad/dm/baldie.mad");			
			InitParameter("m_fScale","1");
			InitParameter("m_iShadowGroup","2");
		
		InitProperty("P_DMGun");				
		InitProperty("P_Track");
--		InitProperty("P_Sound");			
		InitProperty("P_Light");	
			InitParameter("Quadratic_Atten",0.005);
			InitParameter("Diffuse","0.5 0.4 0.4 1");
			InitParameter("Type",2);
			InitParameter("Cutoff",40);
			InitParameter("Exp",40);
			
				
		InitProperty("P_ScriptInterface");
		
		InitProperty("P_ArcadeCharacter");			
		InitProperty("P_Tcs");		
			InitParameter("polygontest","false");
			InitParameter("gravity","true");
			InitParameter("character","true");
			
			InitParameter("radius",0.5);
			
			InitParameter("group",2);
			InitParameter("groupflag",0);
			InitParameter("groupflag",1);
			
			InitParameter("leglength",0.8);
			
			InitParameter("walkablegroupflag",0);

			InitParameter("airfriction",0.1);
			InitParameter("mass",1);
			InitParameter("inertia",1);
			InitParameter("bounce",0);
			InitParameter("friction",5);
			
			InitParameter("activemoment","false");
			InitParameter("cantsleep","true");
			
		
end

