#ifndef FPHONESERVICE_H
#define FPHONESERVICE_H

#include <QObject>
#include <QSet>
#include <QDBusServiceWatcher>

#include "fservice.h"

#define PHONE_SERVICE_NAME   "com.nokia.phone"
#define PHONE_INTERFACE_NAME "com.nokia.phone.net"

#define PHONE_BUS QDBusConnection::systemBus()


class FPhoneService : public FService
{
public:
    static FService *instance()
    {
        static FPhoneService *self = 0;
        if (!self) {
            self =  new FPhoneService("/com/nokia/phone/net");
        }
        return qobject_cast<FService *>(self);
    }

    explicit FPhoneService(const QString& path, QObject *parent = 0, QDBusConnection bus=PHONE_BUS):
        FService(path, parent, bus)
    {
        serviceName   = PHONE_SERVICE_NAME;
        interfaceName = PHONE_INTERFACE_NAME;
    }

};

#endif /* ! FPHONESERVICE_H */
