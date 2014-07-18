#include "stdafx.h"
#include "OngTV.h"
#include "resource.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include "dlgGetValNum.h"
#include "utils.h"
#include "LuaEditorParamMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SCE_UNIVERSAL_FOUND_STYLE_SMART 31

#define WM_INTERPRETER_END (WM_USER+1) // Message signalant à l'IHM que l'execution du script est terminee

static void NotificationCallback(LPVOID lpParam, CLuaInterpreter::T_INTERPRETER_EVENT evt)
{
    HWND hWindow = (HWND)lpParam;
    switch (evt)
    {
    case CLuaInterpreter::EVT_END:
        ::PostMessage(hWindow, WM_INTERPRETER_END, 0, 0);
        break;
    default:
        break;
    }

}

IMPLEMENT_DYNCREATE(CLuaView, CScintillaView)

BEGIN_MESSAGE_MAP(CLuaView, CScintillaView)
    ON_COMMAND(ID_LUASCRIPT_RUN,    OnRunScript)
    ON_COMMAND(ID_LUASCRIPT_STOP,   OnStopScript)
    ON_COMMAND(ID_LUASCRIPT_SAVE,   OnSave)
    ON_COMMAND(ID_LUASCRIPT_SAVEAS, OnSaveAs)
    ON_COMMAND(ID_OPTIONS_TOGGLEMARKER, OnOptionsTogglemarker)
//    ON_COMMAND(ID_OPTIONS_DELETEMARKER, OnOptionsDeletemarker)
    ON_COMMAND(ID_OPTIONS_FIND_NEXTMARKER, OnOptionsFindNextmarker)
    ON_COMMAND(ID_OPTIONS_FIND_PREVMARKER, OnOptionsFindPrevmarker)
    ON_COMMAND(ID_LUASCRIPT_CTRL_F3,       OnCtrlF3)
    ON_COMMAND(ID_LUASCRIPT_CTRL_SPACE,    OnCtrlSpace)
    ON_COMMAND(ID_LUASCRIPT_CTRL_SHIFT_SPACE, OnCtrlShiftSpace)
    ON_COMMAND(ID_LUASCRIPT_SHIFT_F3,      OnShiftF3)
    ON_COMMAND(ID_LUASCRIPT_GOTO_LINE,     OnGotoLine)
//    ON_UPDATE_COMMAND_UI(ID_OPTIONS_DELETEMARKER, OnUpdateOptionsDeletemarker)
//    ON_UPDATE_COMMAND_UI(ID_OPTIONS_ADDMARKER, OnUpdateOptionsAddmarker)
#if 0
    ON_COMMAND(ID_OPTIONS_FOLD_MARGIN, OnOptionsFoldMargin)
    ON_COMMAND(ID_OPTIONS_SELECTION_MARGIN, OnOptionsSelectionMargin)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_SELECTION_MARGIN, OnUpdateOptionsSelectionMargin)
    ON_COMMAND(ID_OPTIONS_VIEW_LINENUMBERS, OnOptionsViewLinenumbers)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIEW_LINENUMBERS, OnUpdateOptionsViewLinenumbers)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_FOLD_MARGIN, OnUpdateOptionsFoldMargin)
  ON_UPDATE_COMMAND_UI(ID_INDICATOR_LINE, OnUpdateLine) 
  ON_UPDATE_COMMAND_UI(ID_INDICATOR_STYLE, OnUpdateStyle) 
  ON_UPDATE_COMMAND_UI(ID_INDICATOR_FOLD, OnUpdateFold) 
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateInsert)
#endif
    ON_UPDATE_COMMAND_UI(ID_LUASCRIPT_RUN,    OnUpdateBtRunScript)
    ON_UPDATE_COMMAND_UI(ID_LUASCRIPT_STOP,   OnUpdateBtStopScript)
    ON_UPDATE_COMMAND_UI(ID_LUASCRIPT_SAVE,   OnUpdateBtSave)
    ON_UPDATE_COMMAND_UI(ID_LUASCRIPT_SAVEAS, OnUpdateBtSaveAs)
    ON_WM_ACTIVATE()
    ON_MESSAGE(WM_INTERPRETER_END, OnExecutionEnd)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


CLuaView::CLuaView()
{
  LoadMarginSettings();
}

void CLuaView::OnDraw(CDC* /*pDC*/)
{
    CLuaDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
}

#ifdef _DEBUG
void CLuaView::AssertValid() const
{
    CScintillaView::AssertValid();
}

void CLuaView::Dump(CDumpContext& dc) const
{
    CScintillaView::Dump(dc);
}

CLuaDoc* CLuaView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLuaDoc)));
    return static_cast<CLuaDoc*>(m_pDocument);
}
#endif //_DEBUG

void CLuaView::OnSave()
{
    ((CLuaDoc*)GetDocument())->PublicOnFileSave();
}

void CLuaView::OnSaveAs()
{
    ((CLuaDoc*)GetDocument())->PublicOnFileSaveAs();
}



void CLuaView::SetAStyle(int style, COLORREF fore, COLORREF back, int size, const char* face) 
{
  CScintillaCtrl& rCtrl = GetCtrl();

    rCtrl.StyleSetFore(style, fore);
    rCtrl.StyleSetBack(style, back);
    if (size >= 1)
        rCtrl.StyleSetSize(style, size);
    if (face) 
        rCtrl.StyleSetFont(style, face);
}

