#pragma once
#include "Speka.h"
#include "SOpcode.h"
#include "SObject.h"
#include "STree.h"
SPEKA_BEGIN
struct SOperation;
typedef Vec<SOperation> SProgram;
template<class T>
class FastStack
{
protected:
	T* data;
	uint _size;
	int t;
public:
	void create(uint s = 1024 * 1024) {
		data = new T[s];
		_size = s;
		t = -1;
	}
	inline T& top() {
		return data[t];
	}
	inline void push(const T& o) {
		data[++t] = o;
	}
	inline T& pop() {
		return data[t--];
	}
	inline bool isEmpty() {
		return t < 0;
	}
	inline int size()const {
		return t + 1;
	}
	inline void clear() { t = -1; }
	void free() { delete data; }
	inline void inc() { t++; }
};
template<class T>
class FastVec: public FastStack<T>
{
public:
	T& operator[](int i) { return data[i]; }
	inline void push_back(const T& o) { push(o); }
	inline void pop_back(const T&o) { pop(o); }
	
};
struct SOperation
{
	SOpcode op;
	uchar operand[8];
	QString attr;	// used for attributes
	SOperation& operator = (const SOperation& o) {
		op = o.op;
		attr = o.attr;
		for (uint i = 0;i < 8;i++)
			operand[i] = o.operand[i];
		return *this;
	}
};
class SGen
{
	Map<QString, uint>	varMap;		// since Speka only has function scope, one varmap for each method is enough
	Map<QString, uint>	classMap;
	Map<QString, uint>	natives;
	QString				entryClass;
	SProgram			output;
	uint				entryPoint;
public:
	void preprocess(Node);			//this will scan all the class definions
	void gen(Node);
	void genExpr(Node);
	void genBlock(Node);
	void genCond(Node);
	void genWhile(Node);
	void genFor(Node);
	void genNative(Node);
	void genReturn(Node);
	void genBinaryOp(Node);
	void genUnaryOp(Node);
	void genAssignOp(Node);
	void genCall(Node);
	void genClass(Node);
	uint genMethod(Node);
	uint getLocal(QString);
	void varDecl(QString);
	void addClass(QString);
	void setEntry(const QString&s) { entryClass = s; }
	uint getEntry()const { return entryPoint; }
	void error(const char*s) { throw std::runtime_error(s); }
	void regFunc(const char*s, uint i) {
		natives.insert(s, i);
	}
	template<typename _Tp>
	void write(SOpcode, _Tp, const QString&);
	 SProgram& out() { return output; }
};
struct CallFrame
{
	uint pc;
	uint sp;
	uint bp;
	uint vp;
};
class SpekaVM;
typedef void (*NativeMethod)(SpekaVM*);
class SpekaVM
{
	FastStack<CallFrame>	callStack;
	FastVec<SObject>		stack;
	FastVec<NativeMethod>	nativeMethod;
	FastVec<SObject>		evalStack;
	FastVec<SObject>		classPool;
	FastVec<SObject>		objectStack;	//stores this pointer
	uint					pc;
	uint					bp, sp, vp;
	SProgram				prog;
	uint entry;
public:
	void init();
	void eval();
	void loadProg(SProgram& p) { prog = p;prog.append(SOperation());prog.last().op = SOpcode::jmp_to_entry; }
	SObject getArg(uint i) {
		return stack[vp + i];
	}
	SObject getThis() {
		return objectStack.top();
	}
	void ret(SObject& o) {
		evalStack.push(o);
		objectStack.pop();
		callStack.pop();
		pc = callStack.top().pc;
		bp = callStack.top().bp;
		sp = callStack.top().sp;
		vp = callStack.top().vp;
	//	pc++;
	}
	void setEntry(uint i) { entry = i; }
	void regFunc(NativeMethod m, uint i) { nativeMethod[i] = m; }
};
class SpekaRunTime
{
	SpekaVM vm;
	SGen gen;
	uint nativeCount;
	void loadLibrary();
public:
	SpekaRunTime() { nativeCount = 0; }
	void init() {
		vm.init();
		loadLibrary();
	}
	void regFunc(const char* name, NativeMethod m) {
		gen.regFunc(name, nativeCount);
		vm.regFunc(m, nativeCount);
		nativeCount++;
	}
	void compileString(QString entry,QString& s) {
		Parser p(s);
		auto n = p.prog();
		if (n) {
			n->clean();
	//		n->print();
		}
		gen.setEntry(entry);
		gen.gen(n);
		vm.loadProg(gen.out());
		vm.setEntry(gen.getEntry());
	}
	void compileFile(QString entry, QString& file) {
		QTextStream ts(fopen(file.toStdString().c_str(), "r"));
		compileString(entry,ts.readAll());
	}
	void launch() {
		vm.eval();
	}
};
SPEKA_END

