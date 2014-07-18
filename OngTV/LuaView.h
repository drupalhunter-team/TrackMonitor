#pragma once

#include "ScintillaDocView.h"
#include "LuaInterpreter.h"

class CFunctionDesc;

class CLuaView : public CScintillaView
{
NON_COPYABLE(CLuaView);
protected: // create from serialization only
    CLuaView();
    DECLARE_DYNCREATE(CLuaView)

// Attributes
public:
    CLuaDoc* GetDocument();

    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual void OnInitialUpdate();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg void OnSave();
    afx_msg void OnSaveAs();
    afx_msg void OnRunScript();
    afx_msg void OnStopScript();
    afx_msg void OnCtrlSpace();
    afx_msg void OnCtrlShiftSpace();
    afx_msg void OnCtrlF3();
    afx_msg void OnShiftF3();
    afx_msg void OnGotoLine();
    afx_msg void OnOptionsTogglemarker();
    afx_msg void OnOptionsAddmarker();
    afx_msg void OnOptionsDeletemarker();
    afx_msg void OnUpdateOptionsDeletemarker(CCmdUI* pCmdUI);
    afx_msg void OnOptionsFindNextmarker();
    afx_msg void OnOptionsFindPrevmarker();
    afx_msg void OnOptionsFoldMargin();
    afx_msg void OnOptionsSelectionMargin();
    afx_msg void OnUpdateOptionsSelectionMargin(CCmdUI* pCmdUI);
    afx_msg void OnOptionsViewLinenumbers();
    afx_msg void OnUpdateOptionsViewLinenumbers(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsAddmarker(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsFoldMargin(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLine(CCmdUI* pCmdUI);
    afx_msg void OnUpdateStyle(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFold(CCmdUI* pCmdUI);
    afx_msg void OnUpdateInsert(CCmdUI* pCmdUI);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnUpdateBtSave(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBtSaveAs(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBtRunScript(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBtStopScript(CCmdUI* pCmdUI);
    afx_msg LRESULT OnExecutionEnd(WPARAM wparam, LPARAM lparam);
    afx_msg void OnDestroy();


    DECLARE_MESSAGE_MAP()
    
  virtual void OnUpdateUI(SCNotification* pSCNotification);
  virtual void OnCharAdded(SCNotification* pSCNotification);
  virtual void OnDwellStart(SCNotification* pSCNotification);
  virtual void OnDwellEnd(SCNotification* pSCNotification);
  virtual void OnModifyAttemptRO(SCNotification* pSCNotification);
  virtual void OnMarginClick(SCNotification* pSCNotification);
  virtual void Expand(int &line, bool doExpand, bool force = false, int visLevels = 0, int level = -1);
  virtual void FoldAll();

  void SetAStyle(int style, COLORREF fore, COLORREF back=RGB(0xff, 0xff, 0xff), int size=-1, const char* face=0);
    void DefineMarker(int marker, int markerType, COLORREF fore, COLORREF back);

private:
    struct Token {
	    std::string token;
	    int   length;
	    bool  isIdentifier;
	    Token(char* tok, int len, bool isID) : token(tok, len), length(len), isIdentifier(isID) {};
    };

    typedef enum {NONE=0, IF=0x1, THEN=0x2, ELSE=0x4, ELSEIF=0x8, END=0x10, FUNCTION=0x20, WHILE=0x40, FOR=0x80, DO=0x100, REPEAT=0x200, UNTIL=0x400, TC_BEGIN=0x800, TC_END=0x1000} T_STATEMENT_TYPE;
    struct Statement {
	    T_STATEMENT_TYPE type;
	    int              position;
	    int              len;
	    Statement(T_STATEMENT_TYPE typ, int pos, int l) : type(typ), position(pos), len(l) {};
    };
    T_STATEMENT_TYPE GetStatementType(const char* pStatement);

    void ProcessFunctionCallTip(void);
    void BuildTokensArray(std::vector<Token>* pArrTokens);
    CFunctionDesc* ExtractFunction(std::vector<Token>* pArrTokens, int* pParamNo);
    void ShowFunctionCalltip(long posCalltip, CFunctionDesc* pFunctionDesc, int paramNo = -1);
    void CheckBracesMatch(void);
    bool HighLightSelectedWord(void);
    void HighLightStatement(void);
    int SearchStatementBackward(std::vector<Statement>& arrStatements, int iCurrStatement, int mskSearchBackward, int level);
    int SearchStatementForward(std::vector<Statement>& arrStatements, int iCurrStatement, int mskSearchForward, int level);
    void BuildStatementsArray(std::vector<Statement>* pArrStatements, int statementStartPos, int statementEndPos);
    bool isQualifiedWord(const char *str) const;
    bool isWordChar(char ch) const;
    bool isCommentOrStringStyle(unsigned char style);
    bool isCommentStyle(unsigned char style);
    bool isStringStyle(unsigned char style);

    CLuaInterpreter m_LuaInterpreter;
};

#ifndef _DEBUG  // debug version in ScintillaDemoView.cpp
inline CLuaDoc* CLuaView::GetDocument()
   { return (CLuaDoc*)m_pDocument; }
#endif