void CLuaView::DefineMarker(int marker, int markerType, COLORREF fore, COLORREF back) 
{
  CScintillaCtrl& rCtrl = GetCtrl();

    rCtrl.MarkerDefine(marker, markerType);
    rCtrl.MarkerSetFore(marker, fore);
    rCtrl.MarkerSetBack(marker, back);
}
void CLuaView::OnInitialUpdate() 
{
  //Let the base class do its thing
    CScintillaView::OnInitialUpdate();
    
    CScintillaCtrl& rCtrl = GetCtrl();

    //Setup the Lexer
    rCtrl.SetLexer(SCLEX_LUA);
    // Note d'apres LexLua.cxx
    // Keyword 0 => SCE_LUA_WORD => 5   => SCE_C_WORD
    // Keyword 1 => SCE_LUA_WORD2 => 13 => SCE_C_VERBATIM
    // Keyword 2 => SCE_LUA_WORD3 => 14 => SCE_C_REGEX
    rCtrl.SetKeyWords(0, CLuaEditorParamMgr::ReadSingleInstance()->GetLuaKeyWords());
    rCtrl.SetKeyWords(1, CLuaEditorParamMgr::ReadSingleInstance()->GetLuaFunctions());
    rCtrl.SetKeyWords(2, CLuaEditorParamMgr::ReadSingleInstance()->GetLuaConstants());

    SetAStyle(STYLE_DEFAULT, RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), 10, "Dina");
    rCtrl.StyleClearAll();
    // set up basic features (iguides on, tab=4, tabs-to-spaces, EOL=CRLF)
    rCtrl.SetTabWidth(4);
    rCtrl.SetUseTabs(false);
    rCtrl.SetEOLMode(SC_EOL_CRLF);
    rCtrl.SetIndentationGuides(1);


    SetAStyle(SCE_C_DEFAULT, RGB(0, 0, 0));
    SetAStyle(SCE_C_COMMENT, RGB(0, 0xAA, 0));
    SetAStyle(SCE_C_COMMENTLINE, RGB(0, 0xAA, 0));
    SetAStyle(SCE_C_COMMENTDOC, RGB(0x7F, 0x7F, 0x7F));
    // style 4: numbers (red)
    SetAStyle(SCE_C_NUMBER, RGB(255, 0, 0));
   // style 5: keywords (blue)
    SetAStyle(SCE_C_WORD, RGB(0, 0, 255));
    SetAStyle(SCE_C_STRING, RGB(0x80, 0, 0x80));
    SetAStyle(SCE_C_IDENTIFIER, RGB(0, 0, 0));
    SetAStyle(SCE_C_PREPROCESSOR, RGB(0x80, 0, 0));

    SetAStyle(SCE_C_OPERATOR, RGB(0, 0, 255));

    SetAStyle(SCE_LUA_WORD2, RGB(0, 127, 191));        // lua functions 
    SetAStyle(SCE_LUA_WORD3, RGB(0x80, 0, 0x80));      // lua constants
    
    // pour la mise en evidence des parentheses
    SetAStyle(STYLE_BRACELIGHT, RGB(255, 0, 0));
    rCtrl.StyleSetBold(STYLE_BRACELIGHT, TRUE);
    SetAStyle(STYLE_BRACEBAD, RGB(0, 0, 255));
    rCtrl.StyleSetBold(STYLE_BRACEBAD, TRUE);

	rCtrl.IndicSetStyle(SCE_UNIVERSAL_FOUND_STYLE_SMART, INDIC_ROUNDBOX);


    //Setup folding
    rCtrl.SetMarginWidthN(0, 40);   // permet d'afficher 4 chiffres
    rCtrl.SetMarginTypeN(0, SC_MARGIN_NUMBER);
    rCtrl.SetMarginWidthN(1, 5);    // largeur de la deuxieme marge = 5 pixels (pour decoller les n° de ligne du texte

    rCtrl.SetMarginWidthN(2, 16);
    rCtrl.SetMarginSensitiveN(2, TRUE);
    rCtrl.SetMarginTypeN(2, SC_MARGIN_SYMBOL);
    rCtrl.SetMarginMaskN(2, SC_MASK_FOLDERS);
    rCtrl.SetProperty(_T("fold"), _T("1"));

    //Setup markers
    DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0xFF));
    DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_PLUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
    DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
    DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
    DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
    DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
    DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));

    //Setup call tips
    rCtrl.SetMouseDwellTime(1000);

    // Positionne la facon dont l'editeur va voir les mots
    // on ajoute le '.' au set par defaut (alphanum + '_') de facon a ce que l'autocompletion fonctionne sur
    // les fonctions telle que math.abs
    rCtrl.SetWordChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.");
    
    // sur appui d'une parenthese l'autocompletion selectionne automatiquement l'item courant dans la liste
    // proposee
    rCtrl.AutoCSetFillUps("(");

    // separateur utilise dans la liste des mots clefs pour l'autocompletion
    rCtrl.AutoCSetSeparator(' ');
    // On ignore la distinction majuscule/minuscule (ce qui suppose que la liste des mots clefs
    // soit triee en ignorant la casse - cf tri utilise dans CLuaEditorParamMgr::Init
    rCtrl.AutoCSetIgnoreCase(TRUE);
    // on regle le nombre de lignes max de la liste affichees lors de l'autocompletion (si il y a plus
    // de mots clefs que de lignes, une scrollbar verticale est activee)
    rCtrl.AutoCSetMaxHeight(CLuaEditorParamMgr::ReadSingleInstance()->GetMaxLinesInAutoCompletionList());

#ifdef _DEBUG
  AfxDump(this);
#endif
}
void CLuaView::OnDestroy()
{
    // arret du sequenceur s'il est en route et attente de la fin d'execution
    if (m_LuaInterpreter.IsRunning())
    {
        m_LuaInterpreter.Stop();
        while (m_LuaInterpreter.IsRunning())
        {
            Sleep(100);
        }
    }
    CScintillaView::OnDestroy();
}

void CLuaView::OnRunScript() 
{
    if (m_LuaInterpreter.IsRunning())
    {
        // pas possible si deja en fonctionnement
        return;
    }
    CScintillaCtrl& rCtrl = GetCtrl();

    // on passe en ReadOnly
    rCtrl.SetReadOnly(true);
    // on recupere le code
    int codeLen = rCtrl.GetLength() + 1;
    char* code = new char[codeLen];
    rCtrl.GetText(codeLen, code);
    m_LuaInterpreter.StartFromBuffer(code, (char*)(LPCSTR)GetDocument()->GetTitle(), NotificationCallback, GetSafeHwnd());
    delete[] code;
}
void CLuaView::OnStopScript() 
{
    if (!m_LuaInterpreter.IsRunning())
    {
        // pas possible si pas en fonctionnement
        return;
    }
    m_LuaInterpreter.Stop();
}
// A la fin de l'execution
LRESULT CLuaView::OnExecutionEnd(WPARAM /*wparam*/, LPARAM /*lparam*/)
{
    // on reautorise l'edition
    CScintillaCtrl& rCtrl = GetCtrl();
    rCtrl.SetReadOnly(false);
    return 0;
}

void CLuaView::OnCtrlF3() 
{
    // on recherche le texte selectionne
    CScintillaCtrl& rCtrl = GetCtrl();
    CString strFind(rCtrl.GetSelText());
    // if selection is empty or spans multiple lines => on ne fait rien
    if (strFind.IsEmpty() || (strFind.FindOneOf(_T("\n\r")) != -1))
    {
        return;
    }
    OnFindNext(strFind, false);
}

void CLuaView::OnShiftF3() 
{
    // on recherche le texte selectionne
    CScintillaCtrl& rCtrl = GetCtrl();
    CString strFind(rCtrl.GetSelText());
    // if selection is empty or spans multiple lines => on ne fait rien
    if (strFind.IsEmpty() || (strFind.FindOneOf(_T("\n\r")) != -1))
    {
        return;
    }
    OnFindNext(strFind, true);
}

// commande appelee sur un ctrl G
void CLuaView::OnGotoLine() 
{
    // On demande la valeur a ecrire
    CDlgGetValNum dlgGetVal(theApp.LoadString(STR_LINE_NUMBER));

    if (dlgGetVal.DoModal() != IDOK)
    {
        return;
    }

    unsigned long lineNumber;
    // on convertit la valeur en un long
    if (CnvStrToULong(std::string(LPCSTR(dlgGetVal.m_szValue)), &lineNumber) == false)
    {
        // erreur de conversion
        MessageBox(theApp.LoadString(STR_ERROR_BAD_VALUE), theApp.LoadString(STR_ERROR), MB_ICONEXCLAMATION);
        return;
    }

    // On va a la ligne demandee
    CScintillaCtrl& rCtrl = GetCtrl();
    rCtrl.GotoLine(lineNumber-1);   // les lignes sont numerotees à partir de 0
}

void CLuaView::OnOptionsTogglemarker() 
{
  CScintillaCtrl& rCtrl = GetCtrl();
  int nPos = rCtrl.GetCurrentPos();
  int nLine = rCtrl.LineFromPosition(nPos);
  int nBits = rCtrl.MarkerGet(nLine);
  if (nBits & 0x1)
  {
    rCtrl.MarkerDelete(nLine, 0);
  }
  else
  {
    rCtrl.MarkerAdd(nLine, 0);
  }
}

