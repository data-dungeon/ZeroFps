function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/meat.mad");
			InitParameter("m_fScale",0.4);
		InitProperty("P_Event");
		InitProperty("P_Ml");
		InitProperty("P_PSystem");
			InitParameter("PSType", "gibb");
		
		InitProperty("P_Tcs");
			InitParameter("radius",-1);
			InitParameter("gravity","true");
			InitParameter("group",3);
			InitParameter("groupflag",1);		
			InitParameter("groupflag",2);		
			InitParameter("groupflag",3);		
		
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	p = GetObjectPos(GetSelfID());
	p[1] = p[1] + Random(50)-25;
	p[2] = p[2] + 25;	
	p[3] = p[3] + Random(50)-25;	
	
	SetVelTo(GetSelfID(),p,3);
	
end

function Collission()
	Bounce(GetSelfID());
end