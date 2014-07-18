// tracebar.cpp : implementation file

// note : l'implementation est faite avec un mecanisme de thread car sinon, il y a
//        des problemes de synchro entre les threads qui postents les messages et
//        l'affichage des traces
//


#include "stdafx.h"
#include <process.h>
#include "OngTV.h"
#include "TraceBar.h"
#include "UTBStrOp.h"
#include "AdeThread.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define CMD_ACTIVATE_LOG_TO_FILE    5000
#define CMD_DEACTIVATE_LOG_TO_FILE  5001
#define CMD_CLEAR_HISTORY           5002

/////////////////////////////////////////////////////////////////////////////
// CAdeOXHistoryCtrl
//

IMPLEMENT_DYNAMIC(CAdeOXHistoryCtrl, COXHistoryCtrl);

BEGIN_MESSAGE_MAP(CAdeOXHistoryCtrl, COXHistoryCtrl)
    //{{AFX_MSG_MAP(CAdeOXHistoryCtrl)
    ON_COMMAND(CMD_ACTIVATE_LOG_TO_FILE, OnActivateLog)
    ON_COMMAND(CMD_DEACTIVATE_LOG_TO_FILE, OnDeactivateLog)
    ON_COMMAND(CMD_CLEAR_HISTORY, OnClearHistory)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAdeOXHistoryCtrl::OnPopulateContextMenu(CMenu* pMenu, CPoint& point)    
{
    UNREFERENCED_PARAMETER(point);

    // On commence par vider le menu construit dans COXHistoryCtrl 
    int itemCount = pMenu->GetMenuItemCount();
    while (itemCount != 0)
    {
        pMenu->RemoveMenu(itemCount-1, MF_BYPOSITION);
        --itemCount;
    }
    // Puis on affiche le menu correspondant a l'action
    pMenu->AppendMenu(MF_STRING, CMD_CLEAR_HISTORY, theApp.LoadString(STR_TRC_CMD_CLEAR_HISTORY));
    if (m_bLogToFileAllowed == true)
    {
        if (m_bLogInProgress)
        {
            pMenu->AppendMenu(MF_STRING, CMD_DEACTIVATE_LOG_TO_FILE, theApp.LoadString(STR_TRC_CMD_DEACTIVATE_LOG_TO_FILE));
        }
        else
        {
            pMenu->AppendMenu(MF_STRING, CMD_ACTIVATE_LOG_TO_FILE, theApp.LoadString(STR_TRC_CMD_ACTIVATE_LOG_TO_FILE));
        }
    }
    return TRUE;
}
void CAdeOXHistoryCtrl::OnActivateLog()
{
    // boite de dialogue pour la selection de l'emplacement de sauvegarde
    std::string SaveFileName = theApp.SelectFile(FALSE, "log", theApp.LoadString(STR_LOG_FILE_FILTER));
    if(0 == SaveFileName.size())
    {
        return;
    }
    // Les fichiers logs existants sont tronques
    SetTruncateLogFile(TRUE);
    SetLogFileName(CString(SaveFileName.c_str()));
    EnableLog(TRUE);
    m_bLogInProgress = true;
}

void CAdeOXHistoryCtrl::OnDeactivateLog()
{
    EnableLog(FALSE);
    m_bLogInProgress = false;
}

void CAdeOXHistoryCtrl::OnClearHistory()
{
    ClearHistory();
}

/////////////////////////////////////////////////////////////////////////////
// CTraceBar
//


CTraceBar::CTraceBar()
{
    m_hMutexDico = CreateMutex(NULL, false, NULL);
    m_bLuaWindowTraceToCreate = false;
}

CTraceBar::~CTraceBar()
{
    CloseHandle(m_hMutexDico);
}



BEGIN_MESSAGE_MAP(CTraceBar, CDockablePane)
    //{{AFX_MSG_MAP(CTraceBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTraceBar message handlers


void CTraceBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void CTraceBar::OnDestroy() 
{
    // arret du thread et vidage de la FIFO de traces
    if (m_pThreadManagement.get())
    {
        m_pThreadManagement->StopThread(1000);   // On attend la fin du Thread avec un timeout d'une seconde (sinon, dans certain cas, on ne sort pas)
    }
    ClearFifoTraces();
    CDockablePane::OnDestroy();
}

int CTraceBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    char szFunc[] = "CTraceBar::OnCreate";
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

    // swapping des ongles interdits
    m_wndTabs.EnableTabSwap(FALSE);
    // style des tabulations
    m_wndTabs.ModifyTabStyle(CMFCTabCtrl::STYLE_3D_ROUNDED_SCROLL);
    // onglet actif en gras
    m_wndTabs.SetActiveTabBoldFont(TRUE);

    // creation de la fenetre console trace
    m_pHistoConsoleTraces.reset(new CAdeOXHistoryCtrl(false)); // pas de log possible dans un fichier
    if (m_pHistoConsoleTraces.get() == NULL)
    {
        return -1;
    }
    if (!m_pHistoConsoleTraces->Create(NULL,NULL,WS_EX_CLIENTEDGE|WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL,CRect(0,0,0,0),&m_wndTabs,1))
    {
        return -1;
    }
