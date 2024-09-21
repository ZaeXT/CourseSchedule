#include "wintoastlib.h"
#include <cstdlib>
#include <ctime>
#include <QApplication>
#include <QFont>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QRect>
#include <QScreen>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>
#include <string>
#include <windows.h>

// 自定义事件处理器类，用于处理 Toast 通知的点击和关闭事件
class CustomToastHandler : public WinToastLib::IWinToastHandler {
public:
    void toastActivated() const override {
        std::wcout << L"Toast clicked!" << '\n';
    }

    void toastActivated(int actionIndex) const override {
        std::wcout << L"Toast clicked with action index: " << actionIndex << '\n';
    }

    void toastDismissed(WinToastDismissalReason state) const override {
        if (state == WinToastDismissalReason::UserCanceled) {
            std::wcout << L"Toast dismissed by user" << '\n';
        }
        else if (state == WinToastDismissalReason::TimedOut) {
            std::wcout << L"Toast timed out" << '\n';
        }
        else {
            std::wcout << L"Toast dismissed for other reasons" << '\n';
        }
    }

    void toastFailed() const override {
        std::wcout << L"Error showing toast" << '\n';
    }
};

// 透明窗口类，显示倒计时
class TransparentWindow : public QWidget {
public:
    TransparentWindow(QWidget* parent = nullptr) : QWidget(parent) {
        // 设置无边框窗口&置顶
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

        // 设置窗口背景透明
        setAttribute(Qt::WA_TranslucentBackground);

        // 禁止系统绘制背景，防止白边
        setAttribute(Qt::WA_NoSystemBackground);

        // 设置窗口大小
        resize(100, 400);

        // 移动窗口至屏幕右侧
        moveWindowToRightEdge();

        // 设置点击穿透
        setClickThrough();

        // 下课计时
        breakTimer = new QTimer(this);
        connect(breakTimer, &QTimer::timeout, this, &TransparentWindow::updateCountdown);
        breakTimer->start(1000);  // 每秒更新一次倒计时

        // R!G!B!
        RGBtimer = new QTimer(this);
        connect(RGBtimer, &QTimer::timeout, this, &TransparentWindow::updatePaint);
        RGBtimer->start(100);

        // 托盘图标
        trayIcon = new QSystemTrayIcon(this);

        // 托盘菜单
    	trayMenu = new QMenu(this);

        // 托盘选项
        trayConfig = trayMenu->addAction("配置");
        trayExit = trayMenu->addAction("退出");

        // 关联托盘菜单
        trayIcon->setContextMenu(trayMenu);

        // 设置托盘图标
        trayIcon->setIcon(QIcon::fromTheme("preferences-system"));

        // 显示托盘
        trayIcon->setVisible(true);
        trayIcon->show();

    	// 初始化 WinToast
        initializeWinToast();

        // 发送初始通知
        sendInitialNotification(L"下课提醒", L"已下课！");
    }

    // 初始化 WinToast
    void initializeWinToast() {
        if (!WinToastLib::WinToast::isCompatible()) {
            std::wcerr << L"Error: This system is not compatible with Toast notifications!" << '\n';
            exit(1);
        }

        WinToastLib::WinToast::instance()->setAppName(L"课程提醒应用");
        const auto aumi = WinToastLib::WinToast::configureAUMI(L"ZaeXT", L"ScheduleReminder", L"NotifyToast", L"NSCDV");
        WinToastLib::WinToast::instance()->setAppUserModelId(aumi);

        if (!WinToastLib::WinToast::instance()->initialize()) {
            std::wcerr << L"Error: Unable to initialize WinToast." << '\n';
            exit(1);
        }
    }

