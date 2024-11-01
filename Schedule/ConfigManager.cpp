#include "configmanager.h"

ConfigManager::ConfigManager(QObject* parent)
    : QObject(parent)
{
    loadConfig();  // 初始化时尝试加载配置
}

// 获取配置文件路径
QString ConfigManager::getConfigFilePath()
{
    QString configPath = QDir::homePath() + "/.config/CourseSchedule";
    QDir dir(configPath);
    if (!dir.exists()) {
        dir.mkpath(configPath);  // 创建目录
    }
    return configPath + "/config.json";
}

// 保存配置
void ConfigManager::saveConfig()
{
    QJsonObject configJson;

    // 保存 app_config 中的配置到 JSON 对象
    configJson["if_notify"] = app_config::if_notify;
    configJson["break_time"] = static_cast<int>(app_config::break_time);
    configJson["if_show_break_time"] = app_config::if_show_break_time;
    configJson["course_circle"] = static_cast<int>(app_config::course_circle);
    configJson["total_course_circle"] = static_cast<int>(app_config::total_course_circle);

    // 将枚举值转换为整数并存储
    configJson["position"] = static_cast<int>(app_config::position);
    configJson["rotation"] = static_cast<int>(app_config::rotation);

    configJson["x_offset"] = static_cast<int>(app_config::x_offset);
    configJson["y_offset"] = static_cast<int>(app_config::y_offset);
    configJson["if_spaceline"] = app_config::if_spaceline;
    configJson["r_g_b"] = app_config::r_g_b;

    // 转换为 JSON 文档并写入文件
    QFile file(getConfigFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(configJson);
        file.write(doc.toJson());
        file.close();
    }
}

// 加载配置
void ConfigManager::loadConfig()
{
    QFile file(getConfigFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (doc.isObject()) {
            QJsonObject configJson = doc.object();

            // 读取 JSON 对象中的配置项
            app_config::if_notify = configJson["if_notify"].toBool();
            app_config::break_time = static_cast<unsigned>(configJson["break_time"].toInt());
            app_config::if_show_break_time = configJson["if_show_break_time"].toBool();
            app_config::course_circle = static_cast<unsigned>(configJson["course_circle"].toInt());
            app_config::total_course_circle = static_cast<unsigned>(configJson["total_course_circle"].toInt());

            // 枚举的读取，包含无效值检测
            int positionValue = configJson["position"].toInt(static_cast<int>(app_config::position));
            if (positionValue >= static_cast<int>(app_config::position_keys::top) &&
                positionValue <= static_cast<int>(app_config::position_keys::right)) {
                app_config::position = static_cast<app_config::position_keys>(positionValue);
            }
            else {
                app_config::position = app_config::position_keys::top;  // 默认值
            }

            int rotationValue = configJson["rotation"].toInt(static_cast<int>(app_config::rotation));
            if (rotationValue >= static_cast<int>(app_config::rotation_keys::horizontal) &&
                rotationValue <= static_cast<int>(app_config::rotation_keys::vertical)) {
                app_config::rotation = static_cast<app_config::rotation_keys>(rotationValue);
            }
            else {
                app_config::rotation = app_config::rotation_keys::vertical;  // 默认值
            }

            app_config::x_offset = static_cast<unsigned>(configJson["x_offset"].toInt());
            app_config::y_offset = static_cast<unsigned>(configJson["y_offset"].toInt());
            app_config::if_spaceline = configJson["if_spaceline"].toBool();
            app_config::r_g_b = configJson["r_g_b"].toBool();
        }
    }
}
