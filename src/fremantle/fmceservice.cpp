
#include <QDBusServiceWatcher>
#include <fmceservice.h>

#define MCE_SERVICE_NAME   "com.nokia.mce"
#define MCE_INTERFACE_NAME "com.nokia.mce.request"

#include <QtDebug>

static FMCEService *self = 0;

FMCEService * FMCEService::instance()
{
    if (!self) {
        self =  new FMCEService("/com/nokia/mce/request");
    }
    return self;
}

FMCEService::FMCEService(const QString& path, QObject *parent):
    FDBusProxy(path, parent),
    watcher(0)
{
    serviceName   = MCE_SERVICE_NAME;
    interfaceName = MCE_INTERFACE_NAME;
}

void FMCEService::start(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (!started) {
        ready = started = true;

        watcher = new QDBusServiceWatcher(serviceName, MCE_BUS);
        QObject::connect(watcher, SIGNAL(serviceRegistered(const QString&)),
                         this, SLOT(isUp()), Qt::QueuedConnection);
        QObject::connect(watcher, SIGNAL(serviceUnregistered(const QString&)),
                         this, SLOT(isDown()));

    }
    subscribers.insert(requestor);
}

void FMCEService::stop(QObject *requestor)
{
    subscribers.remove(requestor);
    if (started && subscribers.size() == 0) {
        ready = started = false;

        delete watcher;
        watcher = 0;
    }
}

void FMCEService::isUp()
{
    ready = true;
    Q_EMIT valueChanged();
}

void FMCEService::isDown()
{
    ready = false;
    Q_EMIT valueChanged();
}

bool FMCEService::isReady() const
{
    return ready;
}


#include "moc_fmceservice.cpp"
