#include "SEval.h"
SPEKA_BEGIN
QString OptoString(SOpcode op) {
	QString s;
	switch (op)
	{
	case SOpcode::none:
		s = "none";
		break;
	case SOpcode::add:
		s = "add";
		break;
	case SOpcode::sub:
		s = "sub";
		break;
	case SOpcode::div:
		s = "div";
		break;
	case SOpcode::mul:
		s = "mul";
		break;
	case SOpcode::eq:
		break;
	case SOpcode::lt:
		s = "lt";
		break;
	case SOpcode::le:
		break;
	case SOpcode::ge:
		break;
	case SOpcode::gt:
		s = "gt";
		break;
	case SOpcode::bz:
		s = "bz";
		break;
	case SOpcode::jmp:
		s = "jmp";
		break;
	case SOpcode::ne:
		break;
	case SOpcode::not:
		break;
	case SOpcode::and:
		break;
	case SOpcode:: or :
		break;
	case SOpcode::load_class:
		s = "load_class";
		break;
	case SOpcode::set_class:
		s = "set_class";
		break;
	case SOpcode::load_attr:
		s = "load_attr";
		break;
	case SOpcode::set_attr:
		s = "set_attr";
		break;
	case SOpcode::load_local:
		s = "load_local";
		break;
	case SOpcode::set_local:
		s = "set_local";
		break;
	case SOpcode::call:
		s = "call";
		break;
	case SOpcode::call_native:
		s = "call_native";
		break;
	case SOpcode::deep_copy:
		break;
	case SOpcode::load_function:
		s = "load_function";
		break;
	case SOpcode::new_table:
		break;
	case SOpcode::load_const_i:
		s = "load_const_i";
		break;
	case SOpcode::load_const_f:
		s = "load_const_f";
		break;
	case SOpcode::load_const_str:
		break;
	case SOpcode::load_idx:
		s = "load_idx";
		break;
	case SOpcode::set_idx:
		s = "set_idx";
		break;
	case SOpcode::ret:
		s = "ret";
		break;
	case SOpcode::pop_top:
		s = "pop_top";
		break;
	case SOpcode::push:
		s = "push";
		break;
	case SOpcode::push_this:
		s = "push_this";
		break;
	case SOpcode::dup:
		s = "dup";
		break;
	case SOpcode::inc_bp:
		s = "inc";
		break;
	case SOpcode::new_object:
		s = "new_object";
		break;
	case SOpcode::load_this:
		s = "load_this";
		break;
	default:
		s = "unknown";
		break;
	}
	return s;
}

void SpekaVM::init()
{
	callStack.create(1024*8);
	evalStack.create(1024*8);
	nativeMethod.create(1024 * 8);
	stack.create(1024 * 8);
	classPool.create(1024 * 8);
	nativeMethod.create(100);
	objectStack.create(1024);
	for (int i = 0;i <1024 * 8;i++) {
		SObject o;
		o.setTable(Ptr<STable>(new STable()));
		classPool.push(o);
	}
}

