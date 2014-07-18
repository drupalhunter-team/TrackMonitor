////////////////////////////////////////////////////////////////////////////////
/// @file 
/// 
///
///  
///  \n \n <b>Copyright ADENEO 2006</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/Version.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//                                                                      Includes
//------------------------------------------------------------------------------
#include <string>
#include <afxwin.h>
#include "AdeNonCopyable.h"

#include "version.h"

//------------------------------------------------------------------------------
//                                                               Defines & Types
//------------------------------------------------------------------------------


struct LANGANDCODEPAGE {
WORD wLanguage;
WORD wCodePage;
};

#define VERINFO_Comments         "Comments"
#define VERINFO_CompanyName      "CompanyName"
#define VERINFO_FileDescrib      "FileDescription"
#define VERINFO_FileVersion      "FileVersion"
#define VERINFO_InternalName     "InternalName"
#define VERINFO_LegalCopyright   "LegalCopyright"
#define VERINFO_LegalTrademarks  "LegalTrademarks"
#define VERINFO_OriginalFilename "OriginalFilename"
#define VERINFO_ProductName      "ProductName"
#define VERINFO_ProductVersion   "ProductVersion"
#define VERINFO_PrivateBuild     "PrivateBuild"
#define VERINFO_SpecialBuild     "SpecialBuild"

//------------------------------------------------------------------------------
//                                                                     Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                                           Fonctions exportees
//------------------------------------------------------------------------------

std::string GetFileVersionInfoString(LPCSTR m_strFileFullName, LPCSTR verInfoString)
{
    char* rcData = 0; 
    DWORD dwSize;
    UINT wSize;
    LANGANDCODEPAGE *lpTranslate;

    LPTSTR fileName = (LPTSTR)m_strFileFullName;

    //file name and path
    CFileStatus status;
    std::string verInfo = "Unavailable";
    if( CFile::GetStatus(fileName, status ) )
    {
        //Version
        LPDWORD handle1 = 0;
        dwSize = ::GetFileVersionInfoSize(fileName, handle1);
        if (! dwSize)
            return verInfo;

        rcData = new char[dwSize];

        // get valid resource data using path, handle, and size
        int result = ::GetFileVersionInfo(fileName, 0, dwSize, (LPVOID)rcData);
        if (result)
        {
            result = ::VerQueryValue(rcData,TEXT("\\VarFileInfo\\Translation" ),
            (LPVOID*)&lpTranslate, &wSize);
            if (result == 0 || wSize == 0)
            {
                delete []rcData;
                return verInfo;
            }
        }
        // Read the file description for each language and code page.
        char SubBlock[100];
        LPVOID lpBuffer;
        for( unsigned i=0; i < (wSize/sizeof(struct LANGANDCODEPAGE)); ++i )
            {
            wsprintf( SubBlock, 
            TEXT("\\StringFileInfo\\%04x%04x\\%s"),
            lpTranslate[i].wLanguage,lpTranslate[i].wCodePage,verInfoString);

            result = VerQueryValue(rcData, SubBlock, (LPVOID*)&lpBuffer, &wSize);
            if (!result) 
                break;
                verInfo = (LPCSTR)lpBuffer;
        }
        delete []rcData;
    }

    return verInfo;
}

//------------------------------------------------------------------CVersion::CVersion
//
// Description    :    Constructeur
//
//-----------------------------------------------------------------------------
CVersion::CVersion(const std::string& fileFullName) : m_strFileFullName(fileFullName)
{
    ReadInfos();
}
CVersion::CVersion(HMODULE hModule)
{
    // recuperation des infos de version de l'exe
    char lpFilename[255];
    memset(lpFilename,0,255);
    GetModuleFileName(hModule ,lpFilename, 255);
    m_strFileFullName = lpFilename;
    ReadInfos();
}

//------------------------------------------------------------------CVersion::CVersion
//
// Description    :    Constructeur
//
// Parametres     :    Aucun
//
// Retour         :    Aucun
//-----------------------------------------------------------------------------
void CVersion::ReadInfos(void)
{
    m_strComments           = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_Comments        );
    m_strCompanyName        = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_CompanyName     );
    m_strFileDescrib        = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_FileDescrib     );
    m_strFileVersion        = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_FileVersion     );
    m_strInternalName       = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_InternalName    );
    m_strLegalCopyright     = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_LegalCopyright  );
    m_strLegalTrademarks    = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_LegalTrademarks );
    m_strOriginalFilename   = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_OriginalFilename);
    m_strProductName        = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_ProductName     );
    m_strProductVersion     = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_ProductVersion  );
    m_strPrivateBuild       = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_PrivateBuild    );
    m_strSpecialBuild       = GetFileVersionInfoString(m_strFileFullName.c_str(), VERINFO_SpecialBuild    );
}

//----------------------------------------------------------------CVersion::~CVersion
//
// Description    :    Destructeur
//
// Parametres    :    Aucun
//
// Retour        :    Aucun
//-----------------------------------------------------------------------------
CVersion::~CVersion(void)
{
}


////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/Version.cpp $
////////////////////////////////////////////////////////////////////////////////
///