//    m_pHistoConsoleTraces->SetBackColor(RGB(0,0,0)); // background noir par defaut
    CFont DinaFont;
    VERIFY(DinaFont.CreatePointFont(100, _T("Dina")));
    m_pHistoConsoleTraces->SetFont(&DinaFont);
    CString strTabName;
	BOOL bNameValid = strTabName.LoadString(STR_TRC_CONSOLE);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(m_pHistoConsoleTraces.get(), strTabName, -1, FALSE);

    // creation de la fenetre LUA trace
    if (m_bLuaWindowTraceToCreate)
    {
        m_pHistoLuaTraces.reset(new CAdeOXHistoryCtrl(true)); // log to file allowed
        if (m_pHistoLuaTraces.get() == NULL)
        {
            return -1;
        }
        if (!m_pHistoLuaTraces->Create(NULL,NULL,WS_EX_CLIENTEDGE|WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL,CRect(0,0,0,0),&m_wndTabs,1))
        {
            return -1;
        }
        // ajout dans le container
	    BOOL bNameValid = strTabName.LoadString(STR_TRC_LUA_OUTPUT);
	    ASSERT(bNameValid);
	    m_wndTabs.AddTab(m_pHistoLuaTraces.get(), strTabName, -1, FALSE);
        m_pHistoLuaTraces->SetFont(&DinaFont);
        m_idxHistoLuaTracesPage = 1;
    }

    DinaFont.DeleteObject();
    
    // trace console active par defaut
    m_wndTabs.SetActiveTab(0);

    // lancement du thread qui depilera les traces console (basse priorite)
    // et ira lire les traces processeurs
    m_pThreadManagement.reset(new CAdeThreadAsMember(StaticProcThreadManagement, this));
    m_pThreadManagement->BeginThread(THREAD_PRIORITY_LOWEST);

    return 0;                    
}

void CTraceBar::ClearTraceWindow(T_TYPE_TRACE typeTrace)
{
    switch(typeTrace)
    {
    case CONSOLE_TRACE:
        m_pHistoConsoleTraces->ClearHistory();
        break;
    case LUA_TRACE:
        if (m_pHistoLuaTraces.get())
        {
            m_pHistoLuaTraces->ClearHistory();
            // Et on la met au premier plan (la fonction clear est appellee
            // au debut de l'excution d'un script LUA
            m_wndTabs.SetActiveTab(m_idxHistoLuaTracesPage);
        }
        break;
    }
}

void CTraceBar::ShowTraceWindow(T_TYPE_TRACE typeTrace)
{
    switch(typeTrace)
    {
    case CONSOLE_TRACE:
            m_wndTabs.SetActiveTab(0);
        break;
    case LUA_TRACE:
        if (m_pHistoLuaTraces.get())
        {
            m_wndTabs.SetActiveTab(m_idxHistoLuaTracesPage);
        }
        else
        {
            // On montre la page 0 par defaut
            m_wndTabs.SetActiveTab(0);
        }
        break;
    }
}

void CTraceBar::AddToTraceWindow(char* szTr, T_TYPE_TRACE typeTrace, COLORREF color /*= RGB(0,0,0)*/)
{
    char szTrace[1000];
    strncpy_s(szTrace, sizeof(szTrace), szTr, _TRUNCATE);
    // on enleve les '\r\n' qui nous vienne du DOS (cf AdeTrace)
    char* pCar = strchr(szTrace, '\r');
    if (pCar)
    {
        *pCar = 0;
    }
    pCar = strchr(szTrace, '\n');
    if (pCar)
    {
        *pCar = 0;
    }
    // on s'alloue une trace et on la place dans la FIFO
    CTraceMsg* pMsg = new CTraceMsg(szTrace, color, typeTrace);
    m_fifoTraces.Push(pMsg, true); // detruit le message si l'insertion echoue
}

//-------------------------------------------------------------------
UINT CTraceBar::StaticProcThreadManagement(LPVOID lpParam, HANDLE hEventStop)
{
    return ((CTraceBar*)lpParam)->ProcThreadManagement(hEventStop);
}
UINT CTraceBar::ProcThreadManagement(HANDLE hEventStop)
//-------------------------------------------------------------------
{
    while (1)
    {
        // Lecture des donnees dans la fifo
        CTraceMsg* pMsg = m_fifoTraces.Pop(100); // timeout de 100 ms pour regarder regulierement si arret demande
        if (pMsg) 
        {
            // ok, on a lu un message,  affiche les donnees
            switch(pMsg->m_typeTrace)
            {
            case CONSOLE_TRACE:
                m_pHistoConsoleTraces->SetTextColor(pMsg->m_color);
                m_pHistoConsoleTraces->AddLine(pMsg->m_szTrace);
                break;
            case LUA_TRACE:
                if (m_pHistoLuaTraces.get())
                {
                    m_pHistoLuaTraces->SetTextColor(pMsg->m_color);
                    m_pHistoLuaTraces->AddLine(pMsg->m_szTrace);
                    // On ecrit la trace dans le fichier de log
                    // On utilise SendWithoutOutputToConsole afin de ne pas logger le message
                    // dans la console en utilisant Send (qui appelle la callback => le message serait
                    // egalement affiche dans le log)
                    CAdeTrace::ReadSingleInstance()->SendWithoutOutputToConsole(1, ADE_TRACE_LEVEL_WARNING, ADE_TRACE_COLOR_WARNING, pMsg->m_szTrace);
                }
                else
                {
                    // On affiche sur la console (cas ou on est en user level engineer - la fenetre LUA n'est pas creee -
                    // et ou l'utilisateur lance une commande utilisateur
                    m_pHistoConsoleTraces->SetTextColor(pMsg->m_color);
                    m_pHistoConsoleTraces->AddLine(pMsg->m_szTrace);
                }
                break;
            }
            delete pMsg;
        }

        // on regarde si evenement stop signale
        if (WaitForSingleObject(hEventStop, 0) == WAIT_OBJECT_0)
        {
            // arret demande
            return 0;
        }
    }
    return 0;
}

//-------------------------------------------------------------------
void CTraceBar::ClearFifoTraces(void)
//-------------------------------------------------------------------
{
    m_fifoTraces.Flush();
}