void SpekaVM::eval()
{
	SObject a, b;
	bp = sp = pc = 0;
	vp = 0;
//	pc = 10;
//	entry = 10;
	evalStack.clear();
	double d;
	int i;
	auto pp = prog.data();
	uint _size = prog.size();
	while (pc < _size) {
//		qDebug() << OptoString(prog[pc].op)<<prog[pc].attr << evalStack.size();
	//	qDebug() << sp;
		switch (pp[pc].op) {
		case SOpcode::new_object:
			a.setTable(Ptr<STable>(new STable(*classPool[*(int*)(pp[pc].operand)].tableValue())));
			evalStack.push(a);
			pc++;
			break;
		case SOpcode::add:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a + b);
	//		evalStack.top() = evalStack.top() + b;
			pc++;
			break;
		case SOpcode::sub:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a - b);
			pc++;
			break;
		case SOpcode::mul:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a * b);
			pc++;
			break;
		case SOpcode::div:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a / b);
			pc++;
			break;
		case SOpcode::eq:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a.eq(b));
			pc++;
			break;
		case SOpcode::ne:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a != b);
			pc++;
			break;
		case SOpcode::lt:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a.lt( b));
			pc++;
			break;
		case SOpcode::gt:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a > b);
			pc++;
			break;
		case SOpcode::le:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a <= b);
			pc++;
			break;
		case SOpcode::ge:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a >= b);
			pc++;
			break;
		case SOpcode::or:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a || b);
			pc++;
			break;
		case SOpcode::and:
			b = evalStack.pop();
			a = evalStack.pop();
			evalStack.push(a && b);
			pc++;
			break;
		case SOpcode::not:
			evalStack.top().not();
			pc++;
			break;
		case SOpcode::neg:
			evalStack.top().neg();
			pc++;
			break;
		case SOpcode::load_const_f:
			d = *(double*)(pp[pc].operand);
			a.setFloat(d);
			evalStack.push(a);
			pc++;
			break;
		case SOpcode::load_const_i:
			i = *(int*)(pp[pc].operand);
	//		qDebug() << i;
			a.setInt(i);
			evalStack.push(a);
			pc++;
			break;
		case SOpcode::load_local:
			i = *(int*)(pp[pc].operand);
			evalStack.push(stack[vp + i]);
			pc++;
			break;
		case SOpcode::set_local:
			i = *(int*)(pp[pc].operand);
			stack[vp + i] = evalStack.top();
			pc++;
			break;
		case SOpcode::call:
			i = *(int*)(pp[pc].operand);
			callStack.top().pc = pc;
			callStack.top().bp = bp;
			callStack.top().sp = sp - i;
			callStack.top().vp = vp;
			callStack.inc();
			vp = sp - i;
			bp = sp;
			pc = evalStack.pop().functionValue()->addr; //crucial: the function is on evalStack. not stack
			break;
		case SOpcode::ret:
			if (callStack.isEmpty())
				return;
			objectStack.pop();			
			callStack.pop();
			pc = callStack.top().pc;
			bp = callStack.top().bp; 
			sp = callStack.top().sp; 
			vp = callStack.top().vp; 
			pc++;
			break;
		case SOpcode::pop_top:
			pc++;
			evalStack.pop();
			break;
		case SOpcode::push:			
			pc++;
			stack[sp] = evalStack.pop();
			sp++;
			break;
		case SOpcode::dup:
			pc++;
			evalStack.push(evalStack.top());
			break;
		case SOpcode::push_this:
			pc++;
			objectStack.push(evalStack.pop());
			break;
		case SOpcode::call_native:
			nativeMethod[*(int*)(pp[pc].operand)](this);
			pc++;
			break;
		case SOpcode::load_attr:	// pop
			a = evalStack.pop();
	//		qDebug() << a.repr();
		//	qDebug() << evalStack.size();
			evalStack.push(a.key(pp[pc].attr));
			pc++;
			break;
		case SOpcode::set_attr:	// no pop
			a = evalStack.pop();
			evalStack.top().setKey(pp[pc].attr, a);
			pc++;
			break;
		case SOpcode::load_idx:
			a = evalStack.pop();
			//		qDebug() << a.repr();
			evalStack.push(a.key(pp[pc].attr));
			pc++;
			break;
		case SOpcode::set_idx:
			a = evalStack.pop();
			evalStack.top().setKey(pp[pc].attr, a);
			pc++;
			break;
		case SOpcode::load_class:
			evalStack.push(classPool[*(int*)(pp[pc].operand)]);
		//	qDebug() << evalStack.top().repr();
			pc++;
			break;
		case SOpcode::set_class:
			classPool[*(int*)(pp[pc].operand)].setTable(Ptr<STable>(new STable(*evalStack.pop().tableValue())));
			pc++;
			break;
		case SOpcode::jmp:
			pc = *(int*)(pp[pc].operand);
			break;
		case SOpcode::bz:
			if (evalStack.pop().isTrue())
				pc++;
			else
				pc = *(int*)(pp[pc].operand);
			break;
		case SOpcode::load_function:
			i = *(int*)(pp[pc].operand);
			a.setClosure(Ptr<SClosure>(new SClosure(i)));
			evalStack.push(a);
			pc++;
			break;
		case SOpcode::jmp_to_entry:
			evalStack.clear();
			pc = entry;
			break;
		case SOpcode::inc_bp:
			bp++;
			sp++;
			pc++;
			break;
		case SOpcode::load_this:
			evalStack.push(objectStack.top());
			pc++;
			break;
		}
	//	qDebug() << "size = " << evalStack.size() << evalStack[0].repr();
	}
	qDebug() << evalStack.top().repr();
}




void SGen::preprocess(Node)
{
}

void SGen::gen(Node n)
{
	for (auto i : n->subNodes) {
		auto name = i->content;
		addClass(name);
	}
	
	for (auto i : n->subNodes) {
		genClass(i);
	}
}

void SGen::genExpr(Node node)
{
	if (node->type == unaryOp) {
		genUnaryOp(node);
	}
	else if (node->type == binaryOp) {
		genBinaryOp(node);
	}
	else if (node->type == asgnOp) {
		genAssignOp(node);
	}
	else if (node->type == functionCall) {
		genCall(node);
	}
	else if (node->type == iden) {
	//	qDebug() << "identifier" << node->content;
		if (node->content == "this") {
			write(SOpcode::load_this, 0, QString::null);
		}else if (classMap.contains(node->content)) {
			write(SOpcode::load_class, classMap[node->content], QString::null);
		}
		else if (varMap.contains((node->content))) {
			write(SOpcode::load_local, getLocal(node->content), QString::null);
		}
		else {
		//	qDebug() << "var decl" << node->content;
			varDecl(node->content);
			write(SOpcode::inc_bp, 0, QString::null);
			write(SOpcode::load_local, getLocal(node->content), QString::null);
		}
	}
	else if (node->type == num) {
		if (node->content.contains('.'))
			write(SOpcode::load_const_f, node->content.toDouble(), QString::null);
		else
			write(SOpcode::load_const_i, node->content.toInt(), QString::null);
	}
}

