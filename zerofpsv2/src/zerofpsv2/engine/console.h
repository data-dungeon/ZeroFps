#ifndef _ENGINE_CONSOLE_H_
#define _ENGINE_CONSOLE_H_

#include "input.h"
#include "../render/render.h"
#include "engine_x.h"
#include <iostream>
#include <vector>
#include <deque>

using namespace std;

class Input;
class ZeroFps;

//max lenght of an entered command
#define COMMAND_LENGHT 128

/** \brief	The Console
	 \ingroup Engine
*/
class ENGINE_API Console : public BasicConsole 
{
	private:
		enum CON_CMD
		{
			CONCMD_NONE,
			CONCMD_TOGGLE,				// Toggle Console on/off.
			CONCMD_RUN,					// Run a cmd, remove it from input line, print it to console and send it to cmdsystem.
			CONCMD_SCROLLUP,			// Scroll console text up.
			CONCMD_SCROLLDOWN,		// Scroll console text down.
			CONCMD_SCROLLEND,
			CONCMD_HISTORYUP,			// Scroll up in console input history
			CONCMD_HISTORYDOWN,		// Scroll down in console inut history.
			CONCMD_MARKERLEFT,		// Move input cursor to the left.
			CONCMD_MARKERRIGHT,		// Move input cursor to the right.
			CONCMD_TOGGLEINSERT		// Toggle insert and overwrite mode.
		};

		ZeroFps*					m_pkEngine;		
		Render*					m_pkRender;
		Input*					m_pkInput;
		TextureManager*		m_pkTexMan;
		
		//SDL_Event				m_kEvent;
		char						m_aCommand[TEXT_MAX_LENGHT];	// Current text in inputline.
		int						m_iInputPos;						// Position of input cursor.
		bool						m_bModeInsert;						// Is mode insert or is it overwrite.

		int						m_iBufferSize;						
		bool						m_bShift;

		unsigned int			m_nStartLine;						

		const unsigned int	MAX_CMD_HISTRORY_LENGTH;		// Max size of input history.
		int						m_iLastCommand;					// Current input history command we are looking at. -1 if none.
		deque<string>			m_kCommandHistory;				// The history of input lines.
		
		bool						m_bActive;
		float						m_fToggleTime;
	
		bool						m_bShowMarker;						// True if cursor is to be displayed when rendering console.
		float						m_fMarkerToggleTime;				// Next time when cursor toggle visible state (for blinking).

		InputHandle*			m_pkInputHandle;

		int						m_iNextAutoComplete;				// Index of next autocomplete name.
		bool						m_bAutoCompleteOn;				// True if autocomplete is on progress

		//void FormatKey(int& r_iKey);
		char FormatKey(QueuedKeyInfo& kKey);
		void InsertKey(unsigned char ucKey);
	
		void RemoveKey(bool bDelete);

		void ConsoleCmd(CON_CMD eCmd);

		void AutoComplete();

public:
		Console();
		~Console(){};
		
		void Update(void);
		void Draw(void);
		bool Execute(char* aText);

		void Toggle();
		bool IsActive()	{ return m_bActive; }

		bool StartUp();
		bool ShutDown();
		bool IsValid();

};



#endif







