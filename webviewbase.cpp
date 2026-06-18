#include "webviewbase.h"
#include "ui_webviewbase.h"
#include <QVBoxLayout>
#include <QUrl>
#include <QDebug>
#include <QWebEngineSettings>

WebViewBase::WebViewBase(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WebViewBase)
{
    ui->setupUi(this);
    m_webView = new QWebEngineView(this);
    
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

    QUrl url("http://baidu.com");   //默认加载的网页
    m_webView->load(url);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // 设置布局的边距为0
    layout->setSpacing(0);  // 设置布局的间距为0
    layout->addWidget(m_webView);
}

WebViewBase::~WebViewBase()
{
    qDebug() << "[WebViewBase::~WebViewBase] this=" << this
             << " parent=" << parent()
             << " m_page=" << m_page
             << " m_webView=" << m_webView;

    // 析构顺序关键：
    // 不能调用 setPage(nullptr)，因为 Qt 会立即为 m_webView 创建新的默认 QWebEnginePage
    // 该默认 page 使用全局 profile，程序退出时 profile 析构触发警告
    // 正确做法：直接 delete m_page，然后立即 delete m_webView（销毁 view 前不给它机会创建默认 page）
    if (m_page) {
        qDebug() << "[WebViewBase::~WebViewBase] deleting m_page" << m_page;
        delete m_page;
        m_page = nullptr;
    }
    if (m_webView) {
        qDebug() << "[WebViewBase::~WebViewBase] deleting m_webView" << m_webView;
        delete m_webView;
        m_webView = nullptr;
    }
    qDebug() << "[WebViewBase::~WebViewBase] done, deleting ui";
    delete ui;
}

void WebViewBase::loadUrl(const QUrl &url)
{
    qDebug() << "loadUrl:" << url;
    m_webView->load(url);
}
