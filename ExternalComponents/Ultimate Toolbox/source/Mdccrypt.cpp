// ==========================================================================
// 							Class Implementation : COXMDCCryptor
// ==========================================================================

// Source file :mdccrypt.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
                         
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard MFC include
#include "mdccrypt.h"	// class specification

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXMDCCryptor, CObject)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of helper macros

/* F, G, H and I are basic MD5 functions */

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits */

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */
/* Rotation is separate from addition to prevent recomputation  */

#define FF(a, b, c, d, x, s, ac) \
	{(a) += F ((b), (c), (d)) + (x) + (DWORD)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }

#define GG(a, b, c, d, x, s, ac) \
	{(a) += G ((b), (c), (d)) + (x) + (DWORD)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }

#define HH(a, b, c, d, x, s, ac) \
	{(a) += H ((b), (c), (d)) + (x) + (DWORD)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }

#define II(a, b, c, d, x, s, ac) \
	{(a) += I ((b), (c), (d)) + (x) + (DWORD)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }

/* Initial values for MD5 Transform hash function */
static const DWORD ihash[4] = {
	0x67452301L, 0xefcdab89L, 0x98badcfeL, 0x10325476L };

/* Round 1 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22

/* Round 2 */
#define S21 5
#define S22 9
#define S23 14
#define S24 20

/* Round 3 */
#define S31 4
#define S32 11
#define S33 16
#define S34 23

/* Round 4 */
#define S41 6
#define S42 10
#define S43 15
#define S44 21

// The default key (must be of MDC_KEY_LENGTH (96))
// has been generated randomly
#define DEFAULT__KEY 												\
	{                                                               \
	0xC0, 0x81, 0x3F, 0x31, 0x4C, 0xD9, 0x84, 0xBD, 0xD1, 0x0D,     \
	0x49, 0xA3, 0xD1, 0x43, 0xEF, 0x14, 0x98, 0xE3, 0x68, 0xD9,     \
	0xF8, 0xED, 0x03, 0x11, 0x68, 0x8D, 0x90, 0x63, 0xC7, 0xB8,     \
	0x7E, 0x8F, 0x9D, 0x61, 0x41, 0x4A, 0xA3, 0xEF, 0xF7, 0x49,     \
	0x85, 0xEE, 0x4B, 0x9B, 0xE6, 0x04, 0x99, 0x3B, 0xA4, 0x27,     \
	0x53, 0xE6, 0x5E, 0x0E, 0xA3, 0x2A, 0x34, 0xE5, 0x51, 0xC0,     \
	0xCD, 0x44, 0xE7, 0x1A, 0x99, 0x62, 0x13, 0x40, 0x6B, 0x87,     \
	0xE4, 0xBF, 0x8B, 0x3C, 0x5D, 0x0C, 0xBD, 0x9E, 0x8E, 0xB6,     \
	0x4F, 0xDA, 0x08, 0xC1, 0xBC, 0x86, 0x61, 0xD7, 0x53, 0x74,     \
	0xE5, 0xE4, 0xBF, 0x57, 0x05, 0xCB                              \
	}


/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXMDCCryptor::COXMDCCryptor(int nStrength/*=MDC_KEY_LENGTH*/)
	{
	m_nStrength=0;
	// Initialize the key
	MDC_KEY key=DEFAULT__KEY;
	if(!SetKey(key,nStrength))
		{
		TRACE(_T("COXMDCCryptor::COXMDCCryptor: failed to set cypher key\n"));
		AfxThrowNotSupportedException();
		}
	}
	
BOOL COXMDCCryptor::SetKey(const PMDC_KEY pKey)
	{
	if(!VerifyKey(pKey,m_nStrength))
		return FALSE;

	memcpy(m_key, pKey, MDC_KEY_LENGTH);
	ApplyStrength(m_keyReal,m_key,m_nStrength);

	return TRUE;
	}
	
BOOL COXMDCCryptor::SetKey(const PMDC_KEY pKey, int nStrength)
	{
	ASSERT(nStrength>0 && nStrength<=MDC_KEY_LENGTH*8);
	int nOldStrength=m_nStrength;
	m_nStrength=nStrength;	
	if(!SetKey(pKey))
		{
		m_nStrength=nOldStrength;
		return FALSE;
		}
	else
		return TRUE;
	}
	
