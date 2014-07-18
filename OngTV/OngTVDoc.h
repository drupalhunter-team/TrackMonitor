// OngTVDoc.h : interfaces de la classe COngTVDoc et de la classe COngTVDocManager
//


#pragma once

class COngTVDoc : public CDocument
{
NON_COPYABLE(COngTVDoc);
protected: // création à partir de la sérialisation uniquement
    COngTVDoc();
    DECLARE_DYNCREATE(COngTVDoc)

// Attributs
public:

// Opérations
public:

// Substitutions
public:
    // On rend public les fonctions protegees OnFileSave et OnFileSaveAs pour profiter de leur
    // comportement
    void PublicOnFileSave(void) {OnFileSave();};
    void PublicOnFileSaveAs(void) {OnFileSaveAs();};

    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

// Implémentation
public:
    virtual ~COngTVDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions générées de la table des messages
protected:
    DECLARE_MESSAGE_MAP()
};

// surcharge du manager de doc afin de gerer specifiquement les repertoires
// de sauvegarde des diffents documents.
// cf http://www.codeguru.com/cpp/w-d/dislog/commondialogs/article.php/c1967
class COngTVDocManager : public CDocManager
{
NON_COPYABLE(COngTVDocManager);
    DECLARE_DYNAMIC(COngTVDocManager)

// Construction
public:
    COngTVDocManager();

// Attributes
public:

// Operations
public:

// Overrides
// helper for standard commdlg dialogs
    virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitleUnused,
                DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);

// Implementation
public:
    virtual ~COngTVDocManager();
};
