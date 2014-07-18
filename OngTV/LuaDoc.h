#pragma once


#include "ScintillaDocView.h"


class CLuaDoc : public CScintillaDoc
{
NON_COPYABLE(CLuaDoc);
protected: // create from serialization only
    CLuaDoc();
    DECLARE_DYNCREATE(CLuaDoc)

public:
    virtual ~CLuaDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    DECLARE_MESSAGE_MAP()
};
