function Create()
	
	InitObject();
		InitProperty("P_Primitives3D");	
			InitParameter("Radius","1");
		InitProperty("PSystemProperty");
			InitParameter("PSType","Smoke");
		SetLocalPos(x,0,x);
		AttachToClosestZone(); 
	
end


