#ifndef _TOKEN_H_
#define _TOKEN_H_

//===============================================================
class CToken
{
//---------------------------------------------------------------
// Members
private:

protected:

public:
	int				m_tokenID;
	int				m_matchPos;
	std::string		m_tokenAsStr;

//---------------------------------------------------------------
// Methods
private:

protected:

public:
						CToken(int tokenID, std::string tokenAsStr);
	virtual				~CToken();
};

#endif // _TOKEN_H_
