#include "wintoastlib.h"
#include <cstdlib>
#include <ctime>
#include <QApplication>
#include <QFont>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QRect>
#include <QScreen>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>
#include <string>
#include <windows.h>
#include "ConfigData.h"
#include "ConfigDialog.h"

// 自定义事件处理器类，用于处理 Toast 通知的点击和关闭事件
class custom_toast_handler final : public WinToastLib::IWinToastHandler
{
public:
	void toastActivated() const override
	{
		std::wcout << L"Toast clicked!" << '\n';
	}

	void toastActivated(const int action_index) const override
	{
		std::wcout << L"Toast clicked with action index: " << action_index << '\n';
	}

	void toastDismissed(const WinToastDismissalReason state) const override
	{
		if (state == UserCanceled)
		{
			std::wcout << L"Toast dismissed by user" << '\n';
		}
		else if (state == TimedOut)
		{
			std::wcout << L"Toast timed out" << '\n';
		}
		else
		{
			std::wcout << L"Toast dismissed for other reasons" << '\n';
		}
	}

	void toastFailed() const override
	{
		std::wcout << L"Error showing toast" << '\n';
	}
};

// 透明窗口类，显示倒计时
class transparent_window final : public QWidget
{
public:
	explicit transparent_window(QWidget* parent = nullptr) : QWidget(parent)
	{
		// 设置无边框窗口&置顶
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

		// 设置窗口背景透明
		setAttribute(Qt::WA_TranslucentBackground);

		// 禁止系统绘制背景，防止白边
		setAttribute(Qt::WA_NoSystemBackground);

		// 设置窗口大小
		resize(100, 400);

		// 移动窗口至屏幕右侧
		move_window_to_right_edge();

		// 设置点击穿透
		set_click_through();

		// 下课计时
		break_timer_ = new QTimer(this);
		connect(break_timer_, &QTimer::timeout, this, &transparent_window::update_countdown);
		break_timer_->start(1000); // 每秒更新一次倒计时

		// R!G!B!
		rgb_timer_ = new QTimer(this);
		connect(rgb_timer_, &QTimer::timeout, this, &transparent_window::update_paint);
		rgb_timer_->start(100);

		// 托盘图标
		tray_icon_ = new QSystemTrayIcon(this);

		// 托盘菜单
		tray_menu_ = new QMenu(this);

		// 托盘选项
		tray_config_ = tray_menu_->addAction("配置");
		tray_menu_->addSeparator();
		tray_exit_ = tray_menu_->addAction("退出");

		// 关联托盘菜单
		tray_icon_->setContextMenu(tray_menu_);

		// 关联事件处理
		// 配置
		connect(tray_config_, &QAction::triggered, this, [this]()
		{
			const auto config = new config_dialog(this);
			config->setAttribute(Qt::WA_DeleteOnClose);
			config->show();
		});
		// 退出
		connect(tray_exit_, &QAction::triggered, this, [this]() { QApplication::quit(); });

		// 设置托盘图标
		tray_icon_->setIcon(QIcon(":/ico/resources/ico/tray-icon.png"));

		// 显示托盘
		tray_icon_->setVisible(true);
		tray_icon_->show();

		// 获取屏幕的尺寸和托盘图标的位置
		connect(tray_icon_, &QSystemTrayIcon::activated, [this](const QSystemTrayIcon::ActivationReason reason)
		{
			if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::Context)
			{
				const QRect trayIconGeometry = tray_icon_->geometry(); // 获取托盘图标的位置

				// 手动设置菜单显示的位置，让它出现在托盘图标上方
				QPoint menuPosition = trayIconGeometry.topLeft();
				menuPosition.setY(trayIconGeometry.top() - tray_menu_->sizeHint().height());

				// 使用 popup() 方法显示菜单
				tray_menu_->popup(menuPosition);
			}
		});

		// 初始化 WinToast
		initialize_win_toast();

