#include <QtCore/QCoreApplication>
#include "STree.h"
#include "SObject.h"
#include "SEval.h"
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
		if (argc > 1)
			rt.compileFile("Main", QString(argv[1]));
		else
			rt.compileFile("Main", QString("test_case.spk"));
		qDebug() <<"elapsed:" <<timing([&]() {
			rt.launch();}) << "ms";
		
	}
	catch (std::exception&e) {
		qDebug() << e.what();
	}
	return a.exec();
}
