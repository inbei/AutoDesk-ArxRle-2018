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
#include "StdAfx.h"

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

#include "ArxRleUiTdcWblockClone.h"
#include "AcadUtils/ArxRleUtils.h"
#include "ArxRle.h"
#include "Reactor/ArxRleAppEditorReactor.h"
#include "Snoop/ArxRleUiTdmObjects.h"
#include "Snoop/ArxRleUiTdmDatabase.h"
#include "ArxRleUiTdmTransDbClones.h"
#include "AcadUtils/ArxRleDocLockWrite.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/****************************************************************************
**
**  ArxRleUiTdcWblockClone::ArxRleUiTdcWblockClone
**
**  **jma
**
*************************************/

ArxRleUiTdcWblockClone::ArxRleUiTdcWblockClone()
:   CAcUiTabChildDialog(NULL, ArxRleApp::getApp()->dllInstance()),
    m_cloneSet(ArxRleAppEditorReactor::getInstance()->cloneSet())
{
    //{{AFX_DATA_INIT(ArxRleUiTdcWblockClone)
    m_puBlockDefIndex = 0;
    m_doForceCopy = FALSE;
    m_saveAsDwg = FALSE;
    //}}AFX_DATA_INIT
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::DoDataExchange
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::DoDataExchange(CDataExchange* pDX)
{
    CAcUiTabChildDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(ArxRleUiTdcWblockClone)
	DDX_Control(pDX, ARXRLE_WBLOCK_BN_PICKPT, m_pickPtButton);
	DDX_Control(pDX, ARXRLE_WBLOCK_LB_SOURCE_DB, m_lbSourceDb);
	DDX_Control(pDX, ARXRLE_WBLOCK_LB_DEST_DB, m_lbDestDb);
	DDX_CBIndex(pDX, ARXRLE_WBLOCK_CB_BLKDEFS, m_puBlockDefIndex);
	DDX_Control(pDX, ARXRLE_WBLOCK_CB_BLKDEFS, m_puBlockDef);
	DDX_Check(pDX, ARXRLE_WBLOCK_CK_FORCE_WBLOCK_COPY, m_doForceCopy);
	DDX_Check(pDX, ARXRLE_WBLOCK_CK_SAVE_AS_DWG, m_saveAsDwg);
    //}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// ArxRleUiTdcWblockClone message handlers

BEGIN_MESSAGE_MAP(ArxRleUiTdcWblockClone, CAcUiTabChildDialog)
    //{{AFX_MSG_MAP(ArxRleUiTdcWblockClone)
	ON_LBN_SELCHANGE(ARXRLE_WBLOCK_LB_SOURCE_DB, OnSourceDatabaseChanged)
	ON_LBN_SELCHANGE(ARXRLE_WBLOCK_LB_DEST_DB, OnDestDatabaseChanged)
	ON_BN_CLICKED(ARXRLE_WBLOCK_BN_WBLOCK_CLONE_SET, OnWblockObjects)
	ON_BN_CLICKED(ARXRLE_WBLOCK_BN_WBLOCK_BLKDEF, OnWblockBlkDef)
	ON_BN_CLICKED(ARXRLE_WBLOCK_BN_WBLOCK_ALL, OnWblockAll)
	ON_BN_CLICKED(ARXRLE_WBLOCK_BN_PICKPT, OnPickBasePt)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ArxRleUiTdcWblockClone message handlers


/****************************************************************************
**
**  ArxRleUiTdcWblockClone::OnInitDialog
**
**  **jma
**
*************************************/

BOOL
ArxRleUiTdcWblockClone::OnInitDialog() 
{
    CAcUiTabChildDialog::OnInitDialog();

    m_pickPtButton.AutoLoad();

    displayDbLists();

    CString str;
    SetDlgItemText(ARXRLE_WBLOCK_EB_BASE_PT, ArxRleUtils::ptToStr(m_basePt, str));

    return TRUE;
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::displayDbLists
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::displayDbLists() 
{
    m_dbPtrs.setLogicalLength(0);
    m_lbSourceDb.ResetContent();
    m_lbDestDb.ResetContent();

    m_cloneSet.getAllDatabases(m_dbPtrs);

    CString str;
    AcDbDatabase* tmpDb;

    int len = m_dbPtrs.length();
    for (int i=0; i<len; i++) {
        tmpDb = static_cast<AcDbDatabase*>(m_dbPtrs[i]);

        ArxRleUtils::dbToStr(tmpDb, str);
        m_lbSourceDb.AddString(str);
        m_lbDestDb.AddString(str);
    }

        // Add a "temp" database to the destination side
    m_dbPtrs.append(NULL);
    m_lbDestDb.AddString(_T("** TEMP DATABASE **"));

    m_lbSourceDb.SetCurSel(0);
    m_lbDestDb.SetCurSel(0);

    fillBlockList();
    setButtonModes();
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::fillBlockList
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::fillBlockList() 
{
    m_blockList.RemoveAll();

    AcDbDatabase* curDb = getSourceDb();
    ASSERT(curDb != NULL);

    Acad::ErrorStatus es;
    es = ArxRleUtils::collectBlockIds(m_blockList, false, false, false, curDb);

    ASSERT(es == Acad::eOk);

    ArxRleUtils::fillListBox(m_blockList, m_puBlockDef, 0);
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::OnSourceDatabaseChanged
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::OnSourceDatabaseChanged() 
{
    fillBlockList();
    setButtonModes();
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::OnDestDatabaseChanged
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::OnDestDatabaseChanged() 
{
    setButtonModes();
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::setButtonModes
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::setButtonModes()
{
    AcDbDatabase* sourceDb = getSourceDb();
    AcDbDatabase* destDb = getDestDb();

    bool allowClones;
    if (sourceDb == destDb)
        allowClones = false;
    else
        allowClones = true;

	ArxRleUtils::enableDlgItem(this, ARXRLE_WBLOCK_BN_WBLOCK_CLONE_SET, allowClones);
	ArxRleUtils::enableDlgItem(this, ARXRLE_WBLOCK_BN_WBLOCK_BLKDEF, allowClones);
	ArxRleUtils::enableDlgItem(this, ARXRLE_WBLOCK_BN_WBLOCK_ALL, allowClones);
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::OnWblockObjects
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::OnWblockObjects() 
{
    AcDbDatabase* sourceDb = getSourceDb();
    ASSERT(sourceDb != NULL);

    AcDbObjectIdArray objIds;
    m_cloneSet.getObjectsForDatabase(sourceDb, objIds);

    // unfortunately, wblock is a little restrictive on what
    // it will allow to be cloned automatically.  So, we factor
    // out all the entities and allow those to be cloned, and
    // then our ArxRleAppEditorReactor will handle the rest by hand.
    AcDbObjectIdArray okToClone, mustCloneByHand;
    divideCloneSet(objIds, mustCloneByHand, okToClone);

    // lock the document we are wblocking from
    ArxRleDocLockWrite docLock(sourceDb);
    if (docLock.lock() != Acad::eOk)
        return;
    // restore database to non-xref state (xes returns eOk if this
    // was necessary, and something else if it wasn't).
    Acad::ErrorStatus xes = sourceDb->restoreOriginalXrefSymbols();
    AcDbDatabase* tmpDb = NULL;
    Acad::ErrorStatus es = sourceDb->wblock(tmpDb, okToClone, m_basePt);
    if (xes ==Acad::eOk)
        xes = sourceDb->restoreForwardingXrefSymbols();
    if (es == Acad::eOk) {
        doInsertOption(tmpDb);

        delete tmpDb;   // we're done with it
    }
    else {
        CString str;
        str.Format(_T("Wblock failed: %s"), ArxRleUtils::rxErrorStr(es));
        ArxRleUtils::stopAlertBox(str);

        delete tmpDb;   // yes, a failed wblock can leave a partially created database!
                        // so always delete it.
    }
}

/****************************************************************************
**
**  ArxRleUiTdcTransientReactors::OnWblockBlkDef
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::OnWblockBlkDef()
{
        // get the block we're suppose to be wblocking
    int index = m_puBlockDef.GetCurSel();
    ASSERT(index != CB_ERR);

    CString str;
    AcDbObjectId blkDefId;
    if (!m_blockList.GetAtIndex(index, str, blkDefId)) {
        ASSERT(0);
        return;
    }

    AcDbDatabase* sourceDb = getSourceDb();
    ASSERT(sourceDb != NULL);

        // lock the document we are wblocking from
    ArxRleDocLockWrite docLock(sourceDb);
    if (docLock.lock() != Acad::eOk)
        return;

    // restore database to non-xref state (xes returns eOk if this
    // was necessary, and something else if it wasn't).
    Acad::ErrorStatus xes = sourceDb->restoreOriginalXrefSymbols();
    AcDbDatabase* tmpDb = NULL;
    Acad::ErrorStatus es = sourceDb->wblock(tmpDb, blkDefId);
    if (xes ==Acad::eOk)
        xes = sourceDb->restoreForwardingXrefSymbols();
    if (es == Acad::eOk) {
        doInsertOption(tmpDb);

        delete tmpDb;   // we're done with it
    }
    else {
        CString str;
        str.Format(_T("Wblock failed: %s"), ArxRleUtils::rxErrorStr(es));
        ArxRleUtils::stopAlertBox(str);

        delete tmpDb;   // yes, a failed wblock can leave a partially created database!
                        // so always delete it.
    }
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::OnWblockAll
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::OnWblockAll()
{
    UpdateData(TRUE);

    AcDbDatabase* sourceDb = getSourceDb();
    ASSERT(sourceDb != NULL);

    if (m_doForceCopy)
        sourceDb->forceWblockDatabaseCopy();

        // lock the document we are wblocking from
    ArxRleDocLockWrite docLock(sourceDb);
    if (docLock.lock() != Acad::eOk)
        return;

    // restore database to non-xref state (xes returns eOk if this
    // was necessary, and something else if it wasn't).
    Acad::ErrorStatus xes = sourceDb->restoreOriginalXrefSymbols();
    AcDbDatabase* tmpDb = NULL;
    Acad::ErrorStatus es = sourceDb->wblock(tmpDb);
    if (xes ==Acad::eOk)
        xes = sourceDb->restoreForwardingXrefSymbols();
    if (es == Acad::eOk) {
        doInsertOption(tmpDb);

        delete tmpDb;   // we're done with it
    }
    else {
        CString str;
        str.Format(_T("Wblock failed: %s"), ArxRleUtils::rxErrorStr(es));
        ArxRleUtils::stopAlertBox(str);

        delete tmpDb;   // yes, a failed wblock can leave a partially created database!
                        // so always delete it.
    }
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::doSaveOption
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::doSaveOption(AcDbDatabase* tmpDb)
{
    UpdateData(TRUE);

    if (m_saveAsDwg) {
        CString fname;
        Acad::ErrorStatus es;
        es = ArxRleUtils::getFileNameForWrite(_T("Save Database As"), NULL, _T("dwg"),
                                        fname, false, false);

        if (es == Acad::eOk) {
            es = tmpDb->saveAs(fname);
            if (es != Acad::eOk) {
                CString str;
                str.Format(_T("SaveAs failed: %s"), ArxRleUtils::rxErrorStr(es));
                ArxRleUtils::stopAlertBox(str);
            }
        }
    }
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::doInsertOption
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::doInsertOption(AcDbDatabase* tmpDb)
{
    AcDbDatabase* destDb = getDestDb();
    if (destDb == NULL) {
        ArxRleUiTdmDatabase dbox(tmpDb, this, _T("Wblock'd Database"));
        dbox.DoModal();

        doSaveOption(tmpDb);
    }
    else {
        Acad::ErrorStatus es;

            // lock the document we are inserting into
        ArxRleDocLockWrite docLock(destDb);
        if (docLock.lockAndSetCurrent() != Acad::eOk)
            return;

        es = destDb->insert(AcGeMatrix3d::kIdentity, tmpDb, false);
        if (es == Acad::eOk) {
            ArxRleUiTdmDatabase dbox(destDb, this, _T("Wblock/Insert Database"));
            dbox.DoModal();

            doSaveOption(destDb);
        }
        else {
            CString str;
            str.Format(_T("Insert failed: %s"), ArxRleUtils::rxErrorStr(es));
            ArxRleUtils::stopAlertBox(str);
        }
    }
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::divideCloneSet
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::divideCloneSet(const AcDbObjectIdArray& cloneSet,
                            AcDbObjectIdArray& nonEntSet,
                            AcDbObjectIdArray& okToCloneSet)
{
    Acad::ErrorStatus es;
    AcDbObject* obj;

    int len = cloneSet.length();
    for (int i=0; i<len; i++) {
        es = acdbOpenAcDbObject(obj, cloneSet[i], AcDb::kForRead);
        if (es == Acad::eOk) {
            if (obj->isKindOf(AcDbEntity::desc()))
                okToCloneSet.append(obj->objectId());
            else
                nonEntSet.append(obj->objectId());

            obj->close();
        }
    }
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::OnPickBasePt
**
**  **jma
**
*************************************/

void
ArxRleUiTdcWblockClone::OnPickBasePt() 
{
        // unfortunately, this structure for making this page know about the main
        // dialog means that we could never use this page within another main dialog
        // container.  Oh well...  if only PropertySheets!
    ArxRleUiTdmTransDbClones* tdmClones = static_cast<ArxRleUiTdmTransDbClones*>(GetMainDialog());
    tdmClones->getBasePoint(m_basePt);

    CString str;
    SetDlgItemText(ARXRLE_WBLOCK_EB_BASE_PT, ArxRleUtils::ptToStr(m_basePt, str));
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::getSourceDb
**
**  **jma
**
*************************************/

AcDbDatabase*
ArxRleUiTdcWblockClone::getSourceDb() 
{
    int index = m_lbSourceDb.GetCurSel();
    if (index == LB_ERR) {
        ASSERT(0);
        return NULL;
    }

    AcDbDatabase* curDb = static_cast<AcDbDatabase*>(m_dbPtrs[index]);
    ASSERT(curDb != NULL);
    return curDb;
}

/****************************************************************************
**
**  ArxRleUiTdcWblockClone::getDestDb
**
**  **jma
**
*************************************/

AcDbDatabase*
ArxRleUiTdcWblockClone::getDestDb() 
{
    int index = m_lbDestDb.GetCurSel();
    if (index == LB_ERR) {
        ASSERT(0);
        return NULL;
    }

    AcDbDatabase* curDb = static_cast<AcDbDatabase*>(m_dbPtrs[index]);
    return curDb;
}

