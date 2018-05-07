#include <QtCore/QCoreApplication>
#include "STree.h"
#include "SObject.h"
#include "SEval.h"
#include "Speka2JS.h"
template<class Func>
uint timing(Func f) {
	uint i = GetTickCount();
	f();
	return GetTickCount() - i;
}
using namespace Speka;
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	try {
		SpekaRunTime rt;
		rt.init();
		if (argc > 1) {
			rt.compileFile("main", QString(argv[1]));
			rt.byteCodetoFile(QString(argv[1]).append("byte").toStdString().c_str());
			qDebug() << "elapsed:" << timing([&]() {
				rt.launch();}) << "ms";
		}
		else {
			rt.repl();
		}
		
		
		
	}
	catch (std::exception&e) {
		qDebug() << e.what();
	}
	return a.exec();
}
