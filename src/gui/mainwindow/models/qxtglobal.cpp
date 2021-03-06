/****************************************************************************
**
** Copyright (C) Qxt Foundation. Some rights reserved.
**
** This file is part of the QxtCore module of the Qxt library.
**
** This library is free software; you can redistribute it and/or modify it
** under the terms of the Common Public License, version 1.0, as published by
** IBM.
**
** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
** FITNESS FOR A PARTICULAR PURPOSE.
**
** You should have received a copy of the CPL along with this file.
** See the LICENSE file and the cpl1.0.txt file included with the source
** distribution for more information. If you did not receive a copy of the
** license, contact the Qxt Foundation.
**
** <http://libqxt.org>  <foundation@libqxt.org>
**
****************************************************************************/

#include "qxtglobal.h"

/*! \mainpage Qt extension library
 *
 * Qxt provides classes for extended needs based on the Qt Library by Trolltech
 *
 * See the list of classes.
 *
 * \image html logo.png
 *
 */

/*!
    \macro QXT_VERSION
    \relates <QxtGlobal>

    This macro expands a numeric value of the form 0xMMNNPP (MM =
    major, NN = minor, PP = patch) that specifies Qxt's version
    number. For example, if you compile your application against Qxt
    0.4.0, the QXT_VERSION macro will expand to 0x000400.

    You can use QXT_VERSION to use the latest Qt features where
    available. For example:
    \code
    #if QXT_VERSION >= 0x000400
        qxtTabWidget->setTabMovementMode(QxtTabWidget::InPlaceMovement);
    #endif
    \endcode

    \sa QXT_VERSION_STR, qxtVersion()
 */

/*!
    \macro QXT_VERSION_STR
    \relates <QxtGlobal>

    This macro expands to a string that specifies Qxt's version number
    (for example, "0.4.0"). This is the version against which the
    application is compiled.

    \sa qxtVersion(), QXT_VERSION
 */

/*!
    \relates <QxtGlobal>

    Returns the version number of Qxt at run-time as a string (for
    example, "0.4.0"). This may be a different version than the
    version the application was compiled against.

    \sa QXT_VERSION_STR
 */

const char* qxtVersion()
{
    return QXT_VERSION_STR;
}
