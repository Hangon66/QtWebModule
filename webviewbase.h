#ifndef WEBVIEWBASE_H
#define WEBVIEWBASE_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QTimer>
#include <functional>
#include <QVariant>
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

    /**
     * @brief 在当前页面中执行 JavaScript 代码。
     * @param scriptSource 要执行的 JavaScript 源代码。
     * @param callback 可选回调，接收脚本执行结果（QVariant）。
     */
    void runJavaScript(const QString &scriptSource,
                       const std::function<void(const QVariant &)> &callback = {});

    /**
     * @brief 设置 WebView 内 HTML 文件选择对话框的默认路径。
     * @param path 默认目录的绝对路径。
     */
    void setDefaultFilePath(const QString &path);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;

private:
    /**
     * @brief 同步独立窗口的位置和大小到占位控件的全局坐标。
     */
    void syncWebWindow();

    Ui::WebViewBase *ui;
    QWebEngineView *m_webView = nullptr; // 成员变量存储 Web 视图
    WebPageBase *m_page = nullptr;

    /**
     * @brief WebEngine 独立顶层窗口，与主窗口分离以避免 D3D11 合成冲突。
     *
     * 将 QWebEngineView 放入独立窗口后，主窗口不再包含 WebEngine 控件，
     * Qt 不会强制顶层窗口使用 D3D11 合成，VTK 的 QOpenGLWidget 可正常工作。
     */
    QWidget *m_webWindow = nullptr;

    /**
     * @brief 位置同步定时器，跟踪父窗口移动（moveEvent 无法捕获父窗口移动）。
     */
    QTimer *m_syncTimer = nullptr;
};
#endif // WEBVIEWBASE_H
