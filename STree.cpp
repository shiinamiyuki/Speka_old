#include "STree.h"
SPEKA_BEGIN

STree::STree()
{
	type = "";
	content = "";
	subNodes.append(nullptr);subNodes.append(nullptr);
}


void STree::print(uint depth)
{
	QTextStream ts(stdout);
	for (int i = 0;i < depth;i++) {
		ts << "\t";
	}
	ts << type << " : " << content << " "<< content2 <<  "\n";
	ts.flush();
	for (auto i : subNodes) {
		if (i)
			i->print(depth + 1);
	}
}

void STree::clean()
{
	for (auto i : subNodes) {
		if (i)
			i->clean();
	}
	if (subNodes.size() > 0) {
		while (subNodes.size() > 0&&subNodes.last().isNull())
			subNodes.removeLast();
	}
	reshape();
}

void STree::reshape()
{
	for (auto i : subNodes) {
		if (i)
			i->reshape();
	}
	// += a b -> a = a + b
	//  op=						=
	//a    b				a		op
	//							a			b
	if (type == asgnOp && content.endsWith("=") && content.length() == 2) {
		Node node(new STree(binaryOp));
		node->content = content[0];
		content = "=";
		auto left = first();
		auto right = second();
		node->setFirst(left);
		node->setSecond(right);
		setSecond(node);
	//	print();
	}
}

STree::~STree()
{
}


Node Parser::identifier()
{
	PRE_DO(identifier);
	Node node(new STree(iden));
	if (cur().isDigit()) {
		error(QString("an identifier cannot begin with number"));
	}
	else {
		QString s;
		while (cur().isLetterOrNumber() || cur() == '_') {
			s += cur();
			next();
		}
		node->setContent(s);
		return node;
	}
}
Node Parser::number()
{
	PRE_DO(number);
	Node node(new STree(num));
	if (cur().isLetter()) {
		error(QString("a number cannot begin with letter"));
	}
	else {
		QString s;
		while (cur().isDigit() || cur() == '.'  || cur() == 'e') {
			s += cur();
			next();
		}
		bool suc;
		s.toDouble(&suc);
		if (!suc) {
			error(s.append(" is not a vaild number"));
		}
		else {
			node->setContent(s);
			return node;
		}
	}
}
Node Parser::block()
{
	PRE_DO(block);
	if (cur() == '{') {
		next();
		Node node(new STree(codeBlock));
		node->subNodes.clear();
		skipLineAndSpace();
		while(1){
			if (cur()== '}') {
				break;
			}
			node->add(stmt());
			skipLineAndSpace();
		}
		check('}');
		next();
		return node;
	}
	else
		return stmt();
}

Node Parser::expr()
{
	PRE_DO(expr);
	Node node = postfix();
	while (1) {
		skipSpace();
		if (cur() == '\n') {
			next();
			return node;
		}
		auto s = assignOp();
		if (s.isEmpty())
			break;//error
		Node n(new STree(asgnOp));
		n->setContent(s);
		n->setFirst(node);
		n->setSecond(or_expr());
		node = n;
	}
	return node;
}

Node Parser::postfix()
{
	PRE_DO(postfix);
	auto result = prim_expr();
	skipSpace();
	while (cur() == '[' || cur() == '(' || cur() == '.') {
		if (cur() == '[' || cur() == '.')
		{
			Node node(new STree(tableIdx));
			node->setFirst(result);
			node->setContent(cur());
			node->setSecond(idx_expr());
			result = node;
		}
		else if (cur() == '(') {
			Node node(new STree(functionCall));
			node->setFirst(result);
			node->setSecond(call_expr());
			result = node;
		}
	}
	return result;
}
Node Parser::idx_expr()
{
	PRE_DO(idx_expr);
	if (cur() == '[') {
		next();
		auto n = or_expr();
		skipSpace();
		check(']');
		next();
		return n;
	}
	else if (cur() == '.') {
		next();
		return identifier();
	}
	else {
		error(QString("not a vaild idx_expr"));
		return nullptr;
	}
}

