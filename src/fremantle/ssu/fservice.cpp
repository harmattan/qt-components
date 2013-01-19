
#include <QDBusServiceWatcher>
#include <fmceservice.h>

#include "fservice.h"

FService::FService(const QString& path, QObject *parent, QDBusConnection bus):
    FDBusProxy(path, parent),
    service_bus(bus),
    watcher(0)
{
}

void FService::start(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (!started) {
        ready = started = true;

        watcher = new QDBusServiceWatcher(serviceName, service_bus);
        QObject::connect(watcher, SIGNAL(serviceRegistered(const QString&)),
                         this, SLOT(isUp()), Qt::QueuedConnection);
        QObject::connect(watcher, SIGNAL(serviceUnregistered(const QString&)),
                         this, SLOT(isDown()));

    }
    subscribers.insert(requestor);
}

void FService::stop(QObject *requestor)
{
    subscribers.remove(requestor);
    if (started && subscribers.size() == 0) {
        ready = started = false;

        delete watcher;
        watcher = 0;
    }
}

void FService::isUp()
{
    ready = true;
    Q_EMIT valueChanged();
}

void FService::isDown()
{
    ready = false;
    Q_EMIT valueChanged();
}

bool FService::isReady() const
{
    return ready;
}


#include "moc_fservice.cpp"
