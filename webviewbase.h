#ifndef WEBVIEWBASE_H
#define WEBVIEWBASE_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebEnginePage>
#include "webpagebase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class WebViewBase;
}
QT_END_NAMESPACE


class WebViewBase : public QWidget
{
    Q_OBJECT

public:
    WebViewBase(QWidget *parent = nullptr);
    ~WebViewBase();
    void loadUrl(const QUrl &url);
private:
    Ui::WebViewBase *ui;
    QWebEngineView *m_webView = nullptr; // 成员变量存储 Web 视图
    WebPageBase *m_page = nullptr;
};
#endif // WEBVIEWBASE_H
