function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/gob_arm.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
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
	p[1] = p[1] + Random(100)-50;
	p[2] = p[2] + 50;	
	p[3] = p[3] + Random(100)-50;

	SetVelTo(GetSelfID(),p,4);
	
	p[1] = Random(100);
	p[2] = Random(100);
	p[3] = Random(100);

	SetRotVel (GetSelfID(),p);
	
end

function Collission()
	Bounce(GetSelfID());
end