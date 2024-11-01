#include "ConfigDialog.h"
#include <QFormLayout>

#include "ConfigManager.h"

config_dialog::config_dialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("课程表配置");

	// 初始化控件
	notifyCheckBox = new QCheckBox("开启下课通知");
	breakTimeSpinBox = new QSpinBox();
	breakTimeSpinBox->setRange(0, 3600); // 设置下课时间范围
	showBreakTimeCheckBox = new QCheckBox("显示下课时间");
	courseCircleSpinBox = new QSpinBox();
	courseCircleSpinBox->setRange(1, 10); // 初始范围

	totalCourseCircleSpinBox = new QSpinBox();
	totalCourseCircleSpinBox->setRange(1, std::numeric_limits<int>::max()); // 无上限

	// 更新状态的最大值
	connect(totalCourseCircleSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int newTotal) {
		courseCircleSpinBox->setMaximum(newTotal); // 更新 courseCircleSpinBox 最大值
		});

	// 初始化时，设置 courseCircleSpinBox 最大值等于 totalCourseCircleSpinBox 值
	courseCircleSpinBox->setMaximum(totalCourseCircleSpinBox->value());

	positionComboBox = new QComboBox();
	positionComboBox->addItems({ "顶部", "底部", "左侧", "右侧" });

	rotationComboBox = new QComboBox();
	rotationComboBox->addItems({ "水平", "垂直" });

	xOffsetSpinBox = new QSpinBox();
	xOffsetSpinBox->setRange(-1000, 1000); // 偏移范围
	yOffsetSpinBox = new QSpinBox();
	yOffsetSpinBox->setRange(-1000, 1000); // 偏移范围

	spacelineCheckBox = new QCheckBox("显示上/下午分割线");
	rgbCheckBox = new QCheckBox("R！G！B！");

	// 布局
	QFormLayout* formLayout = new QFormLayout;
	formLayout->addRow("通知", notifyCheckBox);
	formLayout->addRow("课间时间（秒）", breakTimeSpinBox);
	formLayout->addRow("显示课间时间", showBreakTimeCheckBox);
	formLayout->addRow("当前课表周期", courseCircleSpinBox);
	formLayout->addRow("总课表周期", totalCourseCircleSpinBox);
	formLayout->addRow("显示位置", positionComboBox);
	formLayout->addRow("显示方向", rotationComboBox);
	formLayout->addRow("X方向偏移量", xOffsetSpinBox);
	formLayout->addRow("Y方向偏移量", yOffsetSpinBox);
	formLayout->addRow("上/下午分割线", spacelineCheckBox);
	formLayout->addRow("？？？", rgbCheckBox);

	QPushButton* saveButton = new QPushButton("保存");
	QPushButton* cancelButton = new QPushButton("取消");
	connect(saveButton, &QPushButton::clicked, this, &config_dialog::saveConfig);
	connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(saveButton);
	buttonLayout->addWidget(cancelButton);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addLayout(formLayout);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);

	// 加载配置项
	loadConfig();
}

// 加载配置
void config_dialog::loadConfig()
{
	configManager.loadConfig();
	notifyCheckBox->setChecked(app_config::if_notify);
	breakTimeSpinBox->setValue(app_config::break_time);
	showBreakTimeCheckBox->setChecked(app_config::if_show_break_time);
	courseCircleSpinBox->setValue(app_config::course_circle);
	totalCourseCircleSpinBox->setValue(app_config::total_course_circle);

	positionComboBox->setCurrentIndex(static_cast<int>(app_config::position));
	rotationComboBox->setCurrentIndex(static_cast<int>(app_config::rotation));

	xOffsetSpinBox->setValue(app_config::x_offset);
	yOffsetSpinBox->setValue(app_config::y_offset);
	spacelineCheckBox->setChecked(app_config::if_spaceline);
	rgbCheckBox->setChecked(app_config::r_g_b);
}

// 保存配置
void config_dialog::saveConfig()
{
	app_config::if_notify = notifyCheckBox->isChecked();
	app_config::break_time = breakTimeSpinBox->value();
	app_config::if_show_break_time = showBreakTimeCheckBox->isChecked();
	app_config::course_circle = courseCircleSpinBox->value();
	app_config::total_course_circle = totalCourseCircleSpinBox->value();
	app_config::position = static_cast<app_config::position_keys>(positionComboBox->currentIndex());
	app_config::rotation = static_cast<app_config::rotation_keys>(rotationComboBox->currentIndex());

	app_config::x_offset = xOffsetSpinBox->value();
	app_config::y_offset = yOffsetSpinBox->value();
	app_config::if_spaceline = spacelineCheckBox->isChecked();
	app_config::r_g_b = rgbCheckBox->isChecked();
	configManager.saveConfig();
	accept(); // 关闭对话框
}
