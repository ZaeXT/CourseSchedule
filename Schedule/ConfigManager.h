#pragma once

#include <QObject>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include "configData.h"

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject* parent = nullptr);

    void saveConfig();  // 保存配置到文件
    void loadConfig();  // 从文件加载配置

private:
    QString getConfigFilePath(); // 获取配置文件路径
};