void SGen::genBlock(Node node)
{
	if (node->type != codeBlock) {
		auto i = node;
		if (i->type == conditional) {
			genCond(i);
		}
		else if (i->type == whileLoop) {
			genWhile(i);
		}
		else if (i->type == forLoop) {
			genFor(i);
		}
		else if (i->type == nativeCall) {
			genNative(i);
			//	write(SOpcode::pop_top, 0, QString::null);
		}
		else if (i->type == functionCall) {
			genCall(i);
			write(SOpcode::pop_top, 0, QString::null);
		}
		else if (i->type == ret) {
			genReturn(i);
		}
		else {
			genExpr(i);
			write(SOpcode::pop_top, 0, QString::null);
		}
		return;
	}
	//node->print();
	for (auto i : node->subNodes)
	{
		if (i->type == conditional) {
			genCond(i);
		}
		else if (i->type == whileLoop) {
			genWhile(i);
		}else if (i->type == forLoop) {
			genFor(i);
		}
		else if (i->type == nativeCall) {
			genNative(i);
		//	write(SOpcode::pop_top, 0, QString::null);
		}
		else if (i->type == functionCall) {
			genCall(i);
			write(SOpcode::pop_top, 0, QString::null);
		}
		else if (i->type == ret) {
			genReturn(i);
		}
		else {
			genExpr(i);
			write(SOpcode::pop_top, 0, QString::null);
		}
		
	}
}

void SGen::genCond(Node node)
{
//	node->print();
	genExpr(node->first());		// cond
//	qDebug() << "---------------------cond";
	uint bzIdx = output.size();
	write(SOpcode::bz, 0, QString::null);	// jmp to else
	genBlock(node->second());	
	if (node->subNodes.size() == 3) { //else 
	//	qDebug() << "---------------------else";
		
		uint elseEnd = output.size();
		write(SOpcode::jmp, 0, QString::null); // jmp to elseEnd
		*(int*)(output[bzIdx].operand) = output.size();
	//	node->subNodes[2]->print();
		genBlock(node->subNodes[2]);
		*(int*)(output[elseEnd].operand) = output.size();
	}
	else {
//		qDebug() << "--------------------- no else";
		*(int*)(output[bzIdx].operand) = output.size();
	}

}

void SGen::genWhile(Node node)
{/*
	uint start = output.size();
	uint cond = compileExpr(node->subNodes[0]);
	uint jmpIdx = output.size();
	write(BZ, cond, (uint)0);
	compileBlock(node->subNodes[1]);
	write(JMP, start);
	uint end = output.size();
	*(uint*)(output.data() + jmpIdx + 1 + 4) = end;*/
	uint start = output.size();
	genExpr(node->first());
	uint jmpIdx = output.size();
	write(SOpcode::bz, 0, QString::null);
	genBlock(node->second());
	write(SOpcode::jmp, start, QString::null);
	*(int*)(output[jmpIdx].operand) = output.size();
}

void SGen::genFor(Node)
{
}

void SGen::genNative(Node n)
{
	auto name = n->content;
	write(SOpcode::call_native, natives[name], QString::null);// the native call happens inside a method;
															//therefore the arguments have been converted into local variables
															// we don't need to know how many arguments there are
}

void SGen::genReturn(Node n)
{
	if (n->subNodes.size() == 1) {
		genExpr(n->first());
	}
	write(SOpcode::ret, 0, QString::null);
}

void SGen::genBinaryOp(Node node)
{
	SOpcode op;
	if (node->content == ".") {
		op = SOpcode::load_attr;
		genExpr(node->first());
		if (node->second()->type == iden) { //load attr
			write(op, 0, node->second()->content);
			return;
		}
		else {
			error("load_attr error");
		}
	}else if (node->content == "[") {
		op = SOpcode::load_idx;
		genExpr(node->first());	
		write(op, 0, node->second()->content);
		
	}else if (node->content == "+") {
		op = SOpcode::add;
	}
	else if (node->content == "-") {
		op = SOpcode::sub;
	}
	else if (node->content == "*") {
		op = SOpcode::mul;
	}
	else if (node->content == "/") {
		op = SOpcode::div;
	}	
	else if (node->content == ">") {
		op = SOpcode::gt;
	}
	else if (node->content == "<") {
		op = SOpcode::lt;
	}
	else if (node->content == ">=") {
		op = SOpcode::ge;
	}
	else if (node->content == "<=") {
		op = SOpcode::le;
	}
	else if (node->content == "|") {
		op = SOpcode::or;
	}
	else if (node->content == "&") {
		op = SOpcode::and;
	}else if (node->content == "==") {
		op = SOpcode::eq;
	}else if (node->content == "!=") {
		op = SOpcode::ne;
	}
	genExpr(node->first());
	genExpr(node->second());
	write<int>(op, 0, QString::null);
}

