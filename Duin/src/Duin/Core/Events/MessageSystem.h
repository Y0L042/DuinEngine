#pragma once
#include "Duin/Core/Signals/Signal.h"
#include "MessageCodes.h"

namespace duin
{
	// Singleton Class
	class MessageSystem
	{
          public:
     //     Connect() 
			  //Disconnect()

          private:
			  //std::unordered_map<MessageCode, Signal> signalMap;

            static MessageSystem& Get();

	};
}