BOOL COXMDCCryptor::SetKeyStrength(int nStrength)
	{
	ASSERT(nStrength>0 && nStrength<=MDC_KEY_LENGTH*8);
	if(VerifyKey(m_key,nStrength))
		{
		m_nStrength=nStrength;
		ApplyStrength(m_keyReal,m_key,m_nStrength);
		return TRUE;
		}
	else
		return TRUE;
	}

	
void COXMDCCryptor::Encrypt(PCMDC_MSG pData, PMDC_MSG pEncryptedData) const
	{
	ASSERT(AfxIsValidAddress(pData, MDC_MSG_LENGTH));
	ASSERT(AfxIsValidAddress(pEncryptedData, MDC_MSG_LENGTH));
	Crypt( (LPDWORD)pEncryptedData, (LPDWORD)&pEncryptedData[MDC_MSG_LENGTH / 2],
		(LPCDWORD)pData, (LPCDWORD)&pData[MDC_MSG_LENGTH / 2],
		(LPDWORD)m_keyReal,(LPDWORD)&m_keyReal[MDC_KEY_LENGTH / 2]);
	}
	
void COXMDCCryptor::Decrypt(PCMDC_MSG pData, PMDC_MSG pDecryptedData) const
	{
	ASSERT(AfxIsValidAddress(pData, MDC_MSG_LENGTH));
	ASSERT(AfxIsValidAddress(pDecryptedData, MDC_MSG_LENGTH));
	Crypt((LPDWORD)&pDecryptedData[MDC_MSG_LENGTH / 2], (LPDWORD)pDecryptedData,
		(LPCDWORD)&pData[MDC_MSG_LENGTH / 2], (LPCDWORD)pData,
		(LPDWORD)m_keyReal, (LPDWORD)&m_keyReal[MDC_KEY_LENGTH / 2]);
	}

#ifdef _DEBUG
void COXMDCCryptor::Dump(CDumpContext& dc) const
	{
	CObject::Dump(dc);
	}

void COXMDCCryptor::AssertValid() const
	{
	CObject::AssertValid();
	// Both halves of the key must have a different contents
	ASSERT(MDC_KEY_LENGTH % 2 == 0);
	ASSERT(memcmp(m_key, m_key + MDC_KEY_LENGTH / 2, MDC_KEY_LENGTH / 2) != 0);
	ASSERT(m_nStrength>0 && m_nStrength<=MDC_KEY_LENGTH*8);
	}
#endif

COXMDCCryptor::~COXMDCCryptor()
	{
	}
	
// protected:
void COXMDCCryptor::Crypt(LPDWORD out1,LPDWORD out2,LPCDWORD in1,LPCDWORD in2,
						  const LPDWORD key1, const LPDWORD key2) const
	// --- In  : out1: 16-byte buffers.
	//			 out2 : 16-byte buffers.
	//			 in1 : 16-byte buffers.
	//			 in2 : 16-byte buffers.
	//			 key1: 48-byte buffers 
	//			 key2 : 48-byte buffers 
	// --- Out : 
	// --- Returns :
	// --- Effect : Basic transform for Karn encryption.  Take two 16-byte
	//				 half-buffers, two 48-byte keys (which must be distinct), and use
	//				 the MD5 Transform algorithm to produce two 16-byte output
	//				 half-buffers.
    //				This is reversible: If we get out1 and out2 from in1, in2, key1, key2,
	//				 then we can get in2 and in1 from out2, out1, key1, key2.
    //				By convention, in1 and in2 are two halves of a 32-byte input
	//				 buffer, and out1 and out2 are two halves of a 32-byte output
	//				 buffer.
	//				 key1 and key2 should point to 48-byte buffers with different contents.
	{
	int i;
	DWORD buf[16];
	DWORD hash[4];
	DWORD temp[4];
	
#ifdef WIN32
	CopyMemory(hash,ihash,sizeof(hash));
	CopyMemory(buf,in1,16);
	CopyMemory(buf+4,key1,48);
#else
	hmemcpy(hash,ihash,sizeof(hash));
	hmemcpy(buf,in1,16);
	hmemcpy(buf+4,key1,48);
#endif

	Transform(hash,buf);
		
	for (i=0;i<4;++i)
		temp[i]=buf[i]=in2[i]^hash[i];
		
#ifdef WIN32
	CopyMemory(hash,ihash,sizeof(hash));
	CopyMemory(buf+4,key2,48);
#else
	hmemcpy(hash,ihash,sizeof(hash));
	hmemcpy(buf+4,key2,48);
#endif

	Transform(hash,buf);
		
	for(i=0;i<4;++i)
		out2[i]=buf[i]=in1[i]^hash[i];
		
#ifdef WIN32
	CopyMemory(hash,ihash,sizeof(hash));
	CopyMemory(buf+4,key1,48);
#else
	hmemcpy(hash,ihash,sizeof(hash));
	hmemcpy(buf+4,key1,48);
#endif
	Transform(hash,buf);
		
	for (i=0;i<4;++i)
		out1[i]=temp[i]^hash[i];
	}
   
