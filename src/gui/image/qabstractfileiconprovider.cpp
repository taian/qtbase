/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qabstractfileiconprovider.h"

#include <qguiapplication.h>
#include <private/qabstractfileiconprovider_p.h>
#include <private/qfilesystementry_p.h>

QT_BEGIN_NAMESPACE

QAbstractFileIconProviderPrivate::QAbstractFileIconProviderPrivate(QAbstractFileIconProvider *q)
    : q_ptr(q)
{}

QAbstractFileIconProviderPrivate::~QAbstractFileIconProviderPrivate() = default;

/*!
  \class QAbstractFileIconProvider

  \inmodule QtGui
  \since 6.0

  \brief The QAbstractFileIconProvider class provides file icons for the QFileSystemModel class.
*/

/*!
  \enum QAbstractFileIconProvider::IconType

  \value Computer
  \value Desktop
  \value Trashcan
  \value Network
  \value Drive
  \value Folder
  \value File
*/

/*!
    \enum QAbstractFileIconProvider::Option

    \value DontUseCustomDirectoryIcons Always use the default directory icon.
    Some platforms allow the user to set a different icon. Custom icon lookup
    cause a big performance impact over network or removable drives.
*/

/*!
  Constructs a file icon provider.
*/
QAbstractFileIconProvider::QAbstractFileIconProvider()
    : d_ptr(new QAbstractFileIconProviderPrivate(this))
{
}

/*!
  \internal
*/
QAbstractFileIconProvider::QAbstractFileIconProvider(QAbstractFileIconProviderPrivate &dd)
    : d_ptr(&dd)
{}

/*!
  Destroys the file icon provider.
*/

QAbstractFileIconProvider::~QAbstractFileIconProvider() = default;


/*!
    Sets \a options that affect the icon provider.
    \sa options()
*/

void QAbstractFileIconProvider::setOptions(QAbstractFileIconProvider::Options options)
{
    Q_D(QAbstractFileIconProvider);
    d->options = options;
}

/*!
    Returns all the options that affect the icon provider.
    By default, all options are disabled.
    \sa setOptions()
*/

QAbstractFileIconProvider::Options QAbstractFileIconProvider::options() const
{
    Q_D(const QAbstractFileIconProvider);
    return d->options;
}

/*!
  Returns an icon set for the given \a type.
*/

QIcon QAbstractFileIconProvider::icon(IconType type) const
{
    Q_UNUSED(type);
    return {};
}

/*!
  Returns an icon for the file described by \a info.
*/

QIcon QAbstractFileIconProvider::icon(const QFileInfo &info) const
{
    Q_UNUSED(info);
    return {};
}

/*!
  Returns the type of the file described by \a info.
*/

QString QAbstractFileIconProvider::type(const QFileInfo &info) const
{
    /* ### Qt 6 These string translations being in the QFileDialog context is not ideal,
       but translating them in QFileDialog context only in the QFileIconProvider subclass
       isn't either (it basically requires a duplication of the entire function).
       Either we change it to QAbstractFileIconProvider context (invalidates existing
       translations, but that's probably ok), or make a runtime lookup and use QFileDIalog
       context only if QGuiApplication is a QApplication.
    */
    if (QFileSystemEntry::isRootPath(info.absoluteFilePath()))
        return QGuiApplication::translate("QFileDialog", "Drive");
    if (info.isFile()) {
        // ### could use QMimeDatabase::mimeTypeForFile(const QFileInfo&) here
        if (!info.suffix().isEmpty()) {
            //: %1 is a file name suffix, for example txt
            return QGuiApplication::translate("QFileDialog", "%1 File").arg(info.suffix());
        }
        return QGuiApplication::translate("QFileDialog", "File");
    }

    if (info.isDir())
#ifdef Q_OS_WIN
        return QGuiApplication::translate("QFileDialog", "File Folder", "Match Windows Explorer");
#else
        return QGuiApplication::translate("QFileDialog", "Folder", "All other platforms");
#endif
    // Windows   - "File Folder"
    // macOS     - "Folder"
    // Konqueror - "Folder"
    // Nautilus  - "folder"

    if (info.isSymLink())
#ifdef Q_OS_MACOS
        return QGuiApplication::translate("QFileDialog", "Alias", "macOS Finder");
#else
        return QGuiApplication::translate("QFileDialog", "Shortcut", "All other platforms");
#endif
    // macOS     - "Alias"
    // Windows   - "Shortcut"
    // Konqueror - "Folder" or "TXT File" i.e. what it is pointing to
    // Nautilus  - "link to folder" or "link to object file", same as Konqueror

    return QGuiApplication::translate("QFileDialog", "Unknown");
}

QT_END_NAMESPACE