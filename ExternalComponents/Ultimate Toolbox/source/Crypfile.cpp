// ==========================================================================
// 							Class Implementation : COXCryptFile
// ==========================================================================

// Source file : crypfile.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard MFC include
#include "crypfile.h"		// class specification

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXCryptFile, COXConvertedFile)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXCryptFile::COXCryptFile(int nStrength/*=MDC_KEY_LENGTH*/)
	:
	COXConvertedFile(MDC_MSG_LENGTH),
	m_cryptor(nStrength)
	{
	}
	
BOOL COXCryptFile::SetKey(const PMDC_KEY pKey)
	{
	return m_cryptor.SetKey(pKey);
	}

BOOL COXCryptFile::SetKey(const PMDC_KEY pKey, int nStrength)
	{
	return m_cryptor.SetKey(pKey,nStrength);
	}

BOOL COXCryptFile::SetKeyStrength(int nStrength)
	{
	return m_cryptor.SetKeyStrength(nStrength);
	}

BOOL COXCryptFile::ConvertRead(LPCBYTE pOriginal, LPBYTE pConverted)
	{
	m_cryptor.Encrypt(pOriginal, pConverted);
	return TRUE;
	}
	
BOOL COXCryptFile::ConvertWrite(LPCBYTE pOriginal, LPBYTE pConverted)
	{
	m_cryptor.Decrypt(pOriginal, pConverted);
	return TRUE;
	}

#ifdef _DEBUG
void COXCryptFile::Dump(CDumpContext& dc) const
	{
	COXConvertedFile::Dump(dc);
	}

void COXCryptFile::AssertValid() const
	{
	COXConvertedFile::AssertValid();
	}
#endif

COXCryptFile::~COXCryptFile()
	{
	if (m_pDelegateToFile != NULL)
		Close();
	}
	
// protected:

// private:

// Message handlers ---------------------------------------------------------

// ==========================================================================
