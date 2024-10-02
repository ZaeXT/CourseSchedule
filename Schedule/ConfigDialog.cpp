#include "ConfigDialog.h"

config_dialog::config_dialog(QWidget* parent) : QDialog(parent)
{
	setWindowTitle("课程表配置");

	// 窗口布局
	const auto mainVLayout = new QVBoxLayout(this);
	const auto ifNotifyLabel = new QLabel("下课通知", this);
	mainVLayout->addWidget(ifNotifyLabel);
}
