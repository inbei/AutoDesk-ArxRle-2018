//
//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2017 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
//

#ifndef ARXRLEUITDMLONGTRANS_H
#define ARXRLEUITDMLONGTRANS_H

#include "Resource.h"

class ArxRleUiTdcLongTrans;


/****************************************************************************
**
**	CLASS ArxRleUiTdmLongTrans:
**
**	**jma
**
*************************************/

class ArxRleUiTdmLongTrans : public CAcUiTabMainDialog {

public:
            ArxRleUiTdmLongTrans(AcDbLongTransaction* longTrans, CWnd* parent, const TCHAR* winTitle = NULL);
	virtual	~ArxRleUiTdmLongTrans();

    //{{AFX_DATA(ArxRleUiTdmLongTrans)
    enum { IDD = ARXRLE_TDM_DATABASE };
	CAdUiTab	m_tabCtrl;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ArxRleUiTdmLongTrans)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

protected:
    // Generated message map functions
    //{{AFX_MSG(ArxRleUiTdmLongTrans)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
        // data members
	CString					m_winTitle;
	ArxRleUiTdcLongTrans*	m_tdcLongTrans;
};

#endif // ARXRLEUITDMLONGTRANS_H

