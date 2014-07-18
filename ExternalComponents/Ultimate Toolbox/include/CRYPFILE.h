// ==========================================================================
// 							Class Specification : COXCryptFile
// ==========================================================================

// Header file : crypfile.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                        
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CFile

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This file supplies encryption when converting a file

// Remark:
//		***

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////
#ifndef __CRYPFILE_H__
#define __CRYPFILE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "convfile.h"
#include "mdccrypt.h"


class OX_CLASS_DECL COXCryptFile : public COXConvertedFile
{
DECLARE_DYNAMIC(COXCryptFile)

// Data members -------------------------------------------------------------
public:
	
protected:
	COXMDCCryptor	m_cryptor;
	
private:
	
// Member functions ---------------------------------------------------------
public:
	COXCryptFile(int nStrength=MDC_KEY_LENGTH);
	// --- In  :	nStrength	-	cypher key strength (in bits)	
	// --- Out : 
	// --- Returns:
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	BOOL SetKey(const PMDC_KEY pKey);
	// --- In  :	pKey : Set the new key used for encrypting and decrypting
	// --- Out : 
	// --- Returns:	TRUE if new key was successfully set
	// --- Effect : The key must be a 768 bit (96 byte) buffer. The only restriction is 
	//				that the two 48 byte halves of the key should not be the same.
	//				This function should not be used on an open file.
	
	BOOL SetKey(const PMDC_KEY pKey, int nStrength);
	// --- In  :	pKey : Set the new key used for encrypting and decrypting
	//				nStrength	-	cypher key strength (in bits)
	// --- Out : 
	// --- Returns:	TRUE if new key was successfully set
	// --- Effect : The key must be a 768 bit (96 byte) buffer. The only restriction is 
	//				that the two 48 byte halves of the key should not be the same.
	//				This function should not be used on an open file.
	
	BOOL SetKeyStrength(const int nStrength);
	// --- In  :	nStrength	-	cypher key strength (in bits)
	// --- Out : 
	// --- Returns:	TRUE if specified key strength was set successfully
	// --- Effect : 
	
	virtual BOOL ConvertRead(LPCBYTE pOriginal, LPBYTE pConverted);
	virtual BOOL ConvertWrite(LPCBYTE pOriginal, LPBYTE pConverted);

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXCryptFile();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:      

private:
                   
// Message handlers ---------------------------------------------------------

};

#endif
// ==========================================================================
