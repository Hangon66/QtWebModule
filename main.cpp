#include "webviewbase.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WebViewBase w;
    w.show();
    return a.exec();
}
