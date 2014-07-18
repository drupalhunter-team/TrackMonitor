#include "stdafx.h"
#include "LuaDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CLuaDoc, CScintillaDoc)

BEGIN_MESSAGE_MAP(CLuaDoc, CScintillaDoc)
END_MESSAGE_MAP()

CLuaDoc::CLuaDoc()
{
}

CLuaDoc::~CLuaDoc()
{
}

#ifdef _DEBUG
void CLuaDoc::AssertValid() const
{
    CScintillaDoc::AssertValid();
}

void CLuaDoc::Dump(CDumpContext& dc) const
{
    CScintillaDoc::Dump(dc);
}
#endif //_DEBUG
