#pragma once

#include <Duin/Core/Signals/SignalsModule.h>
#include "FileManager.h"

struct TabSignals
{
	duin::Signal<bool>* onFocusChange = nullptr;
	duin::Signal<FSNode *>* onFileSelect = nullptr;
	duin::Signal<FSNode *>* onFileDoubleSelect = nullptr;
	duin::Signal<FSNode *>* onFileRightSelect = nullptr;
};