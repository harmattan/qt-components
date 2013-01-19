#ifndef FMCESERVICE_H
#define FMCESERVICE_H

#include <QObject>
#include <QSet>
#include <QDBusServiceWatcher>

#include "fservice.h"

#define MCE_SERVICE_NAME   "com.nokia.mce"
#define MCE_INTERFACE_NAME "com.nokia.mce.request"

#define MCE_BUS QDBusConnection::systemBus()


class FMCEService : public FService
{
public:
    static FService *instance()
    {
        static FMCEService *self = 0;
        if (!self) {
            self =  new FMCEService("/com/nokia/mce/request");
        }
        return qobject_cast<FService *>(self);
    }

    explicit FMCEService(const QString& path, QObject *parent = 0, QDBusConnection bus=MCE_BUS):
        FService(path, parent, bus)
    {
        serviceName   = MCE_SERVICE_NAME;
        interfaceName = MCE_INTERFACE_NAME;
    }

};

#endif /* ! FMCESERVICE_H */
