function Create()
	
	InitObject();
		InitProperty("P_Light");
			InitParameter("Quadratic_Atten",0.01);
			InitParameter("Diffuse","0.0 0.0 0.5 1");		
		SetParentObject();
		SetReturnObject();
end

