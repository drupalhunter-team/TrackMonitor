
#pragma once

class CProcessor;

class CAdeFormView : public CFormView
{
NON_COPYABLE(CAdeFormView);
protected:
    CAdeFormView();
    CAdeFormView(UINT nIDTemplate) : CFormView(nIDTemplate) {};
    virtual ~CAdeFormView() {};
public:
    virtual CProcessor*  GetProcessor(void) {return NULL;};
    virtual unsigned GetInfoSup(void) {return 0;};
};

class CAdeFormViewAttachToProc : public CAdeFormView
{
protected:
    CAdeFormViewAttachToProc() {m_pProc = NULL;};
    CAdeFormViewAttachToProc(UINT nIDTemplate) : CAdeFormView(nIDTemplate) {m_pProc = NULL;m_infoSup=0;};
    virtual ~CAdeFormViewAttachToProc() {};

    CProcessor* m_pProc;    // processeur auquel est lie la vue
    unsigned    m_infoSup;  // info supplementaire attachee a la vue
public:
    virtual CProcessor* GetProcessor(void) {return m_pProc;};
    virtual unsigned GetInfoSup(void) {return m_infoSup;};

};
