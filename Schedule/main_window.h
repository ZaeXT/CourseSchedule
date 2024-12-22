#pragma once
#include <QApplication>
#include <QMenu>
#include <QPainter>
#include <QScreen>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>
#include <wintoastlib.h>

#include "ConfigDialog.h"
#include "toast_handler.h"
#include "FileManager.h"


// 透明窗口类，显示倒计时
class main_window final : public QWidget
{
public:
	explicit main_window(QWidget* parent = nullptr);

	// 初始化 WinToast
	static void initialize_win_toast();

	// 使用 WinToast 发送初始通知
	static void send_initial_notification(const wchar_t* title, const wchar_t* message);

	// 更新倒计时
	void update_countdown();

	// 将窗口移动到屏幕右边缘
	void move_window_to_right_edge();

	// 使用 Windows API 设置点击穿透
	void set_click_through() const;


protected:
	// 绘制倒计时内容
	void paintEvent(QPaintEvent* event) override;

private slots:
	void update_paint();

private:
	config_dialog* config_dialog_;
	FileManager file_manager_;
	qint32 default_countdown_time_ = 60; // 倒计时60秒
	QTimer* break_timer_, * rgb_timer_; // 下课计时通知
	QString current_text_;
	QColor current_color_;
	QSystemTrayIcon* tray_icon_; // 托盘图标
	QMenu* tray_menu_; // 托盘菜单
	QAction* tray_config_, * tray_exit_; // 托盘选项
	QDate* today_date_;
};