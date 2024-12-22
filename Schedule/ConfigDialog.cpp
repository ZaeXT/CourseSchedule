#include "ConfigDialog.h"
#include <QFormLayout>
#include <QHeaderView>
#include <QColorDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QInputDialog>
#include <algorithm>
#include <QVector>
#include "FileManager.h"
#include "main_window.h"

config_dialog::config_dialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::Tool | Qt::Window);
	setWindowTitle("课程表配置");

	// 创建 QTabWidget
	tabWidget = new QTabWidget(this);

	// 创建配置页面
	configPage = new QWidget();
	createConfigPage(configPage);

	// 创建课程管理页面
	dataPage = new QWidget();
	createDataPage(dataPage);

	// 将页面添加到 QTabWidget
	tabWidget->addTab(configPage, "配置");
	tabWidget->addTab(dataPage, "课程管理");

	// 主布局
	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabWidget); // 将 QTabWidget 添加到主布局
	setLayout(mainLayout);

	// 加载配置项
	loadConfig();
	loadData();
}


void config_dialog::createConfigPage(QWidget* page) {
	// 初始化控件
	notifyCheckBox = new QCheckBox("开启下课通知");
	onTopCheckBox = new QCheckBox("置顶显示");
	breakTimeSpinBox = new QSpinBox();
	breakTimeSpinBox->setRange(0, 3600); // 设置下课时间范围
	showBreakTimeCheckBox = new QCheckBox("显示下课时间");
	courseCircleSpinBox = new QSpinBox();
	courseCircleSpinBox->setRange(1, 10); // 初始范围

	totalCourseCircleSpinBox = new QSpinBox();
	totalCourseCircleSpinBox->setRange(1, ScheduleData::data.size() ? ScheduleData::data.size() : 1); 

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
	formLayout->addRow("置顶（重启后生效）", onTopCheckBox);
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
	connect(saveButton, &QPushButton::clicked, this, &config_dialog::saveData);
	connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(saveButton);
	buttonLayout->addWidget(cancelButton);

	QVBoxLayout* pageLayout = new QVBoxLayout;
	pageLayout->addLayout(formLayout);
	pageLayout->addLayout(buttonLayout);
	page->setLayout(pageLayout);
}

// 加载配置
void config_dialog::loadConfig()
{
	fileManager.loadConfig();
	notifyCheckBox->setChecked(app_config::if_notify);
	onTopCheckBox->setChecked(app_config::if_on_top);
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
	app_config::if_on_top = onTopCheckBox->isChecked();
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
	fileManager.saveConfig();
	accept(); // 关闭对话框
}

void config_dialog::createDataPage(QWidget* page) {
	QVBoxLayout* pageLayout = new QVBoxLayout(page);

	// 创建颜色选择器按钮和标签
	QHBoxLayout* colorLayout = new QHBoxLayout();
	QLabel* colorLabel = new QLabel("字体颜色:");
	colorPickerButton = new QPushButton();
	defaultTextColor = ScheduleData::default_color; // 初始颜色设置为黑色
	updateColorButton(); // 更新按钮颜色
	colorLayout->addWidget(colorLabel);
	colorLayout->addWidget(colorPickerButton);
	colorLayout->addStretch(); // 添加弹簧，使按钮和标签靠左对齐

	pageLayout->addLayout(colorLayout); // 将颜色选择器布局添加到页面布局中

	// 创建字体大小选择器
	QHBoxLayout* fontSizeLayout = new QHBoxLayout();
	QLabel* fontSizeLabel = new QLabel("字体大小:");
	fontSizeSpinBox = new QSpinBox();
	fontSizeSpinBox->setRange(8, 128); // 设置字体大小范围，例如 8 到 24
	defaultFontSize = ScheduleData::default_font_size; // 默认字体大小设置为 10
	fontSizeSpinBox->setValue(defaultFontSize);
	fontSizeLayout->addWidget(fontSizeLabel);
	fontSizeLayout->addWidget(fontSizeSpinBox);
	fontSizeLayout->addStretch(); // 添加弹簧，使字体大小选择器靠左对齐

	pageLayout->addLayout(fontSizeLayout); // 将字体大小选择器布局添加到页面布局中


	courseTableWidget = new QTableWidget();
	courseTableWidget->setColumnCount(7);
	courseTableWidget->setRowCount(9);
	// 设置行表头
	QStringList colHeaders;
	colHeaders << QString("星期日") << QString("星期一") << QString("星期二") << QString("星期三") << QString("星期四") << QString("星期五") << QString("星期六");
	courseTableWidget->setHorizontalHeaderLabels(colHeaders);

	// 设置列表头
	QStringList rowHeaders;
	for (int i = 0; i < 9; ++i) {
		rowHeaders << QString("第%1节").arg(i + 1);
	}
	courseTableWidget->setVerticalHeaderLabels(rowHeaders);
	pageLayout->addWidget(courseTableWidget);

	// 添加保存和取消按钮
	QPushButton* saveButton = new QPushButton("保存");
	QPushButton* cancelButton = new QPushButton("取消");
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(saveButton);
	buttonLayout->addWidget(cancelButton);
	pageLayout->addLayout(buttonLayout);
	connect(saveButton, &QPushButton::clicked, this, &config_dialog::saveData);
	connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
	connect(colorPickerButton, &QPushButton::clicked, this, &config_dialog::openColorDialog);
	connect(fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &config_dialog::updateFontSize);

	updateCourseTable();
}

void config_dialog::updateFontSize(int fontSize) {
	defaultFontSize = fontSize;
	updateCourseTable();
}

void config_dialog::updateCourseTable() {
	courseTableWidget->blockSignals(true); // 禁用信号，防止在更新表格时触发 itemChanged 信号

	for (int day = 0; day < 7; ++day) {
		for (int row = 0; row < 9; ++row) {

			// 设置课程名称
			QTableWidgetItem* nameItem = new QTableWidgetItem(ScheduleData::data[day][row].courseName);
			courseTableWidget->setItem(row, day, nameItem);
		}
	}
	courseTableWidget->blockSignals(false); // 启用信号
}

void config_dialog::openColorDialog() {
	QColorDialog colorDialog(defaultTextColor, this); // 创建颜色对话框，并设置初始颜色
	if (colorDialog.exec() == QDialog::Accepted) {
		defaultTextColor = colorDialog.selectedColor();
		updateColorButton(); // 更新按钮颜色
		updateCourseTable(); // 更新表格字体颜色
	}
}

void config_dialog::updateColorButton() {
	// 设置按钮的背景颜色为当前选择的颜色
	colorPickerButton->setStyleSheet(QString("background-color: %1;").arg(defaultTextColor.name()));
}

// 加载配置（可能需要修改以加载课程数据）
void config_dialog::loadData()
{
	fileManager.loadData();
	updateCourseTable();
}

// 保存配置
void config_dialog::saveData()
{
	ScheduleData::default_color = defaultTextColor;
	ScheduleData::default_font_size = defaultFontSize;
	// 更新课程名称数据
	for (int day = 0; day < 7; ++day) {
		for (int row = 0; row < 9; ++row) {

			QTableWidgetItem* item = courseTableWidget->item(row, day);
			if (item) {
				ScheduleData::data[day][row].courseName = item->text();
				ScheduleData::data[day][row].color = defaultTextColor;
			}
		}
	}
	fileManager.saveData();
	QMessageBox::information(this, "保存", "数据已保存");
	saveConfig();
	accept();
}