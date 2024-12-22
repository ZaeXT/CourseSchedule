#pragma once
#include <QVector>
#include <QColor>
namespace ScheduleData {
	inline QColor default_color = QColor(80, 80, 80);					// Ĭ����ʾ��ɫ����ң�
	inline int default_font_size = 24;											// �����С
	enum class Type:quint8 { name, spaceline };
	enum class Condition:quint8 { active, post_active, done, undone };			// �γ�״̬�����ڽ��У������꣬����ɣ�δ���
	struct CourseData {															// �γ�����
		Type type = Type::name;													// �������ͣ��γ�/�ָ��ߣ��γ̣�
		QString courseName = "NaN";												// �γ�����
		QColor color = default_color;											// �γ���ɫ
		Condition cond = Condition::undone;										// �γ�״̬��δ��ɣ�
	};
	typedef QVector<CourseData> DaySchedule;									// һ������
	typedef QVector<DaySchedule> WeekSchedule;									// һ������
	extern WeekSchedule data;													// ��ά��[��][����]
}
