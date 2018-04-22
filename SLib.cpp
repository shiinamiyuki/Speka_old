#include "SEval.h"
#include <math.h>
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
void system_print(SpekaVM*vm) {
	QTextStream ts(stdout);
	ts << vm->getArg(0).repr() << "\n";
	ts.flush();
}
#define MATH_FUNC_TEMPLATE(func)void math_##func(SpekaVM*vm) { \
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
void math_pow(SpekaVM*vm) {
	auto x = vm->getArg(0);
	auto y = vm->getArg(1);
	SObject o;
	o.setFloat(pow(x.toFloat(), y.toFloat()));
	vm->ret(o);
}
void SpekaRunTime::loadLibrary()
{
	regFunc("array_new", array_new);
	regFunc("array_set", array_set);
	regFunc("array_get", array_get);
	regFunc("system_print", system_print);
	regFunc("math_sin", math_sin);
	regFunc("math_cos", math_cos);
	regFunc("math_tan",math_tan);
	regFunc("math_asin", math_asin);
	regFunc("math_acos", math_acos);
	regFunc("math_atan", math_atan);
	regFunc("math_exp", math_exp);
	regFunc("math_pow", math_pow);
}

SPEKA_END