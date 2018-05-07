#include "SEval.h"
#include <math.h>
SPEKA_BEGIN
void array_get(SpekaVM*vm,void*)
{
	auto idx = vm->getArg(0);
	vm->ret(vm->getThis().key(QString("data")).idx(idx.toInt()));
}
void array_set(SpekaVM*vm, void*)
{
	auto idx = vm->getArg(0);
	auto x = vm->getArg(1);
	vm->getThis().key(QString("data")).setIdx(idx.toInt(), x);
}
void array_new(SpekaVM*vm, void*)
{
	auto size = vm->getArg(0);
	SObject o;
	o.newArray(size.toInt());
	vm->ret(o);
}
void system_print(SpekaVM*vm, void*) {
	QTextStream ts(stdout);
	ts << vm->getArg(0).repr() << "\n";
	ts.flush();
}
#define MATH_FUNC_TEMPLATE(func)void math_##func(SpekaVM*vm,void*) { \
									auto x = vm->getArg(0); \
									SObject o; \
									o.setFloat(func(x.toFloat())); \
									vm->ret(o);\
								}
MATH_FUNC_TEMPLATE(sin)
MATH_FUNC_TEMPLATE(cos)
MATH_FUNC_TEMPLATE(tan)
MATH_FUNC_TEMPLATE(asin)
MATH_FUNC_TEMPLATE(acos)
MATH_FUNC_TEMPLATE(atan)
MATH_FUNC_TEMPLATE(log)
MATH_FUNC_TEMPLATE(exp)
void math_pow(SpekaVM*vm, void*) {
	auto x = vm->getArg(0);
	auto y = vm->getArg(1);
	SObject o;
	o.setFloat(pow(x.toFloat(), y.toFloat()));
	vm->ret(o);
}
void thread_start(SpekaVM*vm, void*) {
	auto obj = vm->getArg(0);
	auto f = vm->getArg(1);
	qDebug() << f.repr();
	vm->createThread(obj, f.functionValue()->addr);
}
void math_mod(SpekaVM* vm,void*)
{
	auto x = vm->getArg(0).toInt();
	auto y = vm->getArg(1).toInt();
	SObject o;
	o.setInt(x%y);
	vm->ret(o);
}

void SpekaRunTime::loadLibrary()
{
	addNative("array_new", array_new);
	addNative("array_set", array_set);
	addNative("array_get", array_get);
	addNative("system_print", system_print);
	addNative("math_sin", math_sin);
	addNative("math_cos", math_cos);
	addNative("math_tan",math_tan);
	addNative("math_asin", math_asin);
	addNative("math_acos", math_acos);
	addNative("math_atan", math_atan);
	addNative("math_exp", math_exp);
	addNative("math_pow", math_pow);
	addNative("math_mod", math_mod);
	addNative("thread_start", thread_start);
}


SPEKA_END