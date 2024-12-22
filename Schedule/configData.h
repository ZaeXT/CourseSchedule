#pragma once
namespace app_config
{
	inline extern bool if_notify = false;								// 下课通知
	inline extern bool if_on_top = false;								// 置顶
	inline extern quint32 break_time = 600;							// 下课时间
	inline extern bool if_show_break_time = false;						// 时间显示
	inline extern quint32 course_circle = 1;							// 课表轮换状态
	inline extern quint32 total_course_circle = 1;						// 总轮换次数
	enum class position_keys { top, button, left, right };		// 显示位置值
	enum class rotation_keys { horizontal, vertical };			// 显示方向值
	inline extern position_keys position = position_keys::right;		// 显示位置
	inline extern rotation_keys rotation = rotation_keys::vertical;		// 显示方向
	inline extern qint32 x_offset = 0, y_offset = 0;					// 显示偏移值
	inline extern bool if_spaceline = false;							// 上下午分割线
	inline extern bool r_g_b = false;									// 彩蛋
}