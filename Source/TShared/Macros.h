#pragma once

#define CYCLIC_ERASE(vector, index) \
	if(index >= 0 && index < vector.size())\
	{\
		if(!vector.empty())\
		{\
		vector.at(index) = vector.at(vector.size() - 1);\
		vector.pop_back();\
		}\
	}\
	else\
	{\
			assert(false);\
	}