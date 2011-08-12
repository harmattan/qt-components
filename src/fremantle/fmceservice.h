#ifndef FMCESERVICE_H
#define FMCESERVICE_H

#include <QObject>
#include <QSet>
#include <QDBusServiceWatcher>
#include "fdbusproxy.h"

#define MCE_BUS QDBusConnection::systemBus()

class FMCEService : public FDBusProxy
{
    Q_OBJECT

public:
    //Singleton
    static FMCEService* instance();

public:
    Q_PROPERTY(bool ready READ isReady NOTIFY valueChanged())

Q_SIGNALS:
    void valueChanged();

public:
    virtual void start (QObject *requestor = 0);
    virtual void stop  (QObject *requestor = 0);

public:
    explicit FMCEService(const QString& path, QObject *parent = 0);

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

#endif /* ! FMCESERVICE_H */
