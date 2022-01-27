#pragma once

class bf_read
{
public:
	unsigned long		*m_pData;
	int					m_nDataBytes;
	int					m_nDataBits;
	int					m_iCurBit;
	bool				m_bOverflow;
	bool				m_bAssertOnOverflow;
	const char			*m_pDebugName;
};

class bf_write
{
public:
	int StartWriting(void *pData, int nBytes, int iStartBit = 0, int nBits = -1);

public:
	unsigned long	*m_pData;
	int				m_nDataBytes;
	int				m_nDataBits;
	int				m_iCurBit;
	bool			m_bOverflow;
	bool			m_bAssertOnOverflow;
	const char		*m_pDebugName;
};