void SGen::genUnaryOp(Node node)
{
	genExpr(node->first());
	if (node->content == "!")
		write(SOpcode::not, 0, QString::null);
	else if(node->content == "-")
		write(SOpcode::neg, 0, QString::null);
	
}

void SGen::genAssignOp(Node node)
{
	
//	node->print();
	if (node->content == "=") {
		if (node->first()->type == binaryOp && node->first()->content == ".") {
			genExpr(node->first()->first());
			genExpr(node->second());
			write(SOpcode::set_attr, 0, node->first()->second()->content);
		}
		else if (node->first()->type == binaryOp && node->first()->content == "[") {
			genExpr(node->first()->first());
			genExpr(node->second());
			write(SOpcode::set_idx, 0, node->first()->second()->content);
		}
		else {
			if (node->first()->type == iden)
			{
				genExpr(node->second());
				auto s = node->first()->content;
				if(varMap.contains(s))
					write(SOpcode::set_local, getLocal(s), QString::null);
				else if (classMap.contains(s)) {
					write(SOpcode::set_class, classMap[s], QString::null);
				}
				else if (s == "this") {
					error("this cannot be assigned");
				}
				else {
					varDecl(s);
					write(SOpcode::inc_bp, 0, QString::null);
					write(SOpcode::set_local, getLocal(s), QString::null);
				}
			}
		}
	}
}

void SGen::genCall(Node node)
{
	auto callee = node->first();
	auto arg = node->second();
	uint count = 0;

	for (auto i : arg->subNodes) {
		genExpr(i);
		write(SOpcode::push, 0, QString::null);
		count++;
	}
//	callee->print();
	//callee->first()->print();
	if (callee->subNodes.size() != 2) {
		error("illegal call syntax");
	}
	if (callee->second()->content == "new") {	//constructor
												//before calling the new() method, we have to insert few commands to create a new object on the stack
	//	qDebug() << "class" << callee->first()->content;
		write(SOpcode::new_object, classMap[callee->first()->content], QString::null);
	}
	else
		genExpr(callee->first());
	write(SOpcode::dup, 0, QString::null);					//we need two copys of the object, one for this_ptr, one for the method
	write(SOpcode::push_this, 0, QString::null);
	write(SOpcode::load_attr, 0, callee->second()->content);//load method name, now the top is the method
	write(SOpcode::call, count, QString::null);
}

void SGen::genClass(Node node)
{
	auto name = node->content;
	
	if (node->content2.isEmpty() == false) {
		write(SOpcode::load_class, classMap[node->content2], QString::null);
		write(SOpcode::set_class, classMap[node->content], QString::null);
	}
	//	SRT's classPool will be intialilzed so are the unused class object will be set to empty
	for (auto i : node->subNodes) {
		// className.methodName = code
		if (i->content == "main" && name == entryClass) {
			entryPoint =  output.size() + 2;
		}
		write(SOpcode::load_class, classMap[node->content], QString::null);
		uint addr = genMethod(i);
		write(SOpcode::load_function, addr, QString::null);
		write(SOpcode::set_attr, 0, i->content);
	}
}

uint SGen::genMethod(Node node)
{
	varMap.clear();
	uint idx = output.size();
	write(SOpcode::jmp, 0, QString::null);
	uint addr = output.size();
	auto arg = node->first();
	for (auto i : arg->subNodes) {
		varDecl(i->content);
		
	}
	genBlock(node->second());
	write(SOpcode::load_const_i,0, QString::null);	// default ret stmt; every method MUST return a value
	write(SOpcode::ret, 0, QString::null);
	*(uint*)(output[idx].operand) = output.size();
	return addr;
}

uint SGen::getLocal(QString s)
{
	return varMap[s];
}

void SGen::varDecl(QString str)
{
	uint s = varMap.count();
	varMap.insert(str, s);
}


void SGen::addClass(QString s)
{
	uint i = classMap.size();
	classMap.insert(s, i);
}

template<typename _Tp>
inline void SGen::write(SOpcode o, _Tp i, const QString& s)
{
//	qDebug() << output.size() << OptoString(o) << i << s;
	SOperation op;
	op.op = o;
	op.attr = s;
	*(_Tp*)(op.operand) = i;
	output.append(op);
}

SPEKA_END