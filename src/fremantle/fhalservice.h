#ifndef FHALSERVICE_H
#define FHALSERVICE_H

#include <QObject>
#include <QSet>
#include <QDBusServiceWatcher>

#include "fdbusproxy.h"

#define HAL_BUS QDBusConnection::systemBus()

class FHALService : public FDBusProxy
{
    Q_OBJECT

public:
    //Singleton
    static FHALService* instance();

public:
    Q_PROPERTY(bool ready READ isReady NOTIFY valueChanged())

Q_SIGNALS:
    void valueChanged();

public:
    virtual void start (QObject *requestor = 0);
    virtual void stop  (QObject *requestor = 0);

public:
    explicit FHALService(const QString& path, QObject *parent = 0);

public:
    bool isReady() const;

private:
    bool ready;
    QDBusServiceWatcher *watcher;
    QSet<QObject *> subscribers;

private Q_SLOTS:
    void isDown();
    void isUp();
};

#endif /* ! FHALSERVICE_H */