void CLuaView::OnOptionsAddmarker() 
{
  CScintillaCtrl& rCtrl = GetCtrl();
  int nPos = rCtrl.GetCurrentPos();
  int nLine = rCtrl.LineFromPosition(nPos);
    rCtrl.MarkerAdd(nLine, 0);
}

void CLuaView::OnUpdateOptionsAddmarker(CCmdUI* pCmdUI) 
{
  CScintillaCtrl& rCtrl = GetCtrl();
  int nPos = rCtrl.GetCurrentPos();
  int nLine = rCtrl.LineFromPosition(nPos);
  int nBits = rCtrl.MarkerGet(nLine);
  pCmdUI->Enable((nBits & 0x1) == 0);    
}

void CLuaView::OnOptionsDeletemarker() 
{
  CScintillaCtrl& rCtrl = GetCtrl();
  int nPos = rCtrl.GetCurrentPos();
  int nLine = rCtrl.LineFromPosition(nPos);
  rCtrl.MarkerDelete(nLine, 0);
}

void CLuaView::OnUpdateOptionsDeletemarker(CCmdUI* pCmdUI) 
{
  CScintillaCtrl& rCtrl = GetCtrl();
  int nPos = rCtrl.GetCurrentPos();
  int nLine = rCtrl.LineFromPosition(nPos);
  int nBits = rCtrl.MarkerGet(nLine);
  pCmdUI->Enable(nBits & 0x1);    
}

void CLuaView::OnOptionsFindNextmarker() 
{
  CScintillaCtrl& rCtrl = GetCtrl();
  int nPos = rCtrl.GetCurrentPos();
  int nLine = rCtrl.LineFromPosition(nPos);
  int nFoundLine = rCtrl.MarkerNext(nLine + 1, 0x1);
  if (nFoundLine >= 0)
    rCtrl.GotoLine(nFoundLine);
  else
  {
      // on commence du debut
      nFoundLine = rCtrl.MarkerNext(0, 0x1);
      if (nFoundLine >= 0)
        rCtrl.GotoLine(nFoundLine);
      else
        MessageBeep(MB_ICONHAND);
  }
}

void CLuaView::OnOptionsFindPrevmarker() 
{
  CScintillaCtrl& rCtrl = GetCtrl();
  int nPos = rCtrl.GetCurrentPos();
  int nLine = rCtrl.LineFromPosition(nPos);
  int nFoundLine = rCtrl.MarkerPrevious(nLine - 1, 0x1);
  if (nFoundLine >= 0)
    rCtrl.GotoLine(nFoundLine);
  else
    MessageBeep(MB_ICONHAND);
}

void CLuaView::OnOptionsFoldMargin() 
{
  CScintillaCtrl& rCtrl = GetCtrl();
    int nMarginWidth = rCtrl.GetMarginWidthN(2);
  if (nMarginWidth)
    rCtrl.SetMarginWidthN(2, 0);
  else
      rCtrl.SetMarginWidthN(2, 16);
}

void CLuaView::OnUpdateOptionsFoldMargin(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(GetCtrl().GetMarginWidthN(2) != 0);
}

void CLuaView::OnOptionsSelectionMargin() 
{
  CScintillaCtrl& rCtrl = GetCtrl();
    int nMarginWidth = rCtrl.GetMarginWidthN(1);
  if (nMarginWidth)
    rCtrl.SetMarginWidthN(1, 0);
  else
      rCtrl.SetMarginWidthN(1, 16);
}

void CLuaView::OnUpdateOptionsSelectionMargin(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(GetCtrl().GetMarginWidthN(1) != 0);
}

void CLuaView::OnOptionsViewLinenumbers() 
{
  CScintillaCtrl& rCtrl = GetCtrl();
    int nMarginWidth = rCtrl.GetMarginWidthN(0);
  if (nMarginWidth)
    rCtrl.SetMarginWidthN(0, 0);
  else
      rCtrl.SetMarginWidthN(0, 32);
}

void CLuaView::OnUpdateOptionsViewLinenumbers(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(GetCtrl().GetMarginWidthN(0) != 0);
}

void CLuaView::OnUpdateInsert(CCmdUI* pCmdUI)
{
  CString sText;
  if (GetCtrl().GetOvertype())
    sText.LoadString(ID_INDICATOR_OVR);
    pCmdUI->SetText(sText);
}

void CLuaView::OnUpdateStyle(CCmdUI* pCmdUI)
{
   CScintillaCtrl& rCtrl = GetCtrl();
   int nPos = rCtrl.GetCurrentPos();
   int nStyle = rCtrl.GetStyleAt(nPos);
   CString sLine;
   sLine.Format(IDS_STYLE_INDICATOR, nStyle);
   pCmdUI->SetText(sLine);
} 

void CLuaView::OnUpdateFold(CCmdUI* pCmdUI)
{
  CScintillaCtrl& rCtrl = GetCtrl();
  int nPos = rCtrl.GetCurrentPos();
  int nLine = rCtrl.LineFromPosition(nPos);
  int nLevel = rCtrl.GetFoldLevel(nLine) & SC_FOLDLEVELNUMBERMASK;
  nLevel -= 1024;

  CString sLine;
  sLine.Format(IDS_FOLD_INDICATOR, nLevel);
  pCmdUI->SetText(sLine);
} 

void CLuaView::OnUpdateLine(CCmdUI* pCmdUI)
{
  CScintillaCtrl& rCtrl = GetCtrl();
  int nPos = rCtrl.GetCurrentPos();
  int nLine = rCtrl.LineFromPosition(nPos);
  int nColumn = rCtrl.GetColumn(nPos);

  CString sLine;
  sLine.Format(IDS_LINE_INDICATOR, nLine, nColumn, nPos);
  pCmdUI->SetText(sLine);
} 

// autocompletion de fonction sur Ctrl + Shift + Space
void CLuaView::OnCtrlShiftSpace() 
{
    CScintillaCtrl& rCtrl = GetCtrl();
    if (rCtrl.CallTipActive() != 0)
    {
        // deja actif, on ne fait rien
        return;
    }
    ProcessFunctionCallTip();
}

// autocompletion sur Ctrl + Space
void CLuaView::OnCtrlSpace() 
{
    CScintillaCtrl& rCtrl = GetCtrl();
  
    // If autocomplete already active, let Scintilla handle it
    if (rCtrl.AutoCActive() != 0)
    {
        return;
    }

    int caretPos = rCtrl.GetCurrentPos();
    int startPos = static_cast<int>(rCtrl.WordStartPosition(caretPos, true));
    char s[255];
    s[0] = '\0';
    if ((caretPos - startPos) < sizeof(s))
    {
        TextRange tr;
        tr.chrg.cpMin = startPos;
        tr.chrg.cpMax = caretPos;
        tr.lpstrText = s;
        rCtrl.GetTextRange(&tr);
    }
    rCtrl.AutoCShow(strlen(s), CLuaEditorParamMgr::ReadSingleInstance()->GetLuaAutoCList());
}

