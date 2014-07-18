// CAdeMFCToolBar.h : surcharge de CMFCToolBar dans laquelle on ne sauvegarde pas l'etat des boutons
// de la barre


#pragma once

#include <afxtoolbar.h>
#include <afxmenubar.h>
#include <afxtoolbareditboxbutton.h>
#include <afxtoolbarcomboboxbutton.h>

class CAdeMFCMenuBar : public CMFCMenuBar
{
	DECLARE_SERIAL(CAdeMFCMenuBar)
    virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
    virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
};

class CAdeMFCToolBar : public CMFCToolBar
{
	DECLARE_SERIAL(CAdeMFCToolBar)
    virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	CMFCToolBarButton* GetButtonFromItemID(UINT itemID) const;
};


// surcharge de CMFCToolBarEditBoxButton pour pouvoir afficher le tooltip correspondant a l'ID (surcharge de OnGetCustomToolTipText)
// et de NotifyCommand afin de ne pas modifier tous les contrôles ayant un même ID lors de la mise à jour
class CAdeMFCToolBarEditBoxButton : public CMFCToolBarEditBoxButton
{
	DECLARE_SERIAL(CAdeMFCToolBarEditBoxButton)
public:
    CAdeMFCToolBarEditBoxButton();
    CAdeMFCToolBarEditBoxButton(UINT uiID, int iImage, DWORD dwStyle = ES_AUTOHSCROLL, int iWidth = 0);
    virtual BOOL OnGetCustomToolTipText(CString& strToolTip);
	virtual BOOL NotifyCommand(int iNotifyCode);
};

// surcharge de CMFCToolBarComboBoxButton pour pouvoir afficher le tooltip correspondant a l'ID (surcharge de OnGetCustomToolTipText)
class CAdeMFCToolBarComboBoxButton : public CMFCToolBarComboBoxButton
{
	DECLARE_SERIAL(CAdeMFCToolBarComboBoxButton)
public:
    CAdeMFCToolBarComboBoxButton();
    CAdeMFCToolBarComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0);
    virtual BOOL OnGetCustomToolTipText(CString& strToolTip);
};

class CAdeMFCToolBarCheckComboBoxButton : public CAdeMFCToolBarComboBoxButton
{
	DECLARE_SERIAL(CAdeMFCToolBarCheckComboBoxButton)
public:
    CAdeMFCToolBarCheckComboBoxButton();
    CAdeMFCToolBarCheckComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0);
    virtual CComboBox* CreateCombo(CWnd* pWndParent, const CRect& rect);
    virtual BOOL NotifyCommand(int iNotifyCode);
};

class CAdeMFCToolBarOXMultiComboBoxButton : public CAdeMFCToolBarComboBoxButton
{
	DECLARE_SERIAL(CAdeMFCToolBarOXMultiComboBoxButton)
public:
    CAdeMFCToolBarOXMultiComboBoxButton();
    CAdeMFCToolBarOXMultiComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0);
    virtual CComboBox* CreateCombo(CWnd* pWndParent, const CRect& rect);
};



