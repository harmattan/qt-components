/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "asyncdbusinterface.h"
#include <QObject>
#include <QDBusConnection>
#include <QString>

/*!
  \class AsyncDBusInterface

  \brief This is a workaround for QtDBus to be asynchronous.

  In QtDBus, the QDBusInterface class which the developer is supposed
  to use makes introspection calls synchronously.  It is not clear
  (and not documented), what is the benefit of these introspection
  calls, probably invalid use and some errors are catched earlier if
  introspection data is available.

  However this introspection data is requested synchronously, so
  constructing a new QDBusInterface can block your thread if the
  endpoint serving that introspection data on D-Bus is stuck.

  ContextKit tries to be asynchronous, so this behavior of
  QDBusInterface is unacceptable and this is why
  QDBusAbstractInterface has to be used instead (it is simpler, it
  doesn't use introspection at all).

  Unfortunately QDBusAbstractInterface only has a protected
  constructor, so we need this wrapper class, which makes that
  constructor available to us.

  So this class is internal to ContextKit, it is distributed only,
  because we use it in several subprojects, do not use it through
  ContextKit.  If you find this useful, copy the source instead.
*/
AsyncDBusInterface::AsyncDBusInterface(const QString &service, const QString &path,
                                       const QString &interface,
                                       const QDBusConnection &connection,
                                       QObject *parent)
    : QDBusAbstractInterface(service, path, interface.toLatin1().data(), connection, parent)
{
}
