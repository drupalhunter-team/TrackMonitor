// ==========================================================================
// 							Class Specification : COXMDCCryptor
// ==========================================================================

// Header file : mdccrypt.h

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
//	YES	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class encapsulates the encryption and decryption of blocks of data
//   using a supplied key
// 	It uses a fast secret-key encryption.

// Remark:
//	This is a method for turning a hash function, here MD5, into a fast
//	secret-key encryption. Based on a suggestion by Phil Karn in sci.crypt, 13
//	Feb 1992. See also his comments from sci.crypt, 23 Mar 1992. The method is
//	a variant of that described in Zheng, Matsumoto and Imai, Crypto 89. See
//	also, "A New Class of Cryptosystems Based on Interconnection Networks" by
//	michaelp@terpsichore.informatic.rwth-aachen.de

//	The cipher operates on 256 bit (32 byte) blocks, using a 768 bit (96 byte)
//	key. The only restriction is that the two 48 byte halves of the key should
//	not be the same.

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////
#ifndef __MDCCRYPT_H__
#define __MDCCRYPT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

// Key and message and length in bytes
#define MDC_KEY_LENGTH 	96
typedef BYTE MDC_KEY[MDC_KEY_LENGTH];
typedef LPBYTE PMDC_KEY;

#define MDC_MSG_LENGTH 	32
typedef BYTE MDC_MSG[MDC_MSG_LENGTH];
typedef LPBYTE PMDC_MSG;
typedef const unsigned char* PCMDC_MSG;
typedef const unsigned long* LPCDWORD;


class OX_CLASS_DECL COXMDCCryptor : public CObject
{
DECLARE_DYNAMIC(COXMDCCryptor)

// Data members -------------------------------------------------------------
public:
	
protected:
	// secret key
	MDC_KEY	m_key;
	// secret key with applied strength
	MDC_KEY	m_keyReal;
	// key strength (in bits)
	int m_nStrength;

private:
	
// Member functions ---------------------------------------------------------
public:
	COXMDCCryptor(int nStrength=MDC_KEY_LENGTH);
	// --- In  :	nStrength	-	cypher key strength (in bits)	
	// --- Out : 
	// --- Returns:
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	BOOL SetKey(const PMDC_KEY pKey);
	// --- In  :	pKey : Set the new key used for encrypting and decrypting
	// --- Out : 
	// --- Returns:	TRUE if new key was successfully set
	// --- Effect : The two 48 byte halves of the key should not be the same.
	
	BOOL SetKey(const PMDC_KEY pKey, int nStrength);
	// --- In  :	pKey : Set the new key used for encrypting and decrypting
	//				nStrength	-	cypher key strength (in bits)
	// --- Out : 
	// --- Returns:
	// --- Effect : The two 48 byte halves of the key should not be the same.
	
	BOOL SetKeyStrength(int nStrength);
	// --- In  :	nStrength	-	cypher key strength (in bits)
	// --- Out : 
	// --- Returns:	TRUE if specified key strength was set successfully
	// --- Effect : 
	
    void Encrypt(PCMDC_MSG pData, PMDC_MSG pEncryptedData) const;
	// --- In  : pData : The data
	//			 pEncryptedData : The encrypted version of that data
	// --- Out : 
	// --- Returns :
	// --- Effect : Encrypts a specified data block
	
    void Decrypt(PCMDC_MSG pData, PMDC_MSG pDecryptedData) const;
	// --- In  : pData : The data
	//			 pDecryptedData : The decrypted version of that data
	// --- Out : 
	// --- Returns :
	// --- Effect : Decrypts a specified data block

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXMDCCryptor();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:      
	void Crypt(LPDWORD out1, LPDWORD out2, LPCDWORD in1, LPCDWORD in2,
		const LPDWORD key1, const LPDWORD key2) const;
	void Transform(LPDWORD buf, LPDWORD in) const;

	BOOL VerifyKey(const PMDC_KEY pKey, int nStrength) const;
	// --- In  :	pKey		-	the key to be verified
	//				nStrength	-	cypher key strength (in bits)
	// --- Out : 
	// --- Returns:	TRUE if the key is valid
	// --- Effect : The two 48 byte halves of the key should not be the same.
	
	void ApplyStrength(MDC_KEY keyDest, MDC_KEY keySrc, int nStrength) const;
	// --- In  :	keyDest		-	the key to be the copy of keySrc with applied 
	//								strength
	//				keySrc		-	the key to which strength has to be applied
	//				nStrength	-	cypher key strength (in bits)
	// --- Out : 
	// --- Returns:	
	// --- Effect : 
				 

private:
                   
// Message handlers ---------------------------------------------------------

};

#endif
// ==========================================================================
