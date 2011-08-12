
#include <QDBusServiceWatcher>
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
    serviceName   = HAL_SERVICE_NAME;
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


#include "moc_fhalservice.cpp"
