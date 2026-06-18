#ifndef WEBPAGEBASE_H
#define WEBPAGEBASE_H

#include <QObject>
#include <QWebEnginePage>

class WebPageBase : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit WebPageBase(QObject *parent = nullptr);
protected:
    QWebEnginePage* createWindow(QWebEnginePage::WebWindowType type) override;
};

#endif // WEBPAGEBASE_H
