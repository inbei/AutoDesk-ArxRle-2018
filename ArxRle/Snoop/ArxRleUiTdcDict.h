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

#ifndef ARXRLEUITDCDICT_H
#define ARXRLEUITDCDICT_H

#include "Resource.h"
#include "ArxRleUiTdcDbObjectBase.h"

/****************************************************************************
**
**	CLASS ArxRleUiTdcDict:
**
**	**jma
**
*************************************/

class ArxRleUiTdcDict : public ArxRleUiTdcDbObjectBase {

public:
				ArxRleUiTdcDict(const AcDbObjectId& dictId, bool isExtDict);
    virtual		~ArxRleUiTdcDict()    {}

    //{{AFX_DATA(ArxRleUiTdcDict)
    enum { IDD = ARXRLE_TDC_DICTS };
    CTreeCtrl	m_dictTree;
    CListCtrl	m_dataList;
    BOOL		m_dictIsHardOwner;
    CComboBox	m_puMergeStyle;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ArxRleUiTdcDict)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

protected:
    // Generated message map functions
    //{{AFX_MSG(ArxRleUiTdcDict)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangedDict(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEndlabeleditTreeDict(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKeydownTreeDict(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSetHardOwner();
    afx_msg void OnMergeStyleChanged();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
		// data members
    AcDbObjectIdArray	m_dictObjIdList;    // "stack" of dictionary Object Ids
    AcDbObjectId        m_rootObjId;
    bool                m_isRoot;
    int                 m_curIndex;

		// helper functions
    HTREEITEM	addOneTreeItem(LPCTSTR name, const AcDbObjectId& objId, HTREEITEM parent);
    void        addOneDictionary(const AcDbObjectId& dictId, HTREEITEM parent);
	void		setDictInfoButtons(AcDbObject* obj);
    void        displayCurrent(int index);
    bool        renameCurrentItem(LPCTSTR oldName, LPCTSTR newName);
    void        deleteCurrentItem();

	AcDb::DuplicateRecordCloning	indexToDrc(int index) const;
	int								drcToIndex(AcDb::DuplicateRecordCloning drc) const;
};

#endif    // ARXRLEUITDCDICT_H
