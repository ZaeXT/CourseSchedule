#include "toast_handler.h"

void toast_handler::toastActivated() const 
{
	std::wcout << L"Toast clicked!" << '\n';
}

void toast_handler::toastActivated(const int action_index) const 
{
	std::wcout << L"Toast clicked with action index: " << action_index << '\n';
}

void toast_handler::toastDismissed(const WinToastDismissalReason state) const 
{
	if (state == UserCanceled)
	{
		std::wcout << L"Toast dismissed by user" << '\n';
	}
	else if (state == TimedOut)
	{
		std::wcout << L"Toast timed out" << '\n';
	}
	else
	{
		std::wcout << L"Toast dismissed for other reasons" << '\n';
	}
}

void toast_handler::toastFailed() const 
{
	std::wcout << L"Error showing toast" << '\n';
}