// autocompletion quand le nombre de caracteres suffisants
void CLuaView::OnCharAdded(SCNotification* pSCNotification)
{
    CScintillaCtrl& rCtrl = GetCtrl();
    
    if (rCtrl.CallTipActive() || ('(' == pSCNotification->ch))
    {
        // mise a jour de l'autocompletion de fonction si elle est deja active (CallTip actif)
        // ou sur une parenthese ouvrante
        ProcessFunctionCallTip();
    }
    if (rCtrl.CallTipActive())
    {
        // calltip actif => on sort
        return;
    }
  
    if (!pSCNotification->ch)
    {
        return;
    }

    // If autocomplete already active, let Scintilla handle it
    if (rCtrl.AutoCActive() != 0)
    {
        return;
    }

    char s[255];
    int caretPos = rCtrl.GetCurrentPos();
    int startPos = static_cast<int>(rCtrl.WordStartPosition(caretPos, true));
    // on verifie qu'on n'est pas dans un commentaire ou une string
    if (isCommentOrStringStyle(rCtrl.GetStyleAt(startPos)))
    {
        // on est dans un commentaire ou une string, on ne fait rien
        return;
    }
    s[0] = '\0';
    if ((caretPos - startPos) < sizeof(s))
    {
        TextRange tr;
        tr.chrg.cpMin = startPos;
        tr.chrg.cpMax = caretPos;
        tr.lpstrText = s;
        rCtrl.GetTextRange(&tr);
    }
    
    if (strlen(s) >= CLuaEditorParamMgr::ReadSingleInstance()->GetNbCharsToTriggerAutoCompletion())
    {
        rCtrl.AutoCShow(strlen(s), CLuaEditorParamMgr::ReadSingleInstance()->GetLuaAutoCList());
    }
    
}

// gestion de l'affichage d'un calltip presentant la fonction et ses parametres
// On ne gere pas les appels de fonctions sur plusieurs lignes
void CLuaView::ProcessFunctionCallTip(void) 
{
    CScintillaCtrl& rCtrl = GetCtrl();
    if (rCtrl.CallTipActive())
    {
        // on l'enleve s'il est affiche (on le reaffichera peut-etre)
	    rCtrl.CallTipCancel();
	}

    // construction du tableau des tokens contenus dans 
    // le texte allant du debut de la ligne courante jusqu'au curseur
    std::vector<Token> arrTokens;
    BuildTokensArray(&arrTokens);
    
    // on cherche maintenant a trouver la fonction et le n° du parametre courant
    int paramNo;
    CFunctionDesc* pFunctionDesc = ExtractFunction(&arrTokens, &paramNo);
    if (NULL == pFunctionDesc)
    {
        // pas de fonction trouvee
    	return;
    }
    ShowFunctionCalltip(rCtrl.GetCurrentPos(), pFunctionDesc, paramNo);
}
    
void CLuaView::ShowFunctionCalltip(long posCalltip, CFunctionDesc* pFunctionDesc, int paramNo /*= -1*/)
{
    CScintillaCtrl& rCtrl = GetCtrl();
    // on va maintenant afficher le calltip
    // on construit la calltip a afficher
    std::string calltip = pFunctionDesc->GetRetVal();
    if (calltip != "void")
    {
        calltip += " =";
    }
    calltip += " ";
    calltip += pFunctionDesc->GetFunctionName();
    calltip += '(';
    int highlightstart = 0;
    int highlightend = 0;
    const std::vector<std::string>& arrParams(pFunctionDesc->GetArrParams());
    std::string param;
    for (unsigned i = 0; i<arrParams.size(); ++i)
    {
        param = arrParams[i];
        if (i > 0)
        {
            // pas le premier parametre
            if (param[0] == '[')
            {
                // debut de parametres optionnels (par convention dans le fichier XML)
                // on va deplacer le [ avant la virgule
                calltip += " [";
                param.erase(0, 1);  // on efface ce [
            }
            // dans tous les cas, on insere la virgule et un blanc
            calltip += ", ";
        }
        if (i == paramNo)
        {
            highlightstart = calltip.size();
        }
        calltip += param;
        if (i == paramNo)
        {
            highlightend = calltip.size();
        }
    }
    calltip += ')';
    // on va maintenant continuer avec la description de la fonction
    if (!pFunctionDesc->GetDesc().empty())
    {
        calltip += '\n';
        calltip += pFunctionDesc->GetDesc();
    }
    rCtrl.CallTipShow(posCalltip, calltip.c_str());
    if (highlightend != highlightstart)
    {
        rCtrl.CallTipSetHlt(highlightstart, highlightend);
    }
}
    
CFunctionDesc* CLuaView::ExtractFunction(std::vector<Token>* pArrTokens, int* pParamNo)
{
    // on va maintenant le parcourir pour en tirer la fonction dont il faut afficher la description
    // et le n° du parametre courant
    // On commence par la fin du tableau, on ne verifie pas que la syntaxe et la semantique sont bonnes
    *pParamNo = 0;              // n° du parametre
    int nbClosingBraces = 0;    // nb de parentheses fermantes rencontrees sans que l'on ait trouve de parentheses ouvrantes
    while (pArrTokens->size() > 0)
    {
        Token tok = pArrTokens->back();
        pArrTokens->pop_back();
        if (tok.token == ")")
        {
            ++nbClosingBraces;
        }
        else
        {
            if (nbClosingBraces > 0)
            {
                // on ignore tout les tokens qui ne sont pas des parentheses ouvrantes
                if (tok.token == "(")
                {
                    --nbClosingBraces;
                }
            }
            else
            {
                if (tok.token == "(")
                {
                    // on s'attend donc a trouver a gauche de cette parenthese :
                    // 1. d'autre parenthese ouvrante
                    // 2. ou un id avec le nom de la fonction
                    while ((tok.token == "(") && (pArrTokens->size() > 0))
                    {
                        tok = pArrTokens->back();
                        pArrTokens->pop_back();
                    }
                    if (tok.token == "(")
                    {
                        // on n'a pas trouve d'autre token differente d'une parenthese ouvrante
                        return NULL;
                    }
                    if (!tok.isIdentifier)
                    {
                        // le token trouve n'est pas un identifieur
                        return NULL;
                    }
                    // on retourne le pointeur sur le descripteur de fonction correspondant a cet
                    // identifieur (null s'il n'existe pas)
                    return CLuaEditorParamMgr::ReadSingleInstance()->GetFunctionDesc(tok.token);
                }
                else if (tok.token == ",")
                {
                    ++(*pParamNo);
                }
            }
        }
    }
    return NULL; 
}

// construction du tableau des tokens contenus dans 
// le texte allant du debut de la ligne courante jusqu'au curseur
void CLuaView::BuildTokensArray(std::vector<Token>* pArrTokens)
{
    CScintillaCtrl& rCtrl = GetCtrl();

    // On commence par recuperer le texte qui va du debut de la ligne courante
    // a la position courante
    int caretPos = rCtrl.GetCurrentPos();
	int currLine = rCtrl.LineFromPosition(caretPos);
	int startCurrLinePos = rCtrl.PositionFromLine(currLine);
	int len = (caretPos-startCurrLinePos);
    boost::unique_ptr<char[]> buffer;
    buffer.reset(new char[len+1]);    // +1 pour le zero terminal
    TextRange tr;
    tr.chrg.cpMin = startCurrLinePos;
    tr.chrg.cpMax = caretPos;
    tr.lpstrText = buffer.get();
    rCtrl.GetTextRange(&tr);

    // puis on le parse pour en extraire les tokens
    int beginId = -1;
    int i;
    for (i = 0; i < len; ++i)
    {
        char ch = buffer[i];
        if (isWordChar(ch))
        {
            // on ajoute le caractere au token courant
            if (-1 == beginId)
            {
                // on stocke la position du debut de l'ID
                beginId = i;
            }
        }
        else if ((ch == ' ') || (ch == '\t'))
        {
            // si un token en cours => on l'ajoute
            if (-1 != beginId)
            {
                pArrTokens->push_back(Token(&buffer[beginId], i-beginId, true));
                beginId = -1;
            }
        }
        else
        {
            // autre car => on considere comme un token
            // si un token en cours => on l'ajoute
            if (-1 != beginId)
            {
                pArrTokens->push_back(Token(&buffer[beginId], i-beginId, true));
                beginId = -1;
            }
            pArrTokens->push_back(Token(&buffer[i], 1, false));
        }
    }
    if (-1 != beginId)
    {
        // si un token etait en cours => on l'ajoute
        pArrTokens->push_back(Token(&buffer[beginId], i-beginId, true));
    }
}