void COXMDCCryptor::Transform (LPDWORD buf,LPDWORD in) const
	// --- In  : buf :
	//			 in :
	// --- Out : 
	// --- Returns :
	// --- Effect : Basic MD5 step. Transforms buf based on in
	{
	DWORD a = buf[0], b = buf[1], c = buf[2], d = buf[3];
	
	/* Round 1 */
	FF ( a, b, c, d, in[ 0], S11, 3614090360); /* 1 */
	FF ( d, a, b, c, in[ 1], S12, 3905402710); /* 2 */
	FF ( c, d, a, b, in[ 2], S13,  606105819); /* 3 */
	FF ( b, c, d, a, in[ 3], S14, 3250441966); /* 4 */
	FF ( a, b, c, d, in[ 4], S11, 4118548399); /* 5 */
	FF ( d, a, b, c, in[ 5], S12, 1200080426); /* 6 */
	FF ( c, d, a, b, in[ 6], S13, 2821735955); /* 7 */
	FF ( b, c, d, a, in[ 7], S14, 4249261313); /* 8 */
	FF ( a, b, c, d, in[ 8], S11, 1770035416); /* 9 */
	FF ( d, a, b, c, in[ 9], S12, 2336552879); /* 10 */
	FF ( c, d, a, b, in[10], S13, 4294925233); /* 11 */
	FF ( b, c, d, a, in[11], S14, 2304563134); /* 12 */
	FF ( a, b, c, d, in[12], S11, 1804603682); /* 13 */
	FF ( d, a, b, c, in[13], S12, 4254626195); /* 14 */
	FF ( c, d, a, b, in[14], S13, 2792965006); /* 15 */
	FF ( b, c, d, a, in[15], S14, 1236535329); /* 16 */
	
	/* Round 2 */
	GG ( a, b, c, d, in[ 1], S21, 4129170786); /* 17 */
	GG ( d, a, b, c, in[ 6], S22, 3225465664); /* 18 */
	GG ( c, d, a, b, in[11], S23, 643717713); /* 19 */
	GG ( b, c, d, a, in[ 0], S24, 3921069994); /* 20 */
	GG ( a, b, c, d, in[ 5], S21, 3593408605); /* 21 */
	GG ( d, a, b, c, in[10], S22,  38016083); /* 22 */
	GG ( c, d, a, b, in[15], S23, 3634488961); /* 23 */
	GG ( b, c, d, a, in[ 4], S24, 3889429448); /* 24 */
	GG ( a, b, c, d, in[ 9], S21, 568446438); /* 25 */
	GG ( d, a, b, c, in[14], S22, 3275163606); /* 26 */
	GG ( c, d, a, b, in[ 3], S23, 4107603335); /* 27 */
	GG ( b, c, d, a, in[ 8], S24, 1163531501); /* 28 */
	GG ( a, b, c, d, in[13], S21, 2850285829); /* 29 */
	GG ( d, a, b, c, in[ 2], S22, 4243563512); /* 30 */
	GG ( c, d, a, b, in[ 7], S23, 1735328473); /* 31 */
	GG ( b, c, d, a, in[12], S24, 2368359562); /* 32 */
	
	/* Round 3 */
	HH ( a, b, c, d, in[ 5], S31, 4294588738); /* 33 */
	HH ( d, a, b, c, in[ 8], S32, 2272392833); /* 34 */
	HH ( c, d, a, b, in[11], S33, 1839030562); /* 35 */
	HH ( b, c, d, a, in[14], S34, 4259657740); /* 36 */
	HH ( a, b, c, d, in[ 1], S31, 2763975236); /* 37 */
	HH ( d, a, b, c, in[ 4], S32, 1272893353); /* 38 */
	HH ( c, d, a, b, in[ 7], S33, 4139469664); /* 39 */
	HH ( b, c, d, a, in[10], S34, 3200236656); /* 40 */
	HH ( a, b, c, d, in[13], S31, 681279174); /* 41 */
	HH ( d, a, b, c, in[ 0], S32, 3936430074); /* 42 */
	HH ( c, d, a, b, in[ 3], S33, 3572445317); /* 43 */
	HH ( b, c, d, a, in[ 6], S34,  76029189); /* 44 */
	HH ( a, b, c, d, in[ 9], S31, 3654602809); /* 45 */
	HH ( d, a, b, c, in[12], S32, 3873151461); /* 46 */
	HH ( c, d, a, b, in[15], S33,  530742520); /* 47 */
	HH ( b, c, d, a, in[ 2], S34, 3299628645); /* 48 */
	
	/* Round 4 */
	II ( a, b, c, d, in[ 0], S41, 4096336452); /* 49 */
	II ( d, a, b, c, in[ 7], S42, 1126891415); /* 50 */
	II ( c, d, a, b, in[14], S43, 2878612391); /* 51 */
	II ( b, c, d, a, in[ 5], S44, 4237533241); /* 52 */
	II ( a, b, c, d, in[12], S41, 1700485571); /* 53 */
	II ( d, a, b, c, in[ 3], S42, 2399980690); /* 54 */
	II ( c, d, a, b, in[10], S43, 4293915773); /* 55 */
	II ( b, c, d, a, in[ 1], S44, 2240044497); /* 56 */
	II ( a, b, c, d, in[ 8], S41, 1873313359); /* 57 */
	II ( d, a, b, c, in[15], S42, 4264355552); /* 58 */
	II ( c, d, a, b, in[ 6], S43, 2734768916); /* 59 */
	II ( b, c, d, a, in[13], S44, 1309151649); /* 60 */
	II ( a, b, c, d, in[ 4], S41, 4149444226); /* 61 */
	II ( d, a, b, c, in[11], S42, 3174756917); /* 62 */
	II ( c, d, a, b, in[ 2], S43,  718787259); /* 63 */
	II ( b, c, d, a, in[ 9], S44, 3951481745); /* 64 */
	
	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
	}



