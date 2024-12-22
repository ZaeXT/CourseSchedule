#include "wintoastlib.h"
#include <cstdlib>
#include <ctime>
#include <QApplication>
#include <QFont>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QRect>
#include <QScreen>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>
#include <string>
#include <windows.h>
#include "ConfigData.h"
#include "ConfigDialog.h"
#include "main_window.h"


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	std::srand(static_cast<unsigned>(std::time(nullptr)));
	main_window window;
	window.show();


	return QApplication::exec();
}
