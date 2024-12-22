#pragma once
#include <QVector>
#include <QColor>
namespace ScheduleData {
	inline QColor default_color = QColor(80, 80, 80);					// 默认显示颜色（深灰）
	inline int default_font_size = 24;											// 字体大小
	enum class Type:quint8 { name, spaceline };
	enum class Condition:quint8 { active, post_active, done, undone };			// 课程状态：正在进行，刚上完，已完成，未完成
	struct CourseData {															// 课程数据
		Type type = Type::name;													// 数据类型：课程/分割线（课程）
		QString courseName = "NaN";												// 课程名称
		QColor color = default_color;											// 课程颜色
		Condition cond = Condition::undone;										// 课程状态（未完成）
	};
	typedef QVector<CourseData> DaySchedule;									// 一天数据
	typedef QVector<DaySchedule> WeekSchedule;									// 一周数据
	extern WeekSchedule data;													// 二维：[天][数据]
}