BOOL COXMDCCryptor::VerifyKey(const PMDC_KEY pKey, int nStrength) const
{
	ASSERT(AfxIsValidAddress(pKey, MDC_KEY_LENGTH));
	ASSERT(MDC_KEY_LENGTH % 2 == 0);
	ASSERT(memcmp(pKey, pKey + MDC_KEY_LENGTH / 2, MDC_KEY_LENGTH / 2) != 0);
	ASSERT(nStrength>0 && nStrength<=MDC_KEY_LENGTH*8);

	// apply strength
	MDC_KEY key;
	ApplyStrength(key,pKey,nStrength);

	// check integrity
	BOOL bResult=FALSE;
	for(int nIndex=0; nIndex<MDC_KEY_LENGTH/2; nIndex++)
		if(key[nIndex]!=key[MDC_KEY_LENGTH/2+nIndex])
			{
			bResult=TRUE;
			break;
			}

	return bResult;
}

void COXMDCCryptor::ApplyStrength(MDC_KEY keyDest, MDC_KEY keySrc, 
								  int nStrength) const
{
	ASSERT(AfxIsValidAddress(keyDest, MDC_KEY_LENGTH));
	ASSERT(AfxIsValidAddress(keySrc, MDC_KEY_LENGTH));
	ASSERT(MDC_KEY_LENGTH % 2 == 0);
	ASSERT(nStrength>0 && nStrength<=MDC_KEY_LENGTH*8);

	memcpy(keyDest, keySrc, MDC_KEY_LENGTH);

	// apply strength
	for(int nIndex=nStrength/8+(nStrength%8!=0 ? 1 : 0); 
		nIndex<MDC_KEY_LENGTH; nIndex++)
		keyDest[nIndex]=0;
	keyDest[nStrength/8]&=((BYTE)0xff)<<(8-nStrength%8);

}
// private:

// Message handlers ---------------------------------------------------------

// ==========================================================================