void CLuaView::OnUpdateUI(SCNotification* pSCNotification)
{
    CScintillaCtrl& rCtrl = GetCtrl();
    // changement dans le contenu, les markers ou le style
    if (CLuaEditorParamMgr::ReadSingleInstance()->IsBracesHighlightingEnabled())
    {
        // => on va traiter la mise en evidence des parenthese
        CheckBracesMatch();
    }
    
    // On commence par effacer les marques existantes
    rCtrl.SetIndicatorCurrent(SCE_UNIVERSAL_FOUND_STYLE_SMART);
    rCtrl.IndicatorClearRange(0, rCtrl.GetLength());

    bool isSelectedWordHighLight = false;
    if (CLuaEditorParamMgr::ReadSingleInstance()->IsSelectedWordHighlightingEnabled())
    {
        // => on va traiter la mise en evidence des occurences d'un mot selectionne
        isSelectedWordHighLight = HighLightSelectedWord();
    }
    
    if (false == isSelectedWordHighLight)
    {
        // pas de mot mis en evidence
        if (CLuaEditorParamMgr::ReadSingleInstance()->IsStatementHighlightingEnabled())
        {
            // on traite la mise en evidence des statements
            HighLightStatement();
        }
    }
}
// Mise en evidence des parentheses (code repris de notepad ++)
void CLuaView::CheckBracesMatch(void)
{
    CScintillaCtrl& rCtrl = GetCtrl();
    int caretPos = int(rCtrl.GetCurrentPos());
    int braceAtCaret = -1;
    int braceOpposite = -1;
    char charBefore = '\0';

    int lengthDoc = int(rCtrl.GetLength());

    if ((lengthDoc > 0) && (caretPos > 0)) 
    {
        charBefore = char(rCtrl.GetCharAt(caretPos - 1, 0));
    }
    // Priority goes to character before caret
    if (charBefore && strchr(TEXT("[](){}"), charBefore))
    {
        braceAtCaret = caretPos - 1;
    }

    if (lengthDoc > 0  && (braceAtCaret < 0)) 
    {
        // No brace found so check other side
        char charAfter = char(rCtrl.GetCharAt(caretPos, 0));
        if (charAfter && strchr(TEXT("[](){}"), charAfter))
        {
            braceAtCaret = caretPos;
        }
    }
    if (braceAtCaret >= 0)
    {
        braceOpposite = int(rCtrl.BraceMatch(braceAtCaret, 0));
    }
    if ((braceAtCaret != -1) && (braceOpposite == -1))
    {
        rCtrl.BraceBadLight(braceAtCaret);
        rCtrl.SetHighlightGuide(0);
    } 
    else 
    {
        rCtrl.BraceHighlight(braceAtCaret, braceOpposite);
        if (rCtrl.GetIndentationGuides())
        {
            int columnAtCaret = int(rCtrl.GetColumn(braceAtCaret));
            int columnOpposite = int(rCtrl.GetColumn(braceOpposite));
            rCtrl.SetHighlightGuide((columnAtCaret < columnOpposite)?columnAtCaret:columnOpposite);
        }
    }
}
// Mise en evidence d'un mot selectionne => toutes les occurences de ce mot dans la partie du texte visible
// sont mises en evidence
bool CLuaView::HighLightSelectedWord(void)
{
    CScintillaCtrl& rCtrl = GetCtrl();

    // On qu'un mot est selectionne
	long cpMin = rCtrl.GetSelectionStart();
	long cpMax = rCtrl.GetSelectionEnd();
    if (cpMin == cpMax)
    {
        // rien n'est selectionne
        return false;
    }
    // quelque chose est selectionne : est-ce un mot ?
    int selectedWordLen = cpMax - cpMin;
    boost::unique_ptr<char[]> selectedWord;
    selectedWord.reset(new char[selectedWordLen + 1]);    // +1 pour le zero terminal
    TextRange tr;
    tr.chrg.cpMin = cpMin;
    tr.chrg.cpMax = cpMax;
    tr.lpstrText = selectedWord.get();
    rCtrl.GetTextRange(&tr);
    if (!isQualifiedWord(selectedWord.get()))
    {
        // la selection n'est pas composee que de caractere appartenant a un identifieur
        return false;
    }
    int c = rCtrl.GetCharAt(cpMax);
    if ((c) && (isWordChar(char(c))))
    {
        // Le caractere apres la selection appartient au jeu de caractere d'un identifieur
        // => la selection n'est pas un mot entier
        return false;
    }
    c = rCtrl.GetCharAt(cpMin-1);
    if ((c) && (isWordChar(char(c))))
    {
        // Le caractere apres la selection appartient au jeu de caractere d'un identifieur
        // => la selection n'est pas un mot entier
        return false;
    }

    // On cherche toutes les occurences du mots dans le texte visible
    int firstLine =		(int)rCtrl.GetFirstVisibleLine();
    int nrLines =	    (int)rCtrl.LinesOnScreen() + 1;
	int lastLine =		firstLine+nrLines;
    int startPos =		(int)rCtrl.PositionFromLine(firstLine);
    int endPos =		(int)rCtrl.PositionFromLine(lastLine);
    if (-1 == endPos)
    {
        endPos = rCtrl.GetLength();
    }
    int visibleTextLen = endPos - startPos;
    boost::unique_ptr<char[]> visibleText;
    visibleText.reset(new char[visibleTextLen+1]);    // +1 pour le zero terminal
    tr.chrg.cpMin = startPos;
    tr.chrg.cpMax = endPos;
    tr.lpstrText = visibleText.get();
    rCtrl.GetTextRange(&tr);
    // on passe par une string pour utiliser find
    std::string strVisibleText(visibleText.get());
    
    // on selectionne l'indicateur courant
    rCtrl.SetIndicatorCurrent(SCE_UNIVERSAL_FOUND_STYLE_SMART);
    // on va rechercher toutes les occurences du mot et les mettre en evidence
    size_t posSearch = strVisibleText.find(selectedWord.get(), 0, selectedWordLen);
    while (std::string::npos != posSearch)
    {
        // on l'a trouve, on verifie que le caractere precedent et le caractere suivant n'appartiennent
        // pas au jeu de caracteres d'un identifieur
        bool isWholeWord = true;
        if (posSearch > 0)
        {
            if (isWordChar(strVisibleText[posSearch-1]))
            {
                isWholeWord = false;
            }
        }
        if (isWholeWord && ((posSearch + selectedWordLen) < visibleTextLen))
        {
            if (isWordChar(strVisibleText[posSearch + selectedWordLen]))
            {
                isWholeWord = false;
            }
        }
        if (isWholeWord)
        {
            // on a trouve le mot, on marque l'occurence courante
		    rCtrl.IndicatorFillRange(posSearch+startPos, selectedWordLen);
        }
        // on cherche l'occurence suivant apres le mot en cours
        posSearch = strVisibleText.find(selectedWord.get(), posSearch + selectedWordLen, selectedWordLen);
    }
    return true;
}
// Mise en evidence des statements
// si on n'est pres d'un mor clef d'un statement, on va mettre en evidence tous les mots clefs de ce statement
// Ex : si le curseur est a cote ou dans un "then", on va mettre en evidence ce "then" ansi que le "if", le "end"
// (et peut-etre les "else" et autres "elseif / then" de ce statement
// on traite (en essayant de gerer les imbrication) :
// if then elseif then else end
// function end
// while do end
// for do end
// repeat until
void CLuaView::HighLightStatement(void)
{
    CScintillaCtrl& rCtrl = GetCtrl();

    // On determine maintenance si quelque chose est selectionne et si la selection est un mot
	long cpMin = rCtrl.GetSelectionStart();
	long cpMax = rCtrl.GetSelectionEnd();
	
    if (cpMin != cpMax)
    {
        // quelque chose est selectionnne, on ne fait rien
        return;
    }

    // On recupere le mot proche et on regarde si c'est un statement
    int caretPos = rCtrl.GetCurrentPos();
    int wordStartPos = static_cast<int>(rCtrl.WordStartPosition(caretPos, true));
    
    // on verifie qu'on n'est pas dans un commentaire ou une string
    if (isCommentOrStringStyle(rCtrl.GetStyleAt(wordStartPos)))
    {
        // on est dans un commentaire ou une string, on ne fait rien
        return;
    }
    
    int wordEndPos = static_cast<int>(rCtrl.WordEndPosition(caretPos, true));
    int wordLen = wordEndPos - wordStartPos;
    char s[13+1];    // 13 = strlen("TestCaseBegin") + 1 pour le 0 terminal, "TestCaseBegin"
                    // etant le statement le + long qu'on traite
    s[0] = '\0';
    if ((wordLen) >= sizeof(s))   // on fait un >= car il faut prevoir +1 pour le 0 terminal
    {
        // identifier trop long, on ne fait rien 
        return;
    }
    TextRange tr;
    tr.chrg.cpMin = wordStartPos;
    tr.chrg.cpMax = wordEndPos;
    tr.lpstrText = s;
    rCtrl.GetTextRange(&tr);
    
    
    T_STATEMENT_TYPE type = GetStatementType(s);
    if (NONE == type)
    {
        // pas un statement, on ne fait rien
        return;
    }
    
    // On va maintenant construire le tableau des statements sur la portion de texte visible
    std::vector<Statement> arrStatements;
    BuildStatementsArray(&arrStatements, wordStartPos, wordEndPos);
    
    // puis on recherche la position du statement courant dans ce tableau
    int iCurrStatement = -1;
    for (int i = 0; (-1 == iCurrStatement) && (i < arrStatements.size())  ; ++i)
    {
        if (arrStatements[i].position == wordStartPos)
        {
            // c'est lui
            iCurrStatement = i;
        }
    }
    if (-1 == iCurrStatement)
    {
        // ne devrait normalement pas arriver ...
        return;
    }
    
    // on selectionne l'indicateur courant
    rCtrl.SetIndicatorCurrent(SCE_UNIVERSAL_FOUND_STYLE_SMART);
    // et on marque le statement courant
	rCtrl.IndicatorFillRange(wordStartPos, wordLen);
	
	// en fonction du statement courant, on va lancer la recherche des statements correspondants
	unsigned long mskSearchForward = 0;
	unsigned long mskSearchBackward = 0;
	switch (type)
	{
	    case IF:
	        mskSearchForward = THEN;
	        break;
	    case THEN:
	        mskSearchForward = ELSEIF | ELSE | END;
	        mskSearchBackward = IF | ELSEIF;
	        break;
	    case ELSEIF:
	        mskSearchForward = THEN;
	        mskSearchBackward = THEN;
	        break;
	    case ELSE:
	        mskSearchForward = END;
	        mskSearchBackward = THEN;
	        break;
	    case END:
	        mskSearchBackward = THEN|ELSE|FUNCTION|DO;
	        break;
	    case FUNCTION:
	        mskSearchForward = END;
	        break;
	    case WHILE:
	        mskSearchForward = DO;
	        break;
	    case FOR:
	        mskSearchForward = DO;
	        break;
	    case DO:
	        mskSearchForward = END;
	        mskSearchBackward = WHILE|FOR;
	        break;
	    case REPEAT:
	        mskSearchForward = UNTIL;
	        break;
	    case UNTIL:
	        mskSearchBackward = REPEAT;
	        break;
        case TC_BEGIN:
            mskSearchForward = TC_END;
            break;
        case TC_END:
            mskSearchBackward = TC_BEGIN;
            break;
	}
	
	if (mskSearchBackward != 0)
	{
	    SearchStatementBackward(arrStatements, iCurrStatement, mskSearchBackward, 0);
	}
	if (mskSearchForward != 0)
	{
	    SearchStatementForward(arrStatements, iCurrStatement, mskSearchForward, 0);
	}
}
int CLuaView::SearchStatementBackward(std::vector<Statement>& arrStatements, int iCurrStatement, int mskSearchBackward, int level)
{
    --iCurrStatement;
    if (iCurrStatement < 0)
    {
        // fini
        return iCurrStatement;
    }
    if (arrStatements[iCurrStatement].type & mskSearchBackward)
    {
        // le statement fait partie des statements recherches
        if (0 == level)
        {
            // on est au niveau 0 de l'imbrication => on le marque
            CScintillaCtrl& rCtrl = GetCtrl();
    	    rCtrl.IndicatorFillRange(arrStatements[iCurrStatement].position, arrStatements[iCurrStatement].len);
    	}
        // on determine le nouveau masque de recherche
        mskSearchBackward = 0;
        switch (arrStatements[iCurrStatement].type)
        {
            case IF:
            case FUNCTION:
            case WHILE:
            case FOR:
            case REPEAT:
            case TC_BEGIN:
                // debut du statement atteint
                break;
            case THEN:
                mskSearchBackward = IF | ELSEIF;
                break;
            case ELSEIF:
                mskSearchBackward = THEN;
                break;
            case ELSE:
                mskSearchBackward = THEN;
                break;
            case DO:
                mskSearchBackward = WHILE|FOR;
                break;
        }
    }
    else
    {
        switch (arrStatements[iCurrStatement].type)
        {
            case END:
                // on refait une recherche pour ce nouveau niveau d'imbrication
                iCurrStatement = SearchStatementBackward(arrStatements, iCurrStatement, THEN|ELSE|FUNCTION|DO, level+1);
                break;
            case UNTIL:
                // on refait une recherche pour ce nouveau niveau d'imbrication
                iCurrStatement = SearchStatementBackward(arrStatements, iCurrStatement, REPEAT, level+1);
                break;
            default:
                // on ignore le statement
                break;
        }
    }
    if (mskSearchBackward)
    {
        // on continue la recherche arriere s'il y a quelque chose a chercher
        iCurrStatement = SearchStatementBackward(arrStatements, iCurrStatement, mskSearchBackward, level);
    }
    return iCurrStatement;
}
int CLuaView::SearchStatementForward(std::vector<Statement>& arrStatements, int iCurrStatement, int mskSearchForward, int level)
{
    ++iCurrStatement;
    if (iCurrStatement >=  arrStatements.size())
    {
        // fini
        return iCurrStatement;
    }
    if (arrStatements[iCurrStatement].type & mskSearchForward)
    {
        // le statement fait partie des statements recherches
        if (0 == level)
        {
            // on est au niveau 0 de l'imbrication => on le marque
            CScintillaCtrl& rCtrl = GetCtrl();
    	    rCtrl.IndicatorFillRange(arrStatements[iCurrStatement].position, arrStatements[iCurrStatement].len);
    	}
        // on determine le nouveau masque de recherche
        mskSearchForward = 0;
        switch (arrStatements[iCurrStatement].type)
        {
	        case THEN:
	            mskSearchForward = ELSEIF | ELSE | END;
	            break;
	        case ELSEIF:
	            mskSearchForward = THEN;
	            break;
	        case ELSE:
	            mskSearchForward = END;
	            break;
	        case DO:
	            mskSearchForward = END;
	            break;
	        case END:
	        case UNTIL:
            case TC_END:
	            // fin du statement atteinte
	            break;
        }
    }
    else
    {
        switch (arrStatements[iCurrStatement].type)
        {
	        case IF:
                // on refait une recherche pour ce nouveau niveau d'imbrication
                iCurrStatement = SearchStatementForward(arrStatements, iCurrStatement, THEN, level+1);
	            break;
	        case FUNCTION:
                // on refait une recherche pour ce nouveau niveau d'imbrication
                iCurrStatement = SearchStatementForward(arrStatements, iCurrStatement, END, level+1);
	            break;
	        case WHILE:
	        case FOR:
                // on refait une recherche pour ce nouveau niveau d'imbrication
                iCurrStatement = SearchStatementForward(arrStatements, iCurrStatement, DO, level+1);
	            break;
	        case REPEAT:
                // on refait une recherche pour ce nouveau niveau d'imbrication
                iCurrStatement = SearchStatementForward(arrStatements, iCurrStatement, UNTIL, level+1);
                break;
            default:
                // on ignore le statement
                break;
        }
    }
    if (mskSearchForward)
    {
        // on continue la recherche avant s'il y a quelque chose a chercher
        iCurrStatement = SearchStatementForward(arrStatements, iCurrStatement, mskSearchForward, level);
    }
    return iCurrStatement;
}

