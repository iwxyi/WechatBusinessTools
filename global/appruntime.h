#ifndef APPRUNTIME_H
#define APPRUNTIME_H

#include <QObject>

class AppRuntime : public QObject
{
    Q_OBJECT
public:
    AppRuntime(QObject *parent = nullptr);

    void setDeanService(qint64 deanService);
    qint64 deanService();

private:
    qint64 _deanService = 0;
};

extern AppRuntime *ar;

#endif // APPRUNTIME_H
