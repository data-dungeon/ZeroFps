#pragma warning (disable :4786)

#ifndef _RULESYSTEM_CHARACTERFACTORY_H_
#define _RULESYSTEM_CHARACTERFACTORY_H_

#include "../../mcommon/mcommon_x.h"

#include "../rulesystem.h"
#include "characterstats.h"
#include <map>
#include <string>
	using namespace std;

extern map<string, CharacterStats> g_kCharacters;

class CharacterFactory
{
private:
   void static LoadStatTypes();
public:
	// Load character stats from file, if already loaded, just copy
	static CharacterStats* GetCharacterData (string kCharacterName);

};

#endif