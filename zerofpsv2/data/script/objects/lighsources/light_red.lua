function Create()
	
	InitObject();
		InitProperty("P_Light");
			InitParameter("Quadratic_Atten",0.01);
			InitParameter("Diffuse","0.5 0.0 0.0 1");		
		SetParentObject();
		SetReturnObject();
end

