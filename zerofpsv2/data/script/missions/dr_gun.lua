
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Another mission
--// 
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Dr Gun", difficulty=1, xp=100, cash=1000, success=0 } -- difficulty = Level

NumCivilians = 0
NumCiviliansWounded = 0
NumCiviliansAtDoctor = 0
CivWoundedList = {} -- lista p� vilka civilians som har blivit skadade
InjureList = {} -- en annan lista som s�ks igenom f�r att se om n�gra skadeskjutna civila har d�tt
RuningToHospitleList = {} -- lista som h�ller reda p� vilka som springer till sjukhuset
HospitalObject = -1

MissionText = 
{ 
	short = "Dr Gun",
	long  = [[Dr Gun\n\nMr. Wilhelm Von Leath wont you to work for him for a while.\n\nYou are hired to damage 5 persons.
	 You can pick anyone, he does't care, just don't kill anyone (the undertaker have enough work already). When you are 
	 done return to base. You will get 1000 dollar in cash when the mission is done (minus 200 dollar for any killed person)]]
}

function OnMissionStart()

	Print("Starting mission Dr Gun")

	-- t�m listor
	CivWoundedList = {} 
	InjureList = {}
	RuningToHospitleList = {}

	local team = 1 -- team civilians
	local civ_list = GetCharsByFraction(team) -- h�mta en lista p� alla civila
	NumCivilians = civ_list[1] -- f�rsta arg �r vektorns size

	-- H�mta information om dom civila i staden
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

	local pos = GetEntityPos( GetDMObject(0) ) -- h�mta objektposition

	pos[1] = pos[1] + 10
	pos[3] = pos[3] + 10

	HospitalObject = GetDMObject(1)

	Print("Hospital object = ", HospitalObject)

end

function OnMissionSuccess()

		

end

function OnMissionFailed()



end

function GetRangeBetween(a, b) -- tar 2 tabeller med 3 double vardera

	local xdif = a[1] - b[1]
	local zdif = a[3] - b[3]

	-- vi �r intresserade av avst�nd
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

	local hq_pos = GetEntityPos(HospitalObject)
	local obj_pos = GetEntityPos(object)

	-- Kolla om den skadade personen har kommit fram till sjukhuset
	-- i s� fall ta bort det objektet och �ka upp "NumCiviliansAtDoctor"
	if GetRangeBetween( hq_pos, obj_pos ) < 1 then

		Print("------------------------------------------------")
		Print( "Removing patient at doctor Gun: ", object)
		Print("------------------------------------------------")

		Delete(object)

		NumCiviliansAtDoctor = NumCiviliansAtDoctor + 1

		return 1
	end

	-- Se om en ny pathfind beh�vs g�ras mot sjukhuset
	if HavePath(object) == 1 then
		return 0
	end	

	Print("------------------------------------------------")
	Print( "Sending object: ", object, " to hosptile, pos: ")
	Print( hq_pos[1] )
	Print( hq_pos[2] )
	Print( hq_pos[3] )
	Print("------------------------------------------------")

	MakePathFind(object, hq_pos)

	return 0
end

-- Anropas var 3:e sekund
function IsMissionDone()

	-- Kolla om n�gra civila har blivit skadade
	for x = 0, NumCivilians-1, 1
	do
		local obj = CivWoundedList[x*2] 

		if obj > -1 then -- hoppa f�rbi dom som redan har blivit r�knade som skadade

			local prev_life = CivWoundedList[x*2+1] 
			local curr_life = GetCharStats( obj, 0 ) -- 0 = Life
			local dmg = prev_life - curr_life

			if dmg > 0 then
				
				CivWoundedList[x*2] = -1 -- markera att denna inte skall r�knas mer

				InjureList[NumCiviliansWounded] = obj -- l�gg till till lista �ver skadeskjutna gubbar
				RuningToHospitleList[NumCiviliansWounded] = obj

				NumCiviliansWounded = NumCiviliansWounded + 1 -- �ka antalet skadade personer
			end

		end

	end

	-- Kolla om n�gra har blivit skadeskjutna till d�ds, i s� fall minska ner antalet skade-skjutna m�nniskor.
	if NumCiviliansWounded > 0 then

		for y = 0, NumCiviliansWounded-1, 1 
		do
			local obj_wounded = InjureList[y]

			if obj_wounded > -1 then -- om objektet inte redan �r d�d

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

	if NumCiviliansAtDoctor == 5 then
		MissionInfo.success = 1 -- Uppdraget har lyckats!
	end

end

function IsMissionFailed()



end

