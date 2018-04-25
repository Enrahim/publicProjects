#pragma once
#include "UDPServerCore.h"

class UDPServerCoreWinFactory : public UDPServerCore::Factory {
public:
	UDPServerCore* makeCore();
};

