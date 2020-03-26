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


#define EMBER_NET_LOG(format, ...) printf("[EmberNet] " format, __VA_ARGS__) 

#define EMBER_NET_ASSERT(condition, text) assert(condition && "[EmberNet] ")