Node Parser::call_expr()
{
	PRE_DO(call_expr);
	if (cur() == '(') {
		next();
		skipLineAndSpace();
		if (cur() == ')') {
			auto n =  Node(new STree(callArg));
			n->subNodes.clear();
			next();
			return n;
		}
		Node result(new STree(callArg));
		result->subNodes.clear();
		result->add(or_expr());
		skipSpace();
		while (cur() == ',') {
			next();
			result->add(or_expr());
			skipSpace();
		}
		check(')');
		next();
		return result;
	}
	else {
		error(QString("illegal call_expr"));
	}
	return nullptr;
}

#define OP_MATCH(x)		if (cur() == x) {\
							if (next() == '=') {\
								s +=x;s+='=';\
								next();\
							}\
							else {\
								s = x;\
							}\
						}
QString Parser::op()
{
	PRE_DO(op);
//	qDebug() << src[pos - 1] << cur();
	QString s;
	OP_MATCH('>')
	else OP_MATCH('<')
	else if (cur() == '!') {
	expect('=');
	s = "!=";
	next();
}
	else if (cur() == '=') {
		expect('=');
		s = "==";
		next();
	}
	else {
		s = "";
	}
	//qDebug() << "op" << s;
	return s;
}

QString Parser::assignOp()
{
	PRE_DO(assignOp);
	QString s;
	OP_MATCH('+')
	else OP_MATCH('-')
	else OP_MATCH('*')
	else OP_MATCH('/')
	else if (cur() == ':') {
		expect('=');
		s = ":=";
		next();
	}
	else if (cur() == '='&&!matchNext('=')) {
		s = "=";
		next();
	}
	else {
		s = "";
	}
//	qDebug() << "op" << s;
	return s;
}

Node Parser::stmt()
{
	PRE_DO(stmt);
	if (matchWord(QString("if"))) {
		return cond_stmt();
	}
	else if (matchWord(QString("while"))) {
		return whileStmt();
	}
	else if (matchWord(QString("for"))) {
		return forStmt();
	}
	else if(matchWord(QString("return"))){
		return retStmt();
	}
	else if (matchWord(QString("break"))) {
		return breakStmt();
	}
	else if (matchWord(QString("continue"))) {
		return continueStmt();
	}
	else if (matchWord(QString("native"))) {
		return nativeCallStmt();
	}
	else {
		return expr();
	}

}

Node Parser::whileStmt()
{
	PRE_DO(whileStmt);
	expect(QString("while"));
	Node node(new STree(whileLoop));
	node->setFirst(or_expr());
	skipLineAndSpace();
	node->setSecond(block());
	return node;
}

Node Parser::forStmt()
{
	PRE_DO(forStmt);
	return Node();
}

Node Parser::cond_stmt()
{
	PRE_DO(cond_stmt);
	expect(QString("if"));
	Node node(new STree(conditional));
	node->setFirst(or_expr());
	skipLineAndSpace();
	node->setSecond(block());
	if (matchWord(QString("else"))) {
		expect(QString("else"));
		skipLineAndSpace();
		node->add(block());
	}
	return node;
}

Node Parser::retStmt()
{
	PRE_DO(retStmt);
	expect(QString("return"));
	skipSpace();	
	Node n(new STree(ret));
	if (cur() == '\n')
		return n;
	n->setFirst(or_expr());
	skipSpace();
	check('\n');
	next();
	return n;
}
Node Parser::breakStmt()
{
	PRE_DO(breakStmt);
	expect(QString("break"));
	skipSpace();
	Node n(new STree(breakFromLoop));
	check('\n');
	return n;
}

Node Parser::continueStmt()
{
	PRE_DO(continueStmt);
	expect(QString("continue"));
	skipSpace();
	Node n(new STree(continueLoop));
	check('\n');
	return n;
}
Node Parser::nativeCallStmt()
{
	PRE_DO(nativeCallStmt);
	expect(QString("native"));
	Node n(new STree(nativeCall));
	n->setContent(identifier()->content);
	n->subNodes.clear();
	skipSpace();
	check('\n');
	next();
	return n;
}

Node Parser::var_decl()
{
	PRE_DO(var_decl);
	return Node();
}

