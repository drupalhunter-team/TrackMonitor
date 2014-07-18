// OngTVDoc.cpp : implémentation de la classe COngTVDoc de la classe COngTVDocManager
//

#include "stdafx.h"
#include "OngTV.h"

#include "OngTVDoc.h"
#include "PathMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COngTVDoc

IMPLEMENT_DYNCREATE(COngTVDoc, CDocument)

BEGIN_MESSAGE_MAP(COngTVDoc, CDocument)
END_MESSAGE_MAP()


// construction ou destruction de COngTVDoc

COngTVDoc::COngTVDoc()
{
    // TODO : ajoutez ici le code d'une construction unique

}

COngTVDoc::~COngTVDoc()
{
}

BOOL COngTVDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO : ajoutez ici le code de réinitialisation
    // (les documents SDI réutiliseront ce document)

    return TRUE;
}




// sérialisation de COngTVDoc

void COngTVDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO : ajoutez ici le code de stockage
    }
    else
    {
        // TODO : ajoutez ici le code de chargement
    }
}


// diagnostics pour COngTVDoc

#ifdef _DEBUG
void COngTVDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void COngTVDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// COngTVDocManager

IMPLEMENT_DYNAMIC(COngTVDocManager, CDocManager)

COngTVDocManager::COngTVDocManager()
{
}

COngTVDocManager::~COngTVDocManager()
{
}

// code repris des MFC
static void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	CDocTemplate* pTemplate, CString* pstrDefaultExt)
{
	ENSURE_VALID(pTemplate);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	CString strFilterExt, strFilterName;
	if (pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt) &&
		!strFilterExt.IsEmpty() &&
		pTemplate->GetDocString(strFilterName, CDocTemplate::filterName) &&
		!strFilterName.IsEmpty())
	{
		if (pstrDefaultExt != NULL)
			pstrDefaultExt->Empty();

		// add to filter
		filter += strFilterName;
		ASSERT(!filter.IsEmpty());  // must have a file type name
		filter += (TCHAR)'\0';  // next string please

		int iStart = 0;
		do
		{
			CString strExtension = strFilterExt.Tokenize( _T( ";" ), iStart );

			if (iStart != -1)
			{
				// a file based document template - add to filter list

				// If you hit the following ASSERT, your document template 
				// string is formatted incorrectly.  The section of your 
				// document template string that specifies the allowable file
				// extensions should be formatted as follows:
				//    .<ext1>;.<ext2>;.<ext3>
				// Extensions may contain wildcards (e.g. '?', '*'), but must
				// begin with a '.' and be separated from one another by a ';'.
				// Example:
				//    .jpg;.jpeg
				ASSERT(strExtension[0] == '.');
				if ((pstrDefaultExt != NULL) && pstrDefaultExt->IsEmpty())
				{
					// set the default extension
					*pstrDefaultExt = strExtension.Mid( 1 );  // skip the '.'
					ofn.lpstrDefExt = const_cast< LPTSTR >((LPCTSTR)(*pstrDefaultExt));
					ofn.nFilterIndex = ofn.nMaxCustFilter + 1;  // 1 based number
				}

				filter += (TCHAR)'*';
				filter += strExtension;
				filter += (TCHAR)';';  // Always append a ';'.  The last ';' will get replaced with a '\0' later.
			}
		} while (iStart != -1);

		filter.SetAt( filter.GetLength()-1, '\0' );;  // Replace the last ';' with a '\0'
		++ofn.nMaxCustFilter;
	}
}

// Code repris des MFCs
// Modifie pour gerer les répertoires initiaux :
// On sauvegarde (classe CPathMgr), les associations <répertoire>/<suffixe de fichier> lors de chaque demande d'un nom de fichier
// La méthode DoPromptFileName de CDocManager est surchargee afin :
// 1. de positionner le repertoire initial correspondant au suffixe des documents geres par le doc template passe en parametre
//    avant l'execution de la boite de dialogue de saisie d'un nom de fichier
// 2. de sauvegarder le path du fichier saisi (en l'associant à ce suffixe)

BOOL COngTVDocManager::DoPromptFileName(CString& fileName, UINT nIDSTitleUnused, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
{
    DWORD dwFlags;
    if (bOpenFileDialog)
    {
        dwFlags = OFN_FILEMUSTEXIST;
    }
    else
    {
        dwFlags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
    }
    CFileDialog dlgFile(bOpenFileDialog, NULL, NULL, dwFlags, NULL, NULL, 0);

//    CString title;
//    VERIFY(title.LoadString(nIDSTitle));

    dlgFile.m_ofn.Flags |= lFlags;
    
    CString strFilter;
    CString strDefault;
    std::string strSuffix;
    std::string strDirectory;
    if (pTemplate != NULL)
    {
        ASSERT_VALID(pTemplate);
        AppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate, &strDefault);
        
        // Modif OngTV, on recupere le repertoire correspondant au suffixe pour en faire le repertoire par defaut
        CString strSuffixExt;
        pTemplate->GetDocString(strSuffixExt, CDocTemplate::filterExt);
        strSuffix = (LPCSTR)strSuffixExt;
        // on enleve le .
        size_t pos = strSuffix.rfind(".");
        if (pos != std::string::npos)
        {
            strSuffix = strSuffix.substr(pos+1);
        }
        // On recupere le repertoire associe a ce suffixe
        strDirectory = CPathMgr::ReadSingleInstance()->GetSuffixDirectory(strSuffix);
        if (0 != strDirectory.size())
        {
            // s'il n'est pas vide, on en fait le repertoire de depart
            dlgFile.m_ofn.lpstrInitialDir = strDirectory.c_str();
        }
    }
    else
    {
        // do for all doc template
        POSITION pos = m_templateList.GetHeadPosition();
        BOOL bFirst = TRUE;
        while (pos != NULL)
        {
            pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
            AppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate,
                bFirst ? &strDefault : NULL);
            bFirst = FALSE;
        }
    }

    // append the "*.*" all files filter
    CString allFilter;
    VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
    strFilter += allFilter;
    strFilter += (TCHAR)'\0';   // next string please
    strFilter += _T("*.*");
    strFilter += (TCHAR)'\0';   // last string
    ++dlgFile.m_ofn.nMaxCustFilter;

    dlgFile.m_ofn.lpstrFilter = strFilter;
//    dlgFile.m_ofn.lpstrTitle = title;
    dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

    INT_PTR nResult = dlgFile.DoModal();
    fileName.ReleaseBuffer();
    
    // Modif OngTV => On modifie le repertoire associe au suffixe
    if ((IDOK == nResult) && (0 != strSuffix.size()))
    {
        // on recupere le repertoire selectionne
        strDirectory = (LPCSTR)fileName;
        strDirectory = CPathMgr::ExtractPath(strDirectory);
        CPathMgr::ReadSingleInstance()->SetSuffixDirectory(strSuffix, strDirectory);
    }
    return nResult == IDOK;
}