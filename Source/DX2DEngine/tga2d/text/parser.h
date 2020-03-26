#ifndef _PARSER_H_
#define _PARSER_H_

#include "Token.h"

#define NO_TOKEN	0

//===============================================================
class CParser
{
//---------------------------------------------------------------
// Members
private:

protected:
	std::vector<CToken*>	m_pToken; // this array holds the CTokens, this CParser parses for

public:
//---------------------------------------------------------------
// Methods
private:

protected:

public:
	virtual		~CParser();
	bool		Add(int tokenID, std::string tokenAsStr);
	int			CheckForToken(unsigned char currentChar);
	void		Reset();
};

#endif // _PARSER_H_
