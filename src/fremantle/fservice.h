#ifndef FSERVICE_H
#define FSERVICE_H

#include <QObject>
#include <QSet>
#include <QDBusServiceWatcher>

#include "fdbusproxy.h"

#define SERVICE_BUS  QDBusConnection::systemBus()

class FService : public FDBusProxy
{
    Q_OBJECT

public:
    //Singleton
    static FService* instance();

public:
    Q_PROPERTY(bool ready READ isReady NOTIFY valueChanged())

Q_SIGNALS:
    void valueChanged();

public:
    virtual void start (QObject *requestor = 0);
    virtual void stop  (QObject *requestor = 0);

public:
    explicit FService(const QString& path, QObject *parent = 0, QDBusConnection bus =  SERVICE_BUS);

public:
    bool isReady() const;

private:
    QDBusConnection service_bus;

    bool ready;
    QDBusServiceWatcher *watcher;
    QSet<QObject *> subscribers;

private Q_SLOTS:
    void isDown();
    void isUp();
};

#endif /* ! FSERVICE_H */
