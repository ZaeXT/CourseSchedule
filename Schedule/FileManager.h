#pragma once

#include <QObject>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>
#include "configData.h"
#include "ScheduleData.h"
class FileManager : public QObject
{
    Q_OBJECT

public:
    explicit FileManager(QObject* parent = nullptr);

    void saveConfig();  // 保存配置到文件
    void loadConfig();  // 从文件加载配置
    void saveData();
    void loadData();

private:
    QString getFilePath(QString fileName); // 获取配置文件路径
};
