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

#ifndef ARXRLEUIPRINT_H
#define ARXRLEUIPRINT_H

#include "ArxRleUiPrBase.h"

/****************************************************************************
**
**  CLASS ArxRleUiPrInt:
**
**  **jma
**
*************************************/

class ArxRleUiPrInt : public ArxRleUiPrBase {

public:
        // type restricts input to an appropriate range
    enum IntType {
        kAny    = 0,
        kNoNeg,
        kNoZero,
        kNoNegNoZero,
        kRange
    };

                    ArxRleUiPrInt(LPCTSTR msg, LPCTSTR keyWordList, IntType type);
    virtual         ~ArxRleUiPrInt();

    virtual Status  go();             // issue the prompt
    int             value();

    virtual void    setRange(int minVal, int maxVal);
    virtual void    setType(IntType type);

protected:
        // data members
    IntType        m_intType;
    int            m_value;
    int            m_minVal;
    int            m_maxVal;

        // helper functions
    bool		   inRange();

private:
        // outlawed functions
					ArxRleUiPrInt(const ArxRleUiPrInt&);
    ArxRleUiPrInt&	operator=(const ArxRleUiPrInt&);
};


#endif  // ARXRLEUIPRINT_H
