
#include <QDBusServiceWatcher>
#include <QDBusMetaType>
#include <fhalservice.h>

#define HAL_SERVICE_NAME "org.freedesktop.Hal"

#include <QtDebug>

static FHALService *self = 0;

FHALService * FHALService::instance()
{
    if (!self) {
        self =  new FHALService("/");
    }
    return self;
}

FHALService::FHALService(const QString& path, QObject *parent):
    FDBusProxy(path, parent),
    watcher(0)
{
    static bool property_registered = false;
    serviceName = HAL_SERVICE_NAME;
 
   if (!property_registered) {
      qDBusRegisterMetaType< FHALProperty >();
      qDBusRegisterMetaType< QList<FHALProperty> >();
      property_registered = true;
    }
}

void FHALService::start(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (!started) {
        ready = started = true;

        watcher = new QDBusServiceWatcher(serviceName, HAL_BUS);
        QObject::connect(watcher, SIGNAL(serviceRegistered(const QString&)),
                         this, SLOT(isUp()), Qt::QueuedConnection);
        QObject::connect(watcher, SIGNAL(serviceUnregistered(const QString&)),
                         this, SLOT(isDown()));

    }
    subscribers.insert(requestor);
}

void FHALService::stop(QObject *requestor)
{
    subscribers.remove(requestor);
    if (started && subscribers.size() == 0) {
        ready = started = false;

        delete watcher;
        watcher = 0;
    }
}

void FHALService::isUp()
{
    ready = true;
    Q_EMIT valueChanged();
}

void FHALService::isDown()
{
    ready = false;
    Q_EMIT valueChanged();
}

bool FHALService::isReady() const
{
    return ready;
}

const QDBusArgument & operator<<(QDBusArgument &arg, const FHALProperty &change)
{
    arg.beginStructure();
    arg << change.name << change.added << change.removed;
    arg.endStructure();
    return arg;
}

const QDBusArgument & operator>>(const QDBusArgument &arg, FHALProperty &change)
{
    arg.beginStructure();
    arg >> change.name >> change.added >> change.removed;
    arg.endStructure();
    return arg;
}

#include "moc_fhalservice.cpp"