		// 发送初始通知
		send_initial_notification(L"下课提醒", L"已下课！");
	}

	// 初始化 WinToast
	static void initialize_win_toast()
	{
		if (!WinToastLib::WinToast::isCompatible())
		{
			std::wcerr << L"Error: This system is not compatible with Toast notifications!" << '\n';
			exit(1);
		}

		WinToastLib::WinToast::instance()->setAppName(L"课程提醒应用");
		const auto aumi = WinToastLib::WinToast::configureAUMI(L"ZaeXT", L"ScheduleReminder", L"NotifyToast", L"NSCDV");
		WinToastLib::WinToast::instance()->setAppUserModelId(aumi);

		if (!WinToastLib::WinToast::instance()->initialize())
		{
			std::wcerr << L"Error: Unable to initialize WinToast." << '\n';
			exit(1);
		}
	}

	// 使用 WinToast 发送初始通知
	static void send_initial_notification(const wchar_t* title, const wchar_t* message)
	{
		WinToastLib::WinToastTemplate toastTemplate(WinToastLib::WinToastTemplate::Text02);
		toastTemplate.setTextField(title, WinToastLib::WinToastTemplate::FirstLine);
		toastTemplate.setTextField(message, WinToastLib::WinToastTemplate::SecondLine);
		toastTemplate.setAudioOption(WinToastLib::WinToastTemplate::AudioOption::Default);

		const auto handler = new custom_toast_handler();

		if (WinToastLib::WinToast::instance()->showToast(toastTemplate, handler) == -1L)
		{
			std::wcerr << L"Error: Unable to show initial toast notification." << '\n';
		}
	}

	// 更新倒计时
	void update_countdown()
	{
		// 每次定时器触发，减少倒计时
		default_countdown_time_--;

		// 检查倒计时是否结束
		if (default_countdown_time_ >= 0)
		{
			update(); // 更新窗口显示的内容
		}
		else
		{
			break_timer_->stop();
			send_initial_notification(L"倒计时通知", L"倒计时已结束！");
		}
	}

	// 将窗口移动到屏幕右边缘
	void move_window_to_right_edge()
	{
		if (const QScreen* screen = QGuiApplication::primaryScreen())
		{
			const QRect screenGeometry = screen->geometry();
			const int screenWidth = screenGeometry.width();
			const int screenHeight = screenGeometry.height();

			const int windowX = screenWidth - width();
			const int windowY = (screenHeight - height()) / 2;

			move(windowX, windowY);
		}
	}

	// 使用 Windows API 设置点击穿透
	void set_click_through() const
	{
		const auto hwnd = (HWND)winId(); // 获取窗口句柄
		const LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
		// 设置 WS_EX_LAYERED 和 WS_EX_TRANSPARENT 样式，实现点击穿透
		SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	}

protected:
	// 绘制倒计时内容
	void paintEvent(QPaintEvent* event) override
	{
		QPainter painter(this);

		// 确保背景完全透明
		painter.setBrush(Qt::NoBrush);
		painter.setPen(Qt::NoPen);

		// 设置字体
		const QFont font("Arial", 24, QFont::Bold);
		painter.setFont(font);

		// 计算每行文本的高度
		const int lineHeight = font.pointSize() * 1.5; // 调整比例以适合你的需求
		const int x = rect().center().x();
		const int y = rect().center().y() - 2 * lineHeight;

		// 绘制文本
		painter.setPen(QColor(255, 0, 0)); // 红色文字
		painter.drawText(x, y, "课");

		painter.setPen(QColor(0, 255, 0)); // 绿色文字
		painter.drawText(x, y + lineHeight, "程");

		painter.setPen(QColor(0, 0, 255)); // 蓝色文字
		painter.drawText(x, y + 2 * lineHeight, "表");

		painter.setPen(current_color_); // 黄色文字
		painter.drawText(x, y + 3 * lineHeight, QString::number(default_countdown_time_));

		painter.setPen(QColor(255, 0, 255)); // 品红色文字
		painter.drawText(x, y + 4 * lineHeight, "功");

		painter.setPen(QColor(0, 255, 255)); // 青色文字
		painter.drawText(x, y + 5 * lineHeight, "能");

		painter.setPen(QColor(128, 128, 128)); // 灰色文字
		painter.drawText(x, y + 6 * lineHeight, "测");

		painter.setPen(QColor(255, 165, 0)); // 橙色文字
		painter.drawText(x, y + 7 * lineHeight, "试");
	}

private slots:
	void update_paint()
	{
		// 动态修改文本和颜色
		//currentText = QString("Updated Text %1").arg(QTime::currentTime().toString("HH:mm:ss"));
		current_color_ = QColor(rand() % 256, rand() % 256, rand() % 256); // 随机颜色

		// 触发重新绘制
		update();
	}

private:
	int default_countdown_time_ = 60; // 倒计时60秒
	QTimer *break_timer_, *rgb_timer_; // 下课计时通知
	QString current_text_;
	QColor current_color_;
	QSystemTrayIcon* tray_icon_; // 托盘图标
	QMenu* tray_menu_; // 托盘菜单
	QAction *tray_config_, *tray_exit_; // 托盘选项
};

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	std::srand(static_cast<unsigned>(std::time(nullptr)));
	transparent_window window;
	window.show();


	return QApplication::exec();
}
