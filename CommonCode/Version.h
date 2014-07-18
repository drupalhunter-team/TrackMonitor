#ifndef _VERSION_H
#define _VERSION_H
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
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/Version.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//                                                                      Includes
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//                                                      Defines & Types exportes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                                           Variables exportees
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                                           Fonctions exportees
//------------------------------------------------------------------------------

class CVersion
{
NON_COPYABLE(CVersion);
public:
    CVersion(const std::string& fileFullName);
    CVersion(HMODULE hModule);
    virtual ~CVersion(void);

    const std::string& GetComments(void) {return m_strComments;};
    const std::string& GetCompanyName(void) {return m_strCompanyName;};
    const std::string& GetFileDescrib(void) {return m_strFileDescrib;};
    const std::string& GetFileVersion(void) {return m_strFileVersion;};
    const std::string& GetInternalName(void) {return m_strInternalName;};
    const std::string& GetLegalCopyright(void) {return m_strLegalCopyright;};
    const std::string& GetLegalTrademarks(void) {return m_strLegalTrademarks;};
    const std::string& GetOriginalFilename(void) {return m_strOriginalFilename;};
    const std::string& GetProductName(void) {return m_strProductName;};
    const std::string& GetProductVersion(void) {return m_strProductVersion;};
    const std::string& GetPrivateBuild(void) {return m_strPrivateBuild;};
    const std::string& GetSpecialBuild(void) {return m_strSpecialBuild;};
    const std::string& GetFileFullName(void) {return m_strFileFullName;};
private:
    void ReadInfos(void);
    std::string  m_strFileFullName;
    std::string  m_strComments;
    std::string  m_strCompanyName;
    std::string  m_strFileDescrib;
    std::string  m_strFileVersion;
    std::string  m_strInternalName;
    std::string  m_strLegalCopyright;
    std::string  m_strLegalTrademarks;
    std::string  m_strOriginalFilename;
    std::string  m_strProductName;
    std::string  m_strProductVersion;
    std::string  m_strPrivateBuild;
    std::string  m_strSpecialBuild;
};

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/Version.h $
////////////////////////////////////////////////////////////////////////////////
///
#endif