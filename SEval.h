#pragma once
#include "Speka.h"
#include "SOpcode.h"
#include "SObject.h"
#include "STree.h"
#include "Speka2Py.h"
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
class SpekaRunTime;
class SGen
{
	QSet<QString>			globals;
	Map<QString, uint>		varMap;		// since Speka only has function scope, one varmap for each method is enough
	Map<QString, uint>		classMap;
	Map<QString, QString>	superMap;
	Map<QString, uint>		natives;
	QString					entryClass;
	SProgram				output;
	uint					entryPoint;
	QString					currentClass;
public:
	friend class SpekaRunTime;
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
	void genStmt(Node i);
	uint genMethod(Node);
	uint getLocal(QString);
	void varDecl(QString);
	void addClass(QString);
	void setEntry(const QString&s) { entryClass = s; }
	uint getEntry()const { return entryPoint; }
	void error(const char*s) { throw std::runtime_error(s); }
	void addNative(const char*s, uint i) {
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
class SpekaRunTime;
typedef void (*NativeMethod)(SpekaVM*,void*);
class SpekaVM
{
	FastStack<CallFrame>	callStack;
	FastVec<SObject>		stack;
	FastVec<NativeMethod>	nativeMethod;
	FastVec<SObject>		evalStack;
	FastVec<SObject>		classPool;
	FastVec<SObject>		objectStack;	//stores this pointer
	FastVec<std::thread>	threadPool;
	uint					pc;
	uint					bp, sp, vp;
	SProgram				prog;
	void					*userData;
	uint entry;
public:
	void init();
	void eval();
	void loadProg(SProgram& p) {
		prog = p;prog.append(SOperation());prog.last().op = SOpcode::halt;
	*(uint*)prog.last().operand = entry;
	}
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
	void addNative(NativeMethod m, uint i) { nativeMethod[i] = m; }
	void createThread(SObject obj, uint threadFunc);
	void specialEval();
	friend class SpekaRunTime;
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
	void addNative(const char* name, NativeMethod m) {
		gen.addNative(name, nativeCount);
		vm.addNative(m, nativeCount);
		nativeCount++;
	}
	void compileString(QString& s) {
		Parser p(s);
		auto n = p.prog();
		if (n) {
			n->clean();
		}
		gen.gen(n);
	}
	void compileString(QString entry,QString& s) {
		Parser p(s);
		auto n = p.prog();
		if (n) {
			n->clean();
		}
		gen.setEntry(entry);
		gen.gen(n);
		vm.setEntry(gen.getEntry());
		vm.loadProg(gen.out());
		vm.prog.pop_back();
		vm.prog.append(SOperation());
		vm.prog.last().op = SOpcode::jmp_to_entry;
		*(uint*)vm.prog.last().operand = vm.entry;
		
	}
	void compileFile(QString entry, QString& file) {
		QTextStream ts(fopen(file.toStdString().c_str(), "r"));
		QString src = "";
		src.append(ts.readAll());
		compileString(entry,src);
	}
	void launch() {
		try {
			vm.eval();
		}
		catch (std::exception & e) {
			QTextStream ts(stdout);
			ts << "exception caught at pc = " << vm.pc << " " << (int)vm.prog[vm.pc].op << "\n";
			ts << e.what();
		}
	}
	QString toPy(QString &filename) {
		QTextStream ts(fopen(filename.toStdString().c_str(), "r"));
		QString s = ts.readAll();
		Parser p(s);
		auto n = p.prog();
		n->clean();
	//	n->print();
		Speka2Py py;
		py.toPy(n);
		return py.out().append("\nMain.main()\n");
	}
	void byteCodetoFile(const char* filename) {
		FILE *f = fopen(filename, "wb");
		for (auto i : vm.prog)
		{
			char buffer1[1];
			*(char*)buffer1 = (char)i.op;
			fwrite(buffer1, sizeof(char), 1, f);
			fwrite(&(i.operand), sizeof(char), 8, f);
			fwrite(i.attr.toStdString().c_str(), sizeof(char), i.attr.size()+1, f);
		}
		fclose(f);
	}
	void loadByteCode(const char*filename);
	void repl();
};
SPEKA_END

