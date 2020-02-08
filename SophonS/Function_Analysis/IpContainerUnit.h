
#pragma once

#define MaxProtocol 35

#include"../BasisFunction/Base.h"

struct IpContainerUnit{

	unsigned int protocol[MaxProtocol];

	unsigned int conter;

	unsigned int totLength;

	char startTime[20];

	char endTime[20];

	char ip[4 * 8 + 7 + 1];

public:
	IpContainerUnit(){

		conter = totLength = 0;

		memset(protocol, 0, sizeof(protocol));

	}
};

