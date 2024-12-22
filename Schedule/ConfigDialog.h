#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QColorDialog>
#include "ConfigData.h"
#include "ScheduleData.h"
#include "FileManager.h"

class config_dialog final : public QDialog
{
	Q_OBJECT

public:
	explicit config_dialog(QWidget* parent = nullptr);

private slots:
	void saveConfig();
	void saveData();
	void loadConfig();
	void loadData();
	void createConfigPage(QWidget* page);
	void createDataPage(QWidget* page);
	void updateCourseTable();
	void updateColorButton();
	void openColorDialog();
	void updateFontSize(int fontSize);

private:
	FileManager fileManager;
	QCheckBox* notifyCheckBox;
	QCheckBox* onTopCheckBox;
	QSpinBox* breakTimeSpinBox;
	QCheckBox* showBreakTimeCheckBox;
	QSpinBox* courseCircleSpinBox;
	QSpinBox* totalCourseCircleSpinBox;
	QComboBox* positionComboBox;
	QComboBox* rotationComboBox;
	QSpinBox* xOffsetSpinBox;
	QSpinBox* yOffsetSpinBox;
	QCheckBox* spacelineCheckBox;
	QCheckBox* rgbCheckBox;
	QTabWidget* tabWidget;
	QWidget* configPage;
	QWidget* dataPage;
	QTableWidget* courseTableWidget;
	QColor defaultTextColor;
	QPushButton* colorPickerButton;
	int defaultFontSize;
	QSpinBox* fontSizeSpinBox;
};