// construction du tableau des statement contenus dans 
// le texte visible
void CLuaView::BuildStatementsArray(std::vector<Statement>* pArrStatements, int statementStartPos, int statementEndPos)
{
    CScintillaCtrl& rCtrl = GetCtrl();

    // On commence par recuperer le texte visible
    int firstLine =		(int)rCtrl.GetFirstVisibleLine();
    int nrLines =	    (int)rCtrl.LinesOnScreen() + 1;
	int lastLine =		firstLine+nrLines;
    int startPos =		(int)rCtrl.PositionFromLine(firstLine);
    int endPos =		(int)rCtrl.PositionFromLine(lastLine);
    if (-1 == endPos)
    {
        endPos = rCtrl.GetLength();
    }
    if (statementStartPos < startPos)
    {
        // le statement est au dessus du texte visible => on etend la selection dans sa direction
        startPos = statementStartPos;
    }
    if (statementEndPos > endPos)
    {
        // le statement est en dessous du texte visible => on etend la selection dans sa direction
        endPos = statementEndPos;
    }
    int visibleTextLen = endPos - startPos;
    boost::unique_ptr<unsigned char[]> visibleText;
    visibleText.reset(new unsigned char[2*(visibleTextLen+1)]);    // +1 pour le zero terminal
                                                          // *2 car on recupere les bits de style
    TextRange tr;
    tr.chrg.cpMin = startPos;
    tr.chrg.cpMax = endPos;
    tr.lpstrText = (char*)visibleText.get();
    // on recupere aussi les bits de style
    rCtrl.GetStyledText(&tr);
    std::string id;
    
    // puis on le parse pour en extraire les statements
    int beginId = -1;
    int i;
    for (i = 0; i < visibleTextLen; ++i)
    {
        unsigned char style = visibleText[2*i+1];
        if (isCommentOrStringStyle(style))
        {
            // si un token en cours => on l'ajoute
            if (-1 != beginId)
            {
                T_STATEMENT_TYPE type = GetStatementType(id.c_str());
                if (NONE != type)
                {
                    pArrStatements->push_back(Statement(type, startPos+beginId, i-beginId));
                }
                beginId = -1;
                id.clear();
            }
            continue;
        }
        char ch = visibleText[2*i];
        if (isWordChar(ch))
        {
            id += ch;
            // on ajoute le caractere au token courant
            if (-1 == beginId)
            {
                // on stocke la position du debut de l'ID
                beginId = i;
            }
        }
        else
        {
            // si un token en cours => on l'ajoute
            if (-1 != beginId)
            {
                T_STATEMENT_TYPE type = GetStatementType(id.c_str());
                if (NONE != type)
                {
                    pArrStatements->push_back(Statement(type, startPos+beginId, i-beginId));
                }
                beginId = -1;
                id.clear();
            }
        }
    }
    if (-1 != beginId)
    {
        // si un token etait en cours => on l'ajoute
        T_STATEMENT_TYPE type = GetStatementType(id.c_str());
        if (NONE != type)
        {
            pArrStatements->push_back(Statement(type, startPos+beginId, i-beginId));
        }
    }
}
CLuaView::T_STATEMENT_TYPE CLuaView::GetStatementType(const char* pStatement)
{
    if (0 == strcmp(pStatement, "if"))
    {
        return IF;
    } 
    else if (0 == strcmp(pStatement, "then"))
    {
        return THEN;
    } 
    else if (0 == strcmp(pStatement, "else"))
    {
        return ELSE;
    } 
    else if (0 == strcmp(pStatement, "elseif"))
    {
        return ELSEIF;
    } 
    else if (0 == strcmp(pStatement, "end"))
    {
        return END;
    } 
    else if (0 == strcmp(pStatement, "function"))
    {
        return FUNCTION;
    } 
    else if (0 == strcmp(pStatement, "while"))
    {
        return WHILE;
    } 
    else if (0 == strcmp(pStatement, "for"))
    {
        return FOR;
    } 
    else if (0 == strcmp(pStatement, "do"))
    {
        return DO;
    } 
    else if (0 == strcmp(pStatement, "repeat"))
    {
        return REPEAT;
    } 
    else if (0 == strcmp(pStatement, "until"))
    {
        return UNTIL;
    } 
    else if (0 == strcmp(pStatement, "TestCaseBegin"))
    {
        return TC_BEGIN;
    } 
    else if (0 == strcmp(pStatement, "TestCaseEnd"))
    {
        return TC_END;
    } 
    return NONE;
}

