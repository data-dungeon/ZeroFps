-- TO ADD!!! DON'T make this object stackable! Will cause trouble!!!!

function Create()
-- determine type of object

	InitObject();
		InitProperty("MadProperty");	
			InitParameter("m_fScale","0.1");

		InitProperty("P_Event");
		InitProperty("P_Ml");		
		InitProperty("P_Item");
		SetParentObject();
		SetReturnObject();
	
	
end


function Init()
	Attack=0
	Defence=0
	SpecAtt=0
	SpecDef=0
	Skill=0
	Attr=0


	type = Random(4)

--weapon
	if type == 0 then
		wtype = Random(3)
		
		if wtype == 0 then	
			SetItemName ("Sword")
			SetPropertyValue("MadProperty", "m_kMadFile","/data/mad/sword.mad");
		end

		if wtype == 1 then	
			SetItemName ("WarHammer")
			SetPropertyValue("MadProperty", "m_kMadFile","/data/mad/warhmr.mad");
		end

		if wtype == 2 then	
			SetItemName ("EvilCleaverOfDeath")
			SetPropertyValue("MadProperty", "m_kMadFile","/data/mad/cleaver.mad");
		end


		SetAttackBonus ("Normal", 40);

		Attack=60
		Defence=25
		SpecAtt=50
		SpecDef=50

	end

	if type == 1 then
		SetItemName ("Ring");
		StatChans=50
		AttrChans=30
		SpecAtt=40
		SpecDef=40
		Skill=40

		SetPropertyValue("MadProperty", "m_kMadFile","/data/mad/barrel.mad");
	end

	if type == 2 then
		SetItemName ("Armour");

		SetDefenceBonus ("Normal", 50);
		Defence=50
		SpecDef=30

		SetPropertyValue("MadProperty", "m_kMadFile","/data/mad/armour.mad");
	end

	if type == 3 then
		SetItemName ("Shield")
		SetDefenceBonus ("Normal", 50);
		Defence=45
		SpecDef=40
		Attribute=40

		SetPropertyValue("MadProperty", "m_kMadFile","/data/mad/shield.mad");
	end



	if Random(100) < SpecAtt then
		AttType = GetRandomCategory();

		AddBeforeItemName (AttType)
		AddAttackBonus(AttType, Random(SpecAtt+1))
	end

	if Random(100) < SpecDef then
		DefType = GetRandomCategory();

		AddAfterItemName (DefType)
		AddDefenceBonus(DefType, Random(SpecDef+1))
	end

	if Random(100) < Skill then
		SkillType = GetRandomSkill();

		AddAfterItemName (SkillType)
		AddSkillBonus(SkillType, Random(Skill+1))
	end

	if Random(100) < Attr then
		AttrType = GetRandomSkill();

		AddAfterItemName (AttrType)
		AddAttributeBonus(AttrType, Random(Attr+1))
	end

	if Attack > 0 then
		AddAttackBonus ("Normal", Random(Attack+1))
	end

	if Defence > 0 then
		AddDefenceBonus ("Normal", Random(Defence+1))
	end

	PrintItemStats();


end
