#include "SEval.h"
SPEKA_BEGIN
void array_get(SpekaVM*vm)
{
	auto idx = vm->getArg(0);
	vm->ret(vm->getThis().key(QString("data")).idx(idx.toInt()));
}
void array_set(SpekaVM*vm)
{
	auto idx = vm->getArg(0);
	auto x = vm->getArg(1);
	vm->getThis().key(QString("data")).setIdx(idx.toInt(), x);
}
void array_new(SpekaVM*vm)
{
	auto size = vm->getArg(0);
	SObject o;
	o.newArray(size.toInt());
	vm->ret(o);
}
void print(SpekaVM*vm) {
	QTextStream ts(stdout);
	ts << vm->getArg(0).repr() << "\n";
	ts.flush();
}
void SpekaRunTime::loadLibrary()
{
	regFunc("array_new", array_new);
	regFunc("array_set", array_set);
	regFunc("array_get", array_get);
	regFunc("print", print);
}

SPEKA_END