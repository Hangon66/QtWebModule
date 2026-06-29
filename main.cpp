#include "webviewbase.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WebViewBase w;
    w.show();
    w.loadUrl(QUrl("http://192.168.10.4:30031"));
    return a.exec();
}
