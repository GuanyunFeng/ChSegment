#include "segment.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	segment w;
	w.setWindowTitle(u8"G�����ķִ��� Beta 0.1V");
	w.show();
	return a.exec();
}
