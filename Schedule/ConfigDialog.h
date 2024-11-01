#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include "ConfigData.h"
#include "ConfigManager.h"

class config_dialog final : public QDialog
{
	Q_OBJECT

public:
	explicit config_dialog(QWidget* parent = nullptr);

private slots:
	void saveConfig();
	void loadConfig();

private:
	ConfigManager configManager;
	QCheckBox* notifyCheckBox;
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
};