void CLuaView::OnDwellStart(SCNotification* pSCNotification)
{
    CScintillaCtrl& rCtrl = GetCtrl();

    //Only display the call tip if the scintilla window has focus
    CWnd* pFocusWnd = GetFocus();
    if (pFocusWnd && (pFocusWnd->GetSafeHwnd() == rCtrl.GetSafeHwnd()))
    {
        // si on pointe sur une fonction dont on a la description, on l'affiche
        TextRange tr;
        tr.chrg.cpMin = rCtrl.WordStartPosition(pSCNotification->position, TRUE);
        tr.chrg.cpMax = rCtrl.WordEndPosition(pSCNotification->position, TRUE);
        boost::unique_ptr<char[]> buffer;
        buffer.reset(new char[tr.chrg.cpMax-tr.chrg.cpMin+1]);    // +1 pour le zero terminal
        tr.lpstrText = buffer.get();
        rCtrl.GetTextRange(&tr);
        
        CFunctionDesc* pFunctionDesc = CLuaEditorParamMgr::ReadSingleInstance()->GetFunctionDesc(std::string(buffer.get()));
        if (pFunctionDesc)
        {
            ShowFunctionCalltip(pSCNotification->position, pFunctionDesc);
        }
    }
}

void CLuaView::OnDwellEnd(SCNotification* /*pSCNotification*/)
{
    CScintillaCtrl& rCtrl = GetCtrl();

    //Cancel any outstanding call tip
    if (rCtrl.CallTipActive())
        rCtrl.CallTipCancel();
}

