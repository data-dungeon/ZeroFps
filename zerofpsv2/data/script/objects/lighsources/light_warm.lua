function Create()
	
	InitObject();
		InitProperty("P_Light");
			InitParameter("Quadratic_Atten",0.01);
			InitParameter("Diffuse","0.2 0.2 0.1 1");		
		SetParentObject();
		SetReturnObject();
end

