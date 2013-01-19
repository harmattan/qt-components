#ifndef FHALSERVICE_H
#define FHALSERVICE_H

#include <QObject>
#include <QSet>
#include <QDBusServiceWatcher>
#include <QDBusMetaType>

#include "fservice.h"

#define HAL_SERVICE_NAME "org.freedesktop.Hal"
#define HAL_BUS QDBusConnection::systemBus()

struct FHALProperty;
class FHALService : public FService
{
public:
    static FService *instance()
    {
        static FHALService *self = 0;
        if (!self) {
            self =  new FHALService("/");
        }
        return qobject_cast<FService *>(self);
    }

    explicit FHALService(const QString& path, QObject *parent = 0, QDBusConnection bus=HAL_BUS):
        FService(path, parent, bus)
    {
      static bool property_registered = false;

      serviceName = HAL_SERVICE_NAME;
 
      if (!property_registered) {
	property_registered = true;
	qDBusRegisterMetaType< FHALProperty >();
	qDBusRegisterMetaType< QList<FHALProperty> >();
      }
    }
};

struct FHALProperty 
{
    FHALProperty(): name(""), added(false), removed(false) {}
    FHALProperty(QString name) : name(name), added(false), removed(false) {}

    QString name;
    bool    added;
    bool    removed;
};

inline const QDBusArgument & operator<<(QDBusArgument &arg, const FHALProperty &change)
{
    arg.beginStructure();
    arg << change.name << change.added << change.removed;
    arg.endStructure();
    return arg;
}

inline const QDBusArgument & operator>>(const QDBusArgument &arg, FHALProperty &change)
{
    arg.beginStructure();
    arg >> change.name >> change.added >> change.removed;
    arg.endStructure();
    return arg;
}

Q_DECLARE_METATYPE(FHALProperty)
Q_DECLARE_METATYPE(QList<FHALProperty>)

#endif /* ! FHALSERVICE_H */
