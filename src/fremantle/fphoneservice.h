#ifndef FPHONESERVICE_H
#define FPHONESERVICE_H

#include <QObject>
#include <QSet>
#include <QDBusServiceWatcher>

#include "fservice.h"

#define PHONE_SERVICE_NAME   "com.nokia.phone"
#define PHONE_INTERFACE_NAME "com.nokia.phone.request"

#define PHONE_BUS QDBusConnection::systemBus()


class FPhoneService : public FService
{
public:
    FService* instance()
    {
        static FPhoneService *self = 0;
        if (!self) {
            self =  new FPhoneService("/com/nokia/phone/request");
        }
        return qobject_cast<FService *>(self);
    }

    explicit FPhoneService(const QString& path, QObject *parent = 0):
        FService(path, parent),
        watcher(0)
    {
        serviceName   = PHONE_SERVICE_NAME;
        interfaceName = PHONE_INTERFACE_NAME;
        service_bus   = PHONE_BUS;
    }

};

#endif /* ! FPHONESERVICE_H */
