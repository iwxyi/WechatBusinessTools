#include "mainwindow.h"
#include "usettings.h"
#include "signaltransfer.h"
#include <QApplication>

SignalTransfer* st = new SignalTransfer();

int main(int argc, char *argv[])
{
    QApplication::setDesktopSettingsAware(true); // 据说是避免不同分辨率导致显示的字体大小不一致
#if (QT_VERSION > QT_VERSION_CHECK(5,6,0))
        // 设置高分屏属性以便支持2K4K等高分辨率，尤其是手机app。
    // 必须写在main函数的QApplication a(argc, argv);的前面
    // 设置后，读取到的窗口会随着显示器倍数而缩小
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("LanYiXi");
    QCoreApplication::setOrganizationDomain("iwxyi.com");
    QCoreApplication::setApplicationName("微信运营工具");

    // Mac窗口
    qputenv("QT_MAC_WANTS_LAYER", "1");

    // 主窗口
    MainWindow w;
    w.show();
    return a.exec();
}
