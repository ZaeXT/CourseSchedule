#include "main_window.h"
#include "ScheduleData.h"
main_window::main_window(QWidget* parent) : QWidget(parent)
{
	file_manager_.loadConfig();
	// 设置无边框窗口&置顶
	app_config::if_on_top ? setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool) : setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

	// 设置窗口背景透明
	setAttribute(Qt::WA_TranslucentBackground);

	// 禁止系统绘制背景，防止白边
	setAttribute(Qt::WA_NoSystemBackground);

	// 设置窗口大小
	resize(600, 1600);

	// 移动窗口至屏幕右侧
	move_window_to_right_edge();

	// 设置点击穿透
	set_click_through();

	// 下课计时
	//break_timer_ = new QTimer(this);
	//connect(break_timer_, &QTimer::timeout, this, &main_window::update_countdown);
	//break_timer_->start(1000); // 每秒更新一次倒计时

	// R!G!B!
	rgb_timer_ = new QTimer(this);
	connect(rgb_timer_, &QTimer::timeout, this, &main_window::update_paint);
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
			if (!config_dialog_)
			{
				config_dialog_ = new config_dialog(this);
				connect(config_dialog_, &QDialog::finished, this, [this]() {
					config_dialog_ = nullptr; // ConfigDialog 关闭后重置指针
					});
				config_dialog_->show();
			}
			config_dialog_->raise();
			config_dialog_->activateWindow();
			config_dialog_->setAttribute(Qt::WA_DeleteOnClose);
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

	// 当前日期
	today_date_ = new QDate();

	// 加载数据
	file_manager_.loadConfig();
	file_manager_.loadData();

	// 初始化 WinToast
	// initialize_win_toast();

	// 发送初始通知
	// send_initial_notification(L"下课提醒", L"已下课！");
}

// 初始化 WinToast
//void main_window::initialize_win_toast()
//{
//	if (!WinToastLib::WinToast::isCompatible())
//	{
//		std::wcerr << L"Error: This system is not compatible with Toast notifications!" << '\n';
//		exit(1);
//	}
//
//	WinToastLib::WinToast::instance()->setAppName(L"课程提醒应用");
//	const auto aumi = WinToastLib::WinToast::configureAUMI(L"ZaeXT", L"ScheduleReminder", L"NotifyToast", L"NSCDV");
//	WinToastLib::WinToast::instance()->setAppUserModelId(aumi);
//
//	if (!WinToastLib::WinToast::instance()->initialize())
//	{
//		std::wcerr << L"Error: Unable to initialize WinToast." << '\n';
//		exit(1);
//	}
//}

// 使用 WinToast 发送初始通知
//void main_window::send_initial_notification(const wchar_t* title, const wchar_t* message)
//{
//	WinToastLib::WinToastTemplate toastTemplate(WinToastLib::WinToastTemplate::Text02);
//	toastTemplate.setTextField(title, WinToastLib::WinToastTemplate::FirstLine);
//	toastTemplate.setTextField(message, WinToastLib::WinToastTemplate::SecondLine);
//	toastTemplate.setAudioOption(WinToastLib::WinToastTemplate::AudioOption::Default);
//
//	const auto handler = new toast_handler();
//
//	if (WinToastLib::WinToast::instance()->showToast(toastTemplate, handler) == -1L)
//	{
//		std::wcerr << L"Error: Unable to show initial toast notification." << '\n';
//	}
//}


// 将窗口移动到屏幕右边缘
void main_window::move_window_to_right_edge()
{
	if (const QScreen* screen = QGuiApplication::primaryScreen())
	{
		const QRect screenGeometry = screen->geometry();
		const qint32 screenWidth = screenGeometry.width();
		const qint32 screenHeight = screenGeometry.height();
		const qint32 windowX = screenWidth - width();
		const qint32 windowY = (screenHeight - height()) / 2;

		move(windowX, windowY);
	}
}

// 使用 Windows API 设置点击穿透
void main_window::set_click_through() const
{
	const auto hwnd = (HWND)winId(); // 获取窗口句柄
	const LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	// 设置 WS_EX_LAYERED 和 WS_EX_TRANSPARENT 样式，实现点击穿透
	SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
}

// 绘制内容
void main_window::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	// 确保背景完全透明
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::NoPen);

	// 设置字体
	QFont font("Arial", ScheduleData::default_font_size, QFont::Bold);
	painter.setFont(font);

	// 使用 QFontMetrics 计算行高
	QFontMetrics fm(font);
	const qint32 lineHeight = fm.height();

	// 获取窗口的可用高度
	qint32 availableHeight = height();

	// 计算所有课程的总高度
	qint32 totalCoursesHeight = ScheduleData::data[today_date_->dayOfWeek()].size() * lineHeight;

	// 计算初始的 y 坐标，使所有课程垂直居中
	qint32 y = (availableHeight - totalCoursesHeight) / 2;

	// 计算 x 坐标，使文本在水平方向上居中偏左
	const qint32 x = width() * 0.2;

	for (const auto& course : ScheduleData::data[today_date_->dayOfWeek()])
	{
		painter.setPen(course.color);

		// 计算当前课程的文本大小
		QRect textRect = painter.boundingRect(x+app_config::x_offset, y+app_config::y_offset, width() - x, lineHeight, Qt::AlignRight | Qt::AlignVCenter, course.courseName);

		// 绘制文本
		painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter, course.courseName);

		y += lineHeight;
	}
}
void main_window::update_paint()
{
	// 动态修改文本和颜色
	//currentText = QString("Updated Text %1").arg(QTime::currentTime().toString("HH:mm:ss"));
	//current_color_ = QColor(rand() % 256, rand() % 256, rand() % 256); // 随机颜色

	// 触发重新绘制
	update();
}