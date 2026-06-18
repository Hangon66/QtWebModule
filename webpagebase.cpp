#include "webpagebase.h"

WebPageBase::WebPageBase(QObject *parent)
    : QWebEnginePage{parent}
{

}

/**
 * @brief 重写createWindow方法，处理新窗口创建请求
 * 
 * @param type 窗口类型
 * @return QWebEnginePage* 
 * @note 默认返回当前页面，使用当前窗口加载新页面，防止新窗口创建
 */
QWebEnginePage *WebPageBase::createWindow(WebWindowType type)
{
    return this;
}
