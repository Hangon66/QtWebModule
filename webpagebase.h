#ifndef WEBPAGEBASE_H
#define WEBPAGEBASE_H

#include <QObject>
#include <QWebEnginePage>

class WebPageBase : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit WebPageBase(QObject *parent = nullptr);

    /**
     * @brief 设置 WebView 文件选择对话框的默认路径。
     *
     * 当 HTML 页面中的 <input type="file"> 触发文件选择时，
     * chooseFiles 将以此路径作为起始目录。
     *
     * @param path 默认目录的绝对路径，为空则使用系统默认路径。
     */
    void setDefaultFilePath(const QString &path);

    /**
     * @brief 设置是否将 JS 控制台输出转发到 qDebug。
     * @param enabled true 启用日志转发，false 禁用。默认禁用。
     */
    void setJavaScriptConsoleLoggingEnabled(bool enabled);

    /**
     * @brief 查询 JS 控制台日志转发是否启用。
     */
    bool isJavaScriptConsoleLoggingEnabled() const;

protected:
    QWebEnginePage* createWindow(QWebEnginePage::WebWindowType type) override;

    /**
     * @brief 捕获 JS 控制台输出，转发到 qDebug 方便调试。
     */
    void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                                  const QString &message,
                                  int lineNumber,
                                  const QString &sourceID) override;

    /**
     * @brief 重写文件选择逻辑，支持自定义默认目录。
     *
     * 当 HTML 页面请求文件选择时调用。
     * 使用 m_defaultFilePath 作为 QFileDialog 的起始目录。
     *
     * @param mode 文件选择模式（单文件/多文件）。
     * @param oldFiles 之前已选文件列表。
     * @param acceptedMimeTypes HTML accept 属性指定的 MIME 类型。
     * @return 用户选中的文件路径列表，取消时返回空列表。
     */
    QStringList chooseFiles(QWebEnginePage::FileSelectionMode mode,
                            const QStringList &oldFiles,
                            const QStringList &acceptedMimeTypes) override;

private:
    /**
     * @brief 文件选择对话框的默认起始路径。
     *
     * 由 setDefaultFilePath() 设置，为空时使用系统默认路径。
     */
    QString m_defaultFilePath;
    bool m_jsConsoleLogging = false; ///< JS 控制台日志转发开关，默认关闭
};

#endif // WEBPAGEBASE_H
