// CAdeMFCToolBar.cpp : surcharge de CMFCToolBar dans laquelle on ne sauvegarde pas l'etat des boutons
// de la barre

#include "stdafx.h"
#include "OngTV.h"

// pour la surcharge de 
#include <afxregpath.h>
#include <afxdocksite.h>
#include <afxdockingpanesrow.h>

#include "CheckComboBox.h"
#include "OXMultiComboBox.h"
#include "AdeMFCToolBar.h"


#define AFX_REG_SECTION_FMT _T("%sMFCToolBar-%d")
#define AFX_REG_SECTION_FMT_EX _T("%sMFCToolBar-%d%x")
#define AFX_REG_PARAMS_FMT _T("%sMFCToolBarParameters")
#define AFX_REG_ENTRY_NAME _T("Name")
#define AFX_REG_ENTRY_BUTTONS _T("Buttons")
#define AFX_REG_ENTRY_ORIG_ITEMS _T("OriginalItems")
#define AFX_REG_ENTRY_TOOLTIPS _T("Tooltips")
#define AFX_REG_ENTRY_KEYS _T("ShortcutKeys")
#define AFX_REG_ENTRY_LARGE_ICONS _T("LargeIcons")
#define AFX_REG_ENTRY_ANIMATION _T("MenuAnimation")
#define AFX_REG_ENTRY_RU_MENUS _T("RecentlyUsedMenus")
#define AFX_REG_ENTRY_MENU_SHADOWS _T("MenuShadows")
#define AFX_REG_ENTRY_SHOW_ALL_MENUS_DELAY _T("ShowAllMenusAfterDelay")
#define AFX_REG_ENTRY_CMD_USAGE_COUNT _T("CommandsUsage")
#define AFX_REG_ENTRY_RESET_ITEMS _T("OrigResetItems")
static const CString strToolbarProfile = _T("MFCToolBars");

IMPLEMENT_SERIAL(CAdeMFCMenuBar, CMFCMenuBar, 1)
// on ne sauvegarde pas les menus (sinon, ca pose des pb quand on passe d'un mode user à l'autre
BOOL CAdeMFCMenuBar::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
    return FALSE;
}
BOOL CAdeMFCMenuBar::SaveState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
    return FALSE;
}

IMPLEMENT_SERIAL(CAdeMFCToolBar, CMFCToolBar, 1)

// reprise du code de la methode LoadState de CMFCToolBar et on desactive le code correspondant
// a la restauration des boutons
BOOL CAdeMFCToolBar::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
    BOOL bResult = FALSE;
#if 0
    CString strProfileName = ::AFXGetRegPath(strToolbarProfile, lpszProfileName);


    if (nIndex == -1)
    {
        nIndex = GetDlgCtrlID();
    }

    CString strSection;
    if (uiID == (UINT) -1)
    {
        strSection.Format(AFX_REG_SECTION_FMT, (LPCTSTR)strProfileName, nIndex);
    }
    else
    {
        strSection.Format(AFX_REG_SECTION_FMT_EX, (LPCTSTR)strProfileName, nIndex, uiID);
    }

    LPBYTE lpbData = NULL;
    UINT uiDataSize;

    CSettingsStoreSP regSP;
    CSettingsStore& reg = regSP.Create(FALSE, TRUE);

    if (!reg.Open(strSection))
    {
        return FALSE;
    }

    if (!reg.Read(AFX_REG_ENTRY_BUTTONS, &lpbData, &uiDataSize))
    {
        return FALSE;
    }

    try
    {
        CMemFile file(lpbData, uiDataSize);
        CArchive ar(&file, CArchive::load);

        Serialize(ar);
        bResult = TRUE;
    }
    catch(CMemoryException* pEx)
    {
        pEx->Delete();
        TRACE(_T("Memory exception in CMFCToolBar::LoadState()!\n"));
    }
    catch(CArchiveException* pEx)
    {
        pEx->Delete();
        TRACE(_T("CArchiveException exception in CMFCToolBar::LoadState()!\n"));

        m_Buttons.RemoveAll(); // Memory leak! Don't delete wrong objects.
        if (CanBeRestored())
        {
            RestoreOriginalstate();
        }

    }

    if (lpbData != NULL)
    {
        delete [] lpbData;
    }

    LoadResetOriginalState(reg);

    CWinAppEx* pApp = DYNAMIC_DOWNCAST(CWinAppEx, AfxGetApp());

    if (bResult &&
        pApp != NULL &&
        pApp->IsResourceSmartUpdate())
    {
        LoadLastOriginalState(reg);
    }
#endif

    bResult = CPane::LoadState(lpszProfileName, nIndex, uiID);

    AdjustLayout();

    if (m_pParentDockBar != NULL && m_pDockBarRow != NULL)
    {
        ASSERT_VALID(m_pParentDockBar);
        ASSERT_VALID(m_pDockBarRow);

        CSize sizeCurr = CalcFixedLayout(TRUE, IsHorizontal());
        m_pParentDockBar->ResizeRow(m_pDockBarRow, IsHorizontal() ? sizeCurr.cy : sizeCurr.cx);
    }

    return bResult;
}
CMFCToolBarButton* CAdeMFCToolBar::GetButtonFromItemID(UINT itemID) const
{
    for (int i = 0; i < GetCount() ; i++)
    {
        if (itemID == GetItemID(i))
        {
            // C'est le bouton
            return GetButton(i);
        }
    }
    return NULL;
}

IMPLEMENT_SERIAL(CAdeMFCToolBarEditBoxButton, CMFCToolBarEditBoxButton, 1)