Node Parser::method_def()
{
	PRE_DO(method_def);
	expect(QString("def"));
	skipLineAndSpace();
	Node func(new STree(functionDef));
	Node arg(new STree(callArg));
	func->setContent(identifier()->content);
	next();
	if (cur() == ')') {
		func->setFirst(arg);
	}
	else {
		arg->subNodes.clear();
		while (1) {
			arg->add(identifier());
			skipSpace();
			if (cur() == ')') {
				break;
			}
			else if (cur() != ',') {
				error(QString("character \'").append(cur()).append("\' is not a legal token in function arguments"));
				return nullptr;
			}
			next();
		}
		func->setFirst(arg);
	}
	next();
	skipLineAndSpace();
	check('{');
	func->setSecond(block());
	return func;
}

Node Parser::or_expr()
{
	PRE_DO(or_expr);
	if (cur() == "i"&&matchNext('f') && src[pos + 2].isLetter() == false) {
		return cond_stmt();
	}
	Node result = logic_term();
	while (1) {
		skipSpace();
		if (cur() == '|')
			next();
		else
			break;
		Node node(new STree(binaryOp));
		node->content = '|';
		node->setFirst(result);
		node->setSecond(logic_term());
		result = node;
	}
	return result;
}

Node Parser::logic_term()
{
	PRE_DO(logic_term);
	Node result = and_term();
	while (1) {
		skipSpace();
		if (cur() == '&')
			next();
		else
			break;
		Node node(new STree(binaryOp));
		node->content = '&';
		node->setFirst(result);
		node->setSecond(and_term());
		result = node;
	}
	return result;
}

Node Parser::and_term()
{
	PRE_DO(and_term);
#if 0
	if (cur() == ('(')) {
		next();
		auto n = or_expr();
		check(')');
		next();
		return n;
	}
	else {
#endif
		Node result = add_expr();
		while (1) {
	//		qDebug() << cur();
		//	next();
			auto s = op();
			if (s.isEmpty())
				break;
			else {
				Node node(new STree(binaryOp));
				node->content = s;
				node->setFirst(result);
				node->setSecond(add_expr());
				result = node;
			}
		}
		return result;
#if 0
}
#endif // 0
}

Node Parser::add_expr()
{
	PRE_DO(add_expr);
	Node result(new STree());
	if (cur() == '+' || cur() == '-') {
		result->type = unaryOp;
		result->content = cur();
		next();
		result->setFirst(term());
		
	}
	else {
		result = term();
	}
	skipSpace();
	while (cur() == '+' || cur() == '-') {
		Node node(new STree());
		node->type = binaryOp;
		node->content = cur();
		next();
		auto right = term();
		node->setFirst(result);
		node->setSecond(right);//caution circular reference
		result = node;
	}
	return result;
}

Node Parser::term()
{
	PRE_DO(term);
	Node result = factor();
	skipSpace();
	while (cur() == '*' || cur() == '/') {
		Node node(new STree());
		node->type = binaryOp;
		node->content = cur();
		next();
		auto right = factor();
		node->setFirst(result);
		node->setSecond(right);//caution circular reference
		result = node;
		skipSpace();
	}
	return result;
}

Node Parser::factor()
{
	PRE_DO(factor);
	if (cur()==('(')) {
		next();
		auto n = or_expr();//add_expr();
		check(')');
		next();
		return n;
	}
	else {
		auto c = cur();
		if (c.isDigit()) {
			return number();
		}
		else if (c == "!") {
			Node node(new STree(unaryOp));
			node->content = c;
			next();
			node->setFirst(prim_expr());
			return node;
		}
		else {
			return postfix();
		}
	}
}

Node Parser::prim_expr()
{
	PRE_DO(prim_expr);
	if (cur() == ('(')) {
		next();
		auto n = or_expr();//expr();
		check(')');
		return n;
	}
	else if (cur().isLetter() || cur() == "_") {
		auto n = identifier();
		return n;
	}
	else if (cur().isDigit()) {
		return number();
	}
	else if (cur() == "\'" || cur() == "\"") {
		return string();
	}
	return Node();
}




Node Parser::string()
{
	PRE_DO(string);
	Node n(new STree(stringLiteral));
	QString s = "";
	QChar start = cur();
	next();
	while (cur() != start) {
		s += cur();
		next();
	}
	next();
	n->content = s;
	//qDebug() << s;
	return n;
}

Node Parser::list()
{
	PRE_DO(list);
	return Node();
}