void CLuaView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
  //Let the base class do its thing
    CScintillaView::OnActivate(nState, pWndOther, bMinimized);

  CScintillaCtrl& rCtrl = GetCtrl();

  //Cancel any outstanding call tip
    if (nState == WA_INACTIVE && rCtrl.CallTipActive())
    rCtrl.CallTipCancel();
}

void CLuaView::OnModifyAttemptRO(SCNotification* /*pSCNotification*/)
{
    // uniquement si on n'est pas running
    if (m_LuaInterpreter.IsRunning() == false)
    {
        if (AfxMessageBox(IDP_ALLOW_MODIFY_READONLY_FILE, MB_YESNO) == IDYES)
        GetCtrl().SetReadOnly(FALSE);
    }
}

void CLuaView::Expand(int &line, bool doExpand, bool force, int visLevels, int level)
{
    // reprise/adpatation d'un code de SCITE
    CScintillaCtrl& rCtrl = GetCtrl();
    int lineMaxSubord = rCtrl.GetLastChild(line, level & SC_FOLDLEVELNUMBERMASK);
    line++;
    while (line <= lineMaxSubord)
    {
        if (force)
        {
            if (visLevels > 0)
                rCtrl.ShowLines(line, line);
            else
                rCtrl.HideLines(line, line);
        }
        else
        {
            if (doExpand)
                rCtrl.ShowLines(line, line);
        }
        int levelLine = level;
        if (levelLine == -1)
            levelLine = rCtrl.GetFoldLevel(line);
        if (levelLine & SC_FOLDLEVELHEADERFLAG)
        {
            if (force)
            {
                if (visLevels > 1)
                    rCtrl.SetFoldExpanded(line, 1);
                else
                    rCtrl.SetFoldExpanded(line, 0);
                Expand(line, doExpand, force, visLevels - 1);
            }
            else
            {
                if (doExpand)
                {
                    if (!rCtrl.GetFoldExpanded(line))
                        rCtrl.SetFoldExpanded(line, 1);
                    Expand(line, true, force, visLevels - 1);
                }
                else
                {
                    Expand(line, false, force, visLevels - 1);
                }
            }
        }
        else
        {
            line++;
        }
    }
}

void CLuaView::FoldAll()
{
    // reprise/adpatation d'un code de SCITE
    CScintillaCtrl& rCtrl = GetCtrl();
    rCtrl.Colourise(0, -1);
    int maxLine = rCtrl.GetLineCount();
    bool expanding = true;
    for (int lineSeek = 0; lineSeek < maxLine; lineSeek++)
    {
        if (rCtrl.GetFoldLevel(lineSeek) & SC_FOLDLEVELHEADERFLAG)
        {
            expanding = !rCtrl.GetFoldExpanded(lineSeek);
            break;
        }
    }
    for (int line = 0; line < maxLine; line++)
    {
        int level = rCtrl.GetFoldLevel(line);
        if ((level & SC_FOLDLEVELHEADERFLAG) &&
                (SC_FOLDLEVELBASE == (level & SC_FOLDLEVELNUMBERMASK)))
        {
            if (expanding)
            {
                rCtrl.SetFoldExpanded(line, 1);
                Expand(line, true, false, 0, level);
                line--;
            }
            else
            {
                int lineMaxSubord = rCtrl.GetLastChild(line, -1);
                rCtrl.SetFoldExpanded(line, 0);
                if (lineMaxSubord > line)
                    rCtrl.HideLines(line + 1, lineMaxSubord);
            }
        }
    }
}
void CLuaView::OnMarginClick(SCNotification* pSCNotification)
{
    //By default get the line where the click occured and toggle its fold state
    CScintillaCtrl& rCtrl = GetCtrl();
    if ((pSCNotification->modifiers & (SCMOD_SHIFT|SCMOD_CTRL)) == (SCMOD_SHIFT|SCMOD_CTRL))
    {
        FoldAll();
    }
    else
    {
        int nLine = rCtrl.LineFromPosition(pSCNotification->position);
        rCtrl.ToggleFold(nLine);
    }
}

void CLuaView::OnUpdateBtSave(CCmdUI* pCmdUI)
{
    if (GetDocument()->IsModified())
    {
        pCmdUI->Enable(1);
    }
    else
    {
        pCmdUI->Enable(0);
    }
}
void CLuaView::OnUpdateBtSaveAs(CCmdUI* pCmdUI)
{
    CScintillaCtrl& rCtrl = GetCtrl();
    if (rCtrl.GetLength())
    {
        pCmdUI->Enable(1);
    }
    else
    {
        pCmdUI->Enable(0);
    }
}
void CLuaView::OnUpdateBtRunScript(CCmdUI* pCmdUI)
{
    // On autorise le bouton de run que si l'interpreteur n'est pas deja en fonctionnement
    if (false == m_LuaInterpreter.IsRunning())
    {
        pCmdUI->Enable(1);
    }
    else
    {
        pCmdUI->Enable(0);
    }
}
void CLuaView::OnUpdateBtStopScript(CCmdUI* pCmdUI)
{
    // On autorise le bouton de stop que si l'interpreteur est en fonctionnement
    if (m_LuaInterpreter.IsRunning())
    {
        pCmdUI->Enable(1);
    }
    else
    {
        pCmdUI->Enable(0);
    }
}
bool CLuaView::isQualifiedWord(const char *str) const
{
    size_t len = strlen(str);
    for (size_t i = 0 ; i < len ; ++i)
    {
        if (!isWordChar(str[i]))
            return false;
    }
    return true;
}

bool CLuaView::isWordChar(char ch) const
{
    if (   (ch >= 'A' && ch <= 'Z')
        || (ch >= 'a' && ch <= 'z')
        || (ch >= '0' && ch <= '9')
        || (ch == '_')
        || (ch == '.')
       )
    {
        return true;
    }
    return false;
}
bool CLuaView::isCommentOrStringStyle(unsigned char style)
{
    // on ne garde que les 5 bits de style
    style &= 0x1F;
    if (   (style == SCE_C_COMMENT)
        || (style == SCE_C_COMMENTLINE)
        || (style == SCE_C_COMMENTDOC)
        || (style == SCE_C_STRING)
       )
    {
        return true;
    }
    return false;
}
bool CLuaView::isCommentStyle(unsigned char style)
{
    // on ne garde que les 5 bits de style
    style &= 0x1F;
    if (   (style == SCE_C_COMMENT)
        || (style == SCE_C_COMMENTLINE)
        || (style == SCE_C_COMMENTDOC)
       )
    {
        return true;
    }
    return false;
}
bool CLuaView::isStringStyle(unsigned char style)
{
    // on ne garde que les 5 bits de style
    style &= 0x1F;
    if (style == SCE_C_STRING)
    {
        return true;
    }
    return false;
}