// surcharge de CMFCToolBarEditBoxButton pour pouvoir afficher le tooltip correspondant a l'ID (surcharge de OnGetCustomToolTipText)
CAdeMFCToolBarEditBoxButton::CAdeMFCToolBarEditBoxButton() : CMFCToolBarEditBoxButton()
{
}

CAdeMFCToolBarEditBoxButton::CAdeMFCToolBarEditBoxButton(UINT uiID, int iImage, DWORD dwStyle /*= ES_AUTOHSCROLL*/, int iWidth /*= 0*/)
: CMFCToolBarEditBoxButton(uiID, iImage, dwStyle, iWidth)
{
}

BOOL CAdeMFCToolBarEditBoxButton::OnGetCustomToolTipText(CString& strToolTip)
{
    strToolTip = theApp.LoadString(m_nID);
    return TRUE;
}
BOOL CAdeMFCToolBarEditBoxButton::NotifyCommand(int iNotifyCode)
{
	if (m_pWndEdit->GetSafeHwnd() == NULL)
	{
		return FALSE;
	}

	switch (iNotifyCode)
	{
	case EN_UPDATE:
#if CODE_VOLONTAIREMENT_DESACTIVE
        // afin de résoudre le ticket #300
		{
			m_pWndEdit->GetWindowText(m_strContents);

			// Try set selection in ALL editboxes with the same ID:
			CObList listButtons;
			if (CMFCToolBar::GetCommandButtons(m_nID, listButtons) > 0)
			{
				for (POSITION posCombo = listButtons.GetHeadPosition(); posCombo != NULL;)
				{
					CMFCToolBarEditBoxButton* pEdit = DYNAMIC_DOWNCAST(CMFCToolBarEditBoxButton, listButtons.GetNext(posCombo));

					if ((pEdit != NULL) &&(pEdit != this))
					{
						pEdit->SetContents(m_strContents);
					}
				}
			}
		}
#endif
		return !m_bChangingText;
	}
	return FALSE;
}


IMPLEMENT_SERIAL(CAdeMFCToolBarComboBoxButton, CMFCToolBarComboBoxButton, 1)

CAdeMFCToolBarComboBoxButton::CAdeMFCToolBarComboBoxButton() : CMFCToolBarComboBoxButton()
{
}

CAdeMFCToolBarComboBoxButton::CAdeMFCToolBarComboBoxButton(UINT uiID, int iImage, DWORD dwStyle /*= CBS_DROPDOWNLIST*/, int iWidth /*= 0*/)
: CMFCToolBarComboBoxButton(uiID, iImage, dwStyle, iWidth)
{
}

BOOL CAdeMFCToolBarComboBoxButton::OnGetCustomToolTipText(CString& strToolTip)
{
    strToolTip = theApp.LoadString(m_nID);
    return TRUE;
}

IMPLEMENT_SERIAL(CAdeMFCToolBarCheckComboBoxButton, CAdeMFCToolBarComboBoxButton, 1)

CAdeMFCToolBarCheckComboBoxButton::CAdeMFCToolBarCheckComboBoxButton() : CAdeMFCToolBarComboBoxButton()
{
}

CAdeMFCToolBarCheckComboBoxButton::CAdeMFCToolBarCheckComboBoxButton(UINT uiID, int iImage, DWORD dwStyle /*= CBS_DROPDOWNLIST*/, int iWidth /*= 0*/)
: CAdeMFCToolBarComboBoxButton(uiID, iImage, dwStyle, iWidth)
{
}
CComboBox* CAdeMFCToolBarCheckComboBoxButton::CreateCombo(CWnd* pWndParent, const CRect& rect)
{
    CComboBox* pWndCombo = new CCheckComboBox;
    if (!pWndCombo->Create(m_dwStyle, rect, pWndParent, m_nID))
    {
        delete pWndCombo;
        return NULL;
    }

    return pWndCombo;
}
// Surcharge de NotifyCommand pour traiter la commande CBN_CLOSEUP
BOOL CAdeMFCToolBarCheckComboBoxButton::NotifyCommand(int iNotifyCode)
{
    if (m_pWndCombo->GetSafeHwnd() == NULL)
    {
        return FALSE;
    }

    if (m_bFlat && iNotifyCode == 0)
    {
        return TRUE;
    }

    if ((CBN_CLOSEUP == iNotifyCode) || (CBN_SELCHANGE == iNotifyCode))
    {
        // on affiche la chaine calculee par la CCheckCombo
        // On met a jour la chaine interne
        m_pWndCombo->GetWindowText(m_strEdit);
        // et on force le reaffichage
        m_pWndCombo->GetParent()->InvalidateRect(m_rectCombo);
        m_pWndCombo->GetParent()->UpdateWindow();
        return TRUE;
    }

    return FALSE;
}


IMPLEMENT_SERIAL(CAdeMFCToolBarOXMultiComboBoxButton, CAdeMFCToolBarComboBoxButton, 1)

CAdeMFCToolBarOXMultiComboBoxButton::CAdeMFCToolBarOXMultiComboBoxButton() : CAdeMFCToolBarComboBoxButton()
{
}

CAdeMFCToolBarOXMultiComboBoxButton::CAdeMFCToolBarOXMultiComboBoxButton(UINT uiID, int iImage, DWORD dwStyle /*= CBS_DROPDOWNLIST*/, int iWidth /*= 0*/)
: CAdeMFCToolBarComboBoxButton(uiID, iImage, dwStyle, iWidth)
{
}
CComboBox* CAdeMFCToolBarOXMultiComboBoxButton::CreateCombo(CWnd* pWndParent, const CRect& rect)
{
    CComboBox* pWndCombo = new COXMultiComboBox;
    if (!pWndCombo->Create(m_dwStyle, rect, pWndParent, m_nID))
    {
        delete pWndCombo;
        return NULL;
    }

    return pWndCombo;
}