Node Parser::prog()
{
	Node n(new STree(program));
	n->subNodes.clear();
	skipSpace();
	while (pos < src.size()) {
		if (matchWord(QString("import")))
		{
			expect(QString("import"));
			Node node(new STree("import"));
			QString s = "";
			while (cur() != "\n") {
				s += cur();
				next();
			}
			next();
			if (s.endsWith(".spk") == false) {
				s.append(".spk");
			}
			node->content = s.trimmed();
			n->add(node);
		}
		else if (matchWord(QString("def"))) {
			n->add(method_def());
		}
		else if (matchWord(QString("class"))) {
			n->add(classDef());
		}
		else {
			n->add(stmt());
		}		
		skipLineAndSpace();
	}
	return n;
}

Node Parser::classDef()
{
	PRE_DO(classDef);
	Node n(new STree(classDefinition));
	n->subNodes.clear();
	expect(QString("class"));
	skipLineAndSpace();
	n->setContent(identifier()->content);
	skipLineAndSpace();
	if (cur() == '=') {
		next();
		skipLineAndSpace();
		n->setContent2(identifier()->content);
	}
	skipLineAndSpace();
	check('{');
	next();
	skipLineAndSpace();
	while (cur() != '}') {
		n->add(method_def());
		skipLineAndSpace();
	}
	next();
	return n;
}



void Parser::newLine()
{
}

void Parser::check(QChar c)
{
	PRE_DO(check);
	if (cur() != c) {
		error(QString("expect ").append(c).append(" but have ").append(cur()));
	}
}

void Parser::expect(QChar c)
{
	PRE_DO(expect);
	if (next() != c) {
		error(QString("expect ").append(c));
	}
}

void Parser::expect(QString &s)
{
	PRE_DO(expect);
	bool f = true;
	for (auto i : s) {
		if (cur() != i)
		{
			error(QString(i).append(" expected"));
		}
		next();
	}
	
}

bool Parser::matchNext(QChar c)
{
	return src[pos + 1] == c;
}

bool Parser::matchWord(QString &s)
{
	PRE_DO(matchWord)
	bool f = true;
	uint l = line;
	uint lp = linePos;
	uint p = pos;
	for (auto i : s) {
		if (cur() != i)
		{
			f = false;
			break;
		}
		next();
	}
	if (f) {
		f = !cur().isLetterOrNumber();
	}
	line = l;
	linePos = lp;
	pos = p;
	return f;
}

void Parser::error(QString & s)
{
	QString msg = s.append(" at line: ").append(QString::number(line)).append(":").append(QString::number(linePos)).append('\n');
	msg.prepend("syntax error: ");
	throw std::runtime_error(msg.toStdString());
}

void Parser::skipSpace()
{
		if (cur().isSpace() && cur() !='\n') {
			while (cur().isSpace() && cur() != '\n') { next(); }
		}
		if (cur() == '/' && matchNext(QChar('*'))) {
			while (!(cur() == '*' && matchNext(QChar('/')))) { next(); }
			next();
			next();
		}
		if (cur() == '/' && matchNext(QChar('/'))) {
			while (cur() != '\n') { next(); }
			next();
		}
}

void Parser::skipLineAndSpace()
{
	if (cur().isSpace()) {
		while(1) {
			if (cur() == '/' && matchNext(QChar('*'))) {
				while (!(cur() == '*' && matchNext(QChar('/')))) { next(); }
				next();
				next();
			}
			if (cur() == '/' && matchNext(QChar('/'))) {
				while (cur() != '\n') { next(); }
				next();
			}
			if (cur().isSpace() == false)
				break;
			next();
		}
	}
	 
}


bool Parser::funcDefCheck()
{
	PRE_DO(funcDefCheck);
	uint k = pos;
	uint l = line;
	uint lp = linePos;
	if (cur() == '(') {
		next();
		while (1) {			
			if (cur() == ')')
				break;
		
			if (!(cur().isLetterOrNumber() || cur() == ' ' || cur() == ',' || cur() == '_')) {
				line = l;
				linePos = lp;
				pos = k;
				return false;
			}
			next();
		}
	}
	next();
	skipSpace();
	if (cur() == '-') {
		if (next() == '>') {
			line = l;
			linePos = lp;
			pos = k;
			return true;
		}
	}
	line = l;
	linePos = lp;
	pos = k;
	return false;
	
}

SPEKA_END

