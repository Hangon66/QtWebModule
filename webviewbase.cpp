#include "webviewbase.h"
#include "ui_webviewbase.h"
#include <QVBoxLayout>
#include <QUrl>
#include <QDebug>
#include <QWebEngineSettings>
#include <QShowEvent>
#include <QHideEvent>
#include <QResizeEvent>
#include <QMoveEvent>

/**
 * @brief 构造 WebViewBase 占位控件，QWebEngineView 放入独立顶层窗口以避免 D3D11 合成冲突。
 *
 * 原理：QWebEngineView 嵌入 widget 树会强制顶层窗口使用 D3D11 合成，
 * 导致 VTK 的 QOpenGLWidget 报错黑屏。将 QWebEngineView 移到独立窗口后，
 * 主窗口不再触发 D3D11 合成，VTK 可正常工作。
 * 独立窗口通过定时器跟踪占位控件的全局坐标，实现视觉上的嵌入效果。
 *
 * @param parent 父控件（占位用途，WebViewBase 自身嵌入 widget 树中）。
 */
WebViewBase::WebViewBase(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WebViewBase)
{
    ui->setupUi(this);

    // 创建独立顶层窗口，承载 QWebEngineView
    // Qt::Window 使其成为独立顶层窗口，不参与主窗口的 widget 合成
    // Qt::FramelessWindowHint 去掉标题栏，视觉上与嵌入一致
    m_webWindow = new QWidget(nullptr, Qt::Window | Qt::FramelessWindowHint);

    m_webView = new QWebEngineView(m_webWindow);

    // 使用自定义 Page 来处理新窗口请求
    // 注意：m_page 不能以 m_webView 为 parent，否则析构时会被 m_webView 再次 delete 导致双重释放
    // 使用 nullptr parent，由析构函数手动管理生命周期
    m_page = new WebPageBase(nullptr);
    m_webView->setPage(m_page);

    QWebEngineSettings *settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false);

    QVBoxLayout *layout = new QVBoxLayout(m_webWindow);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_webView);

    // 位置同步定时器：跟踪父窗口移动、QStackedWidget 页面切换等
    // moveEvent 只能捕获自身移动，无法捕获父窗口移动，因此需要定时器轮询
    m_syncTimer = new QTimer(this);
    m_syncTimer->setInterval(50); // 50ms = 20fps，跟踪流畅且开销极小
    connect(m_syncTimer, &QTimer::timeout, this, &WebViewBase::syncWebWindow);
    m_syncTimer->start();
}

WebViewBase::~WebViewBase()
{
    qDebug() << "[WebViewBase::~WebViewBase] this=" << this
             << " parent=" << parent()
             << " m_page=" << m_page
             << " m_webView=" << m_webView
             << " m_webWindow=" << m_webWindow;

    // 析构顺序关键：
    // 1. 先删 m_page，避免 Qt 为 m_webView 创建默认 QWebEnginePage
    if (m_page) {
        qDebug() << "[WebViewBase::~WebViewBase] deleting m_page" << m_page;
        delete m_page;
        m_page = nullptr;
    }
    // 2. m_webView 是 m_webWindow 的子对象，delete m_webWindow 会级联删除
    //    但为确保安全（避免 Qt 在 m_webWindow 析构期间创建默认 page），先显式删除
    if (m_webView) {
        qDebug() << "[WebViewBase::~WebViewBase] deleting m_webView" << m_webView;
        delete m_webView;
        m_webView = nullptr;
    }
    // 3. 删除独立窗口容器
    if (m_webWindow) {
        qDebug() << "[WebViewBase::~WebViewBase] deleting m_webWindow" << m_webWindow;
        delete m_webWindow;
        m_webWindow = nullptr;
    }
    qDebug() << "[WebViewBase::~WebViewBase] done, deleting ui";
    delete ui;
}

void WebViewBase::loadUrl(const QUrl &url)
{
    qDebug() << "loadUrl:" << url;
    m_webView->load(url);
}

void WebViewBase::runJavaScript(const QString &scriptSource,
                                const std::function<void(const QVariant &)> &callback)
{
    if (m_page) {
        m_page->runJavaScript(scriptSource, callback);
    }
}

void WebViewBase::setDefaultFilePath(const QString &path)
{
    if (m_page) {
        m_page->setDefaultFilePath(path);
    }
}

/**
 * @brief 同步独立窗口的位置和大小到占位控件的全局坐标。
 *
 * 每 50ms 由定时器调用一次。检查占位控件在整个父级链中是否可见，
 * 可见时将独立窗口对齐到占位控件的全局坐标；不可见时隐藏独立窗口。
 * 这处理了 QStackedWidget 页面切换、父窗口移动/最小化等场景。
 */
void WebViewBase::syncWebWindow()
{
    if (!m_webWindow || !m_webView)
        return;

    // 检查占位控件在整个父级链中是否可见
    // QStackedWidget 切换页面时，隐藏的页面 isVisible()=true 但 isVisibleTo(parent)=false
    bool visibleToParent = true;
    QWidget *w = this;
    while (w) {
        QWidget *p = qobject_cast<QWidget *>(w->parentWidget());
        if (p && !w->isVisibleTo(p)) {
            visibleToParent = false;
            break;
        }
        w = p;
    }

    // 还需检查顶层窗口是否最小化
    QWidget *topLevel = window();
    if (topLevel && topLevel->isMinimized()) {
        visibleToParent = false;
    }

    if (isVisible() && visibleToParent) {
        QRect globalRect(mapToGlobal(QPoint(0, 0)), size());
        if (m_webWindow->geometry() != globalRect) {
            m_webWindow->setGeometry(globalRect);
        }
        if (!m_webWindow->isVisible()) {
            m_webWindow->show();
        }
        // 确保独立窗口在主窗口之上（处理 z-order 变化）
        if (topLevel && topLevel->isActiveWindow()) {
            m_webWindow->raise();
        }
    } else {
        if (m_webWindow->isVisible()) {
            m_webWindow->hide();
        }
    }
}

void WebViewBase::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    syncWebWindow();
}

void WebViewBase::hideEvent(QHideEvent *event)
{
    if (m_webWindow)
        m_webWindow->hide();
    QWidget::hideEvent(event);
}

void WebViewBase::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    syncWebWindow();
}

void WebViewBase::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    syncWebWindow();
}
