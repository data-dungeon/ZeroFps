function Create()
	
	InitObject();
--		InitProperty("P_Primitives3D");
		
--		AttachToClosestZone();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/orc.mad");
			InitParameter("m_fScale","1");
--		InitProperty("CameraProperty");
--		InitProperty("P_Event");
--		InitProperty("P_Ml");		
--		InitProperty("P_Physic");	
		SetParentObject();
		SetReturnObject();
	
	for x = 0,360,45
	do
		InitObject(); 
			InitProperty("P_Primitives3D");
			--InitProperty("PSystemProperty");
			--	InitParameter("PSType","Smoke");				
		HaveRelativOri();
		SetLocalPos(Sin(x)*5,5,Cos(x)*5);	
		AttachToParent();			
	end

--	InitObject();
--		InitProperty("PSystemProperty");
--		InitParameter("PSType","Rain");				
--		SetLocalPos(0,25,0);	
--		AttachToParent();
--		HaveRelativOri();
	
end