    // 使用 WinToast 发送初始通知
    void sendInitialNotification(const wchar_t* title, const wchar_t* message) {
        WinToastLib::WinToastTemplate toastTemplate(WinToastLib::WinToastTemplate::Text02);
        toastTemplate.setTextField(title, WinToastLib::WinToastTemplate::FirstLine);
        toastTemplate.setTextField(message, WinToastLib::WinToastTemplate::SecondLine);
        toastTemplate.setAudioOption(WinToastLib::WinToastTemplate::AudioOption::Default);

        CustomToastHandler* handler = new CustomToastHandler();

        if (WinToastLib::WinToast::instance()->showToast(toastTemplate, handler) == -1L) {
            std::wcerr << L"Error: Unable to show initial toast notification." << '\n';
        }
    }

    // 更新倒计时
    void updateCountdown() {
        // 每次定时器触发，减少倒计时
        defaultCountdownTime--;

        // 检查倒计时是否结束
        if (defaultCountdownTime >= 0) {
            update();  // 更新窗口显示的内容
        }
        else {
            breakTimer->stop();
            sendInitialNotification(L"倒计时通知", L"倒计时已结束！");
        }
    }

    // 将窗口移动到屏幕右边缘
    void moveWindowToRightEdge() {
        QScreen* screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect screenGeometry = screen->geometry();
            int screenWidth = screenGeometry.width();
            int screenHeight = screenGeometry.height();

            int windowX = screenWidth - width();
            int windowY = (screenHeight - height()) / 2;

            move(windowX, windowY);
        }
    }

	// 使用 Windows API 设置点击穿透
    void setClickThrough() {
        HWND hwnd = (HWND)winId();  // 获取窗口句柄
        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        // 设置 WS_EX_LAYERED 和 WS_EX_TRANSPARENT 样式，实现点击穿透
        SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    }
protected:
    // 绘制倒计时内容
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);

        // 确保背景完全透明
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::NoPen);

        // 设置字体
        QFont font("Arial", 24, QFont::Bold);
        painter.setFont(font);

        // 计算每行文本的高度
        int lineHeight = font.pointSize() * 1.5;  // 调整比例以适合你的需求
        int x = rect().center().x();
        int y = rect().center().y() - 2 * lineHeight;

        // 绘制文本
        painter.setPen(QColor(255, 0, 0));  // 红色文字
        painter.drawText(x, y, "课");

        painter.setPen(QColor(0, 255, 0));  // 绿色文字
        painter.drawText(x, y + lineHeight, "程");

        painter.setPen(QColor(0, 0, 255));  // 蓝色文字
        painter.drawText(x, y + 2 * lineHeight, "表");

        painter.setPen(currentColor);  // 黄色文字
        painter.drawText(x, y + 3 * lineHeight, QString::number(defaultCountdownTime));

        painter.setPen(QColor(255, 0, 255));  // 品红色文字
        painter.drawText(x, y + 4 * lineHeight, "功");

        painter.setPen(QColor(0, 255, 255));  // 青色文字
        painter.drawText(x, y + 5 * lineHeight, "能");

        painter.setPen(QColor(128, 128, 128));  // 灰色文字
        painter.drawText(x, y + 6 * lineHeight, "测");

        painter.setPen(QColor(255, 165, 0));  // 橙色文字
        painter.drawText(x, y + 7 * lineHeight, "试");
    }

private slots:
	void updatePaint() {
    	// 动态修改文本和颜色
    	//currentText = QString("Updated Text %1").arg(QTime::currentTime().toString("HH:mm:ss"));
    	currentColor = QColor(rand() % 256, rand() % 256, rand() % 256);  // 随机颜色

    	// 触发重新绘制
    	update();
        }

private:
    int defaultCountdownTime = 60;  // 倒计时60秒
    QTimer* breakTimer,* RGBtimer;       // 下课计时通知
    QString currentText;
    QColor currentColor;
    QSystemTrayIcon* trayIcon;      // 托盘图标
    QMenu* trayMenu;                // 托盘菜单
    QAction* trayConfig,*trayExit;  // 托盘选项
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    std::srand(static_cast<unsigned>(std::time(0)));
    TransparentWindow window;
    window.show();
    

    return app.exec();
}
