#include "webpagebase.h"
#include <QFileDialog>
#include <QDebug>

WebPageBase::WebPageBase(QObject *parent)
    : QWebEnginePage{parent}
{

}

void WebPageBase::setDefaultFilePath(const QString &path)
{
    m_defaultFilePath = path;
}

void WebPageBase::setJavaScriptConsoleLoggingEnabled(bool enabled)
{
    m_jsConsoleLogging = enabled;
}

bool WebPageBase::isJavaScriptConsoleLoggingEnabled() const
{
    return m_jsConsoleLogging;
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

QStringList WebPageBase::chooseFiles(FileSelectionMode mode,
                                     const QStringList &oldFiles,
                                     const QStringList &acceptedMimeTypes)
{
    Q_UNUSED(oldFiles);

    QString startDir = m_defaultFilePath.isEmpty() ? QString() : m_defaultFilePath;

    // 将 MIME 类型转换为 QFileDialog 可用的文件过滤器
    QString filter;
    if (!acceptedMimeTypes.isEmpty()) {
        QStringList patterns;
        for (const QString &mime : acceptedMimeTypes) {
            // MIME 类型如 "text/csv"、".json"、".csv" 等
            if (mime.startsWith(".")) {
                patterns << "*" + mime;
            } else if (mime.contains("/")) {
                // 简单映射常见 MIME 类型
                if (mime == "text/csv")           patterns << "*.csv";
                else if (mime == "application/json") patterns << "*.json";
                else if (mime.startsWith("image/")) patterns << "*.png *.jpg *.jpeg *.gif *.bmp";
                else if (mime == "text/plain")     patterns << "*.txt";
                else if (mime == "text/html")      patterns << "*.html *.htm";
            }
        }
        if (!patterns.isEmpty()) {
            patterns.removeDuplicates();
            filter = tr("允许的文件 (%1)").arg(patterns.join(" "));
        }
    }

    if (mode == FileSelectOpenMultiple) {
        return QFileDialog::getOpenFileNames(nullptr, tr("选择文件"), startDir, filter);
    }
    // FileSelectOpen: 单文件选择
    QString file = QFileDialog::getOpenFileName(nullptr, tr("选择文件"), startDir, filter);
    if (file.isEmpty())
        return QStringList();
    return QStringList(file);
}

void WebPageBase::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
                                            const QString &message,
                                            int lineNumber,
                                            const QString &sourceID)
{
    if (!m_jsConsoleLogging)
        return;

    const char *levelStr = "";
    switch (level) {
    case InfoMessageLevel:    levelStr = "INFO"; break;
    case WarningMessageLevel: levelStr = "WARN"; break;
    case ErrorMessageLevel:   levelStr = "ERROR"; break;
    default:                  levelStr = "LOG"; break;
    }
    qDebug().noquote() << QString("[JS %1] %2 (line %3, %4)")
                              .arg(levelStr).arg(message).arg(lineNumber).arg(sourceID);
}
