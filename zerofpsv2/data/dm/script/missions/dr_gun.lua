

--///////////////////////////////////////////////////////////////////////////////

--//

--// Mission: Another mission

--// 

--///////////////////////////////////////////////////////////////////////////////



MissionInfo = { name="Dr Gun", difficulty=1, xp=100, cash=1000, success=0 } -- difficulty = Level



NumCivilians = 0

NumCiviliansWounded = 0

NumCiviliansAtDoctor = 0

CivWoundedList = {} -- lista på vilka civilians som har blivit skadade

InjureList = {} -- en annan lista som söks igenom för att se om några skadeskjutna civila har dött

RuningToHospitleList = {} -- lista som håller reda på vilka som springer till sjukhuset

HospitalObject = -1



DrGunCounter = 5



MissionText = 

{ 

	short = "Dr Gun\n\nWork for Ludavico Hospital",

	long  = [[Dr Gun\n\nMr. Wilhelm Von Leath on Ludavico Hospital wants you to work for him for a while.\n\nTo few people are in need of hospitalisation these days. You are hired to damage five persons. You can pick anyone, he doesn't care, just don't kill anyone (the undertaker have enough work already.)\nReturn to base when you are done. You will get 1000 dollar in cash when the mission is done (minus 200 dollar for any killed person.)]]

}





Skin1 = { tex1="map_icons/hospital.bmp", tex2="0", tex3="0", tex4="0", tex1a="0",

 tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255, bkB=255, borderR=255, borderG=255, borderB=255, bd_size=0, tile=0, trans=0

 }



function UpdateCivList()



	-- töm listor

--	CivWoundedList = {} 

--	InjureList = {}

--	RuningToHospitleList = {}



	local team = 1 -- team civilians

	local civ_list = GetCharsByFraction(team) -- hämta en lista på alla civila

	NumCivilians = civ_list[1] -- första arg är vektorns size



	Print( "NumCivilians = ", NumCivilians)



	-- Hämta information om dom civila i staden

	local counter = 0 

	for x = 2, NumCivilians+1, 1

	do

		local obj_id = civ_list[x]

		local life = GetCharStats( obj_id, 0 ) -- 0 = Life

		

		CivWoundedList[counter] = obj_id

		counter = counter + 1



		CivWoundedList[counter] = life

		counter = counter + 1

	end



end



function OnMissionStart()



	Print("Starting mission Dr Gun")



	-- Skapa 2 spawners för gubbar

	local pos = { 83.0,1.20561,33.5357 } --{ 83.4634,1.20561,34.7872 }

	RunScript("data/script/objects/t_spawner_mechanic_door.lua",-1, pos );



	local pos2 = { 83.0,1.20561,29.1071 } --{ 83.8529,1.20561,26.6406 }

	RunScript("data/script/objects/t_spawner_man_door.lua",-1, pos2 );



	UpdateCivList()



	local pos = GetEntityPos( GetDMObject(0) ) -- hämta objektposition



	pos[1] = pos[1] + 10

	pos[3] = pos[3] + 10



	HospitalObject = GetDMObject(1)



	Print("Hospital object = ", HospitalObject)

	Print("Num civilian = ", NumCivilians)



	-----------------------------------------------------------------------

	-- Rensa alla gamla mapiconer och lägg till en ny som visar

	-- vart polistationen finns.

	-----------------------------------------------------------------------

	HideAllMapIcons()

	CreateWnd(4,"HosptialMapLabel","MapWnd","",420,179,16,16,0,0,0)

	ChangeSkin("HosptialMapLabel","Skin1","Label")



end



function OnMissionSuccess()



		



end



function OnMissionFailed()







end



function GetRangeBetween(a, b) -- tar 2 tabeller med 3 double vardera



	local xdif = a[1] - b[1]

	local zdif = a[3] - b[3]



	-- vi är intresserade av avstånd

	if xdif < 0 then xdif = -xdif end

	if zdif < 0 then zdif = -zdif end



	range = xdif + zdif



	return range

end



