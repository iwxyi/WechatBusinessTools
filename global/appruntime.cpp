#include "appruntime.h"

AppRuntime *ar = new AppRuntime();

AppRuntime::AppRuntime(QObject *parent) : QObject(parent)
{

}

void AppRuntime::setDeanService(qint64 deanService)
{
    this->_deanService = deanService;
}

qint64 AppRuntime::deanService()
{
    return this->_deanService;
}
