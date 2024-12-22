#pragma once
#include <wintoastlib.h>

// 自定义事件处理器类，用于处理 Toast 通知的点击和关闭事件
class toast_handler final : public WinToastLib::IWinToastHandler
{
public:
	void toastActivated() const override;

	void toastActivated(const int action_index) const override;

	void toastDismissed(const WinToastDismissalReason state) const override;

	void toastFailed() const override;
};