function SendObjectToHospitle(object)



	if HospitalObject == -1 then



		Print( "No hospital found in Dark Metropolis" )

		return 0

	end



	local hospital_pos = GetEntityPos(HospitalObject)

	local obj_pos = GetEntityPos(object)



	-- Kolla om den skadade personen har kommit fram till sjukhuset

	-- i så fall ta bort det objektet och öka upp "NumCiviliansAtDoctor"

	if GetRangeBetween( hospital_pos, obj_pos ) < 1 then



		Print("------------------------------------------------")

		Print( "Removing patient at doctor Gun: ", object)

		Print("------------------------------------------------")



		Delete(object)



	--	PlayAnim(HospitalObject, "open");

	--	SetNextAnim(HospitalObject, "idle");



		NumCiviliansAtDoctor = NumCiviliansAtDoctor + 1



		return 1

	end



	-- Se om en ny pathfind behövs göras mot sjukhuset

	if HavePath(object) == 1 then

		return 0

	end	



	Print("------------------------------------------------")

	Print( "Sending object: ", object, " to hosptile, pos: ")

	Print( hospital_pos[1] )

	Print( hospital_pos[2] )

	Print( hospital_pos[3] )

	Print("------------------------------------------------")



	MakePathFind(object, hospital_pos)



	return 0

end





-- Anropas var 3:e sekund

function IsMissionDone()



	if DrGunCounter == 5 then



		UpdateCivList()

		DrGunCounter = 0

	end



	if HospitalObject == -1 then



		HospitalObject = GetDMObject(1)

	end



	DrGunCounter = DrGunCounter + 1



	-- Kolla om några civila har blivit skadade

	for x = 0, NumCivilians-1, 1

	do

		local obj = CivWoundedList[x*2] 



		if obj > -1 then -- hoppa förbi dom som redan har blivit räknade som skadade



			local prev_life = CivWoundedList[x*2+1] 

			local curr_life = GetCharStats( obj, 0 ) -- 0 = Life

			local dmg = prev_life - curr_life



		--	Print("prev_life = ", prev_life)

		--	Print("curr_life = ", curr_life)

		--	Print("dmg = ", dmg)



			if dmg > 0 and IsDead(obj) == 0 then

				

				CivWoundedList[x*2] = -1 -- markera att denna inte skall räknas mer



				InjureList[NumCiviliansWounded] = obj -- lägg till till lista över skadeskjutna gubbar

				RuningToHospitleList[NumCiviliansWounded] = obj



				NumCiviliansWounded = NumCiviliansWounded + 1 -- öka antalet skadade personer



				Print("NumCiviliansWounded = ", NumCiviliansWounded)

			end



		end



	end



	-- Kolla om några har blivit skadeskjutna till döds, i så fall minska ner antalet skade-skjutna människor.

	if NumCiviliansWounded > 0 then



		for y = 0, NumCiviliansWounded-1, 1 

		do

			local obj_wounded = InjureList[y]



			if obj_wounded > -1 then -- om objektet inte redan är död



				if IsDead(obj_wounded) == 1 then



					Print("--------- YOU ARE NOT SUPPOSED TO KILL PEOPLE! ------------")



					MissionInfo.cash = MissionInfo.cash - 200



					NumCiviliansWounded = NumCiviliansWounded - 1 -- minska antalet skadade personer



					InjureList[y] = -1

				end



			end

		end



	end



	-- PathFind'a till sjukhuset med dom som skall ditt

	if NumCiviliansWounded > 0 then



		for y = 0, NumCiviliansWounded-1, 1

		do

			if RuningToHospitleList[y] > 0 then



				local obj_running = RuningToHospitleList[y]

				local swallowed = SendObjectToHospitle( obj_running )



				-- ta bort dom ur listan som har raderats

				if swallowed == 1 then

					RuningToHospitleList[y] = -1

				end



			end

		end



	end



	Print("Person wounded: ", NumCiviliansWounded)

	Print("Persons in hospital: ", NumCiviliansAtDoctor)

	Print("Hospital: ", HospitalObject)



	if NumCiviliansWounded == 5 then --NumCiviliansAtDoctor == 5 then

		MissionInfo.success = 1 -- Uppdraget har lyckats!

	end







end



function IsMissionFailed()







end



