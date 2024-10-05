#ifndef DEANSERVICE_H
#define DEANSERVICE_H

#include <QObject>
#include <QWebSocket>

class DeanService : public QObject
{
    Q_OBJECT
public:
    explicit DeanService(QObject *parent = nullptr);

    void init();
    void start();

signals:

public slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);

private:
    QWebSocket *deanWs = nullptr;
};

#endif // DEANSERVICE_H