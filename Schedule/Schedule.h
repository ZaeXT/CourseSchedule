#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Schedule.h"

class Schedule : public QMainWindow
{
    Q_OBJECT

public:
    Schedule(QWidget *parent = nullptr);
    ~Schedule();

private:
    Ui::ScheduleClass ui;
};
