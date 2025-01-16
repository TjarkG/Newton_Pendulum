//
// Created by Joel Neumann on 14.08.24.
//

#include "Protocol.h"

size_t touchCountToIndex(TouchCount count)
{
	size_t index = 0;
	switch (count)
	{
		case ZERO:
			index = 0;
			break;
		case ONE:
			index = 1;
			break;
		case TWO:
			index = 2;
			break;
		case THREE:
			index = 3;
			break;
		case FOUR:
			index = 4;
			break;
		case FIVE:
			index = 5;
			break;
		case GREATER_THAN_FIVE:
			index = 6;
			break;
		case ALL:
			break;
	}
	return index;
}
