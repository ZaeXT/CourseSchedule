#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

class config_dialog final : public QDialog
{
	Q_OBJECT

public:
	explicit config_dialog(QWidget* parent = nullptr);
};
