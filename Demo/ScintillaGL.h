#pragma once

#include <new>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>

#include <string>
#include <vector>

#include <SDL.h>

#include "Platform.h"

#include "ILexer.h"
#include "Scintilla.h"
#include "SVector.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "XPM.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "Decoration.h"
#include "CharClassify.h"
#include "Document.h"
#include "Selection.h"
#include "PositionCache.h"
#include "Editor.h"
#include "UniConversion.h"

#include "SciLexer.h"
#include "LexerModule.h"
#include "SciLexer.h"
#include "LexerModule.h"
#include "Catalogue.h"

class LexState2 : public LexInterface {
	const LexerModule *lexCurrent;
public:
	int lexLanguage;

	LexState2(Document *pdoc_) : LexInterface(pdoc_) {
		lexCurrent = 0;
		performingStyle = false;
		lexLanguage = SCLEX_CONTAINER;
	}

	~LexState2() {
		if (instance) {
			instance->Release();
			instance = 0;
		}
	}

	void SetLexerModule(const LexerModule *lex) {
		if (lex != lexCurrent) {
			if (instance) {
				instance->Release();
				instance = 0;
			}
			lexCurrent = lex;
			if (lexCurrent)
				instance = lexCurrent->Create();
			pdoc->LexerChanged();
		}
	}

	void SetLexer(uptr_t wParam) {
		lexLanguage = wParam;
		if (lexLanguage == SCLEX_CONTAINER) {
			SetLexerModule(0);
		} else {
			const LexerModule *lex = Catalogue::Find(lexLanguage);
			if (!lex)
				lex = Catalogue::Find(SCLEX_NULL);
			SetLexerModule(lex);
		}
	}

	void SetLexerLanguage(const char *languageName) {
		const LexerModule *lex = Catalogue::Find(languageName);
		if (!lex)
			lex = Catalogue::Find(SCLEX_NULL);
		if (lex)
			lexLanguage = lex->GetLanguage();
		SetLexerModule(lex);
	}

	void SetWordList(int n, const char *wl) {
		if (instance) {
			int firstModification = instance->WordListSet(n, wl);
			if (firstModification >= 0) {
				pdoc->ModifiedAt(firstModification);
			}
		}
	}

	void PropSet(const char *key, const char *val) {
		if (instance) {
			int firstModification = instance->PropertySet(key, val);
			if (firstModification >= 0) {
				pdoc->ModifiedAt(firstModification);
			}
		}
	}
};

class MyEditor: public Editor
{
	size_t nextTime;
	static const size_t tickInterval = 100;
public:
	LexState2 ls;
	
	MyEditor();

	bool BraceMatch();
	void OnKeyDown(SDL_KeyboardEvent& event);
	void Paint();
};