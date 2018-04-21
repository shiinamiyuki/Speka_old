#pragma once
#include "Speka.h"
SPEKA_BEGIN
const QString functionDef = "func_def";
const QString functionCall = "func_call";
const QString callArg = "call_arg";
const QString binaryOp = "bin_op";
const QString unaryOp = "unaryOp";
const QString iden = "id";
const QString num = "num";
const QString conditional = "cond";
const QString codeBlock = "block";
const QString tableIdx = binaryOp;//"idx";
const QString asgnOp = "assign_op";
const QString program = "prog";
const QString empty = "null";
const QString classDefinition = "classdef";
const QString ret = "return";
const QString whileLoop = "while";
const QString forLoop = "for";
const QString nativeCall = "native_call";
class STree;
typedef QSharedPointer<STree> Node;
class STree
{
public:
	QVector<Node> subNodes;
	QString type;
	QString content;
	QString content2;
	STree(const QString &s) { type = s; content = ""; subNodes.append(nullptr);subNodes.append(nullptr);
	};
	STree();
	void setContent(const QString&s) { content = s; }
	void setContent2(const QString&s) { content2 = s; }
	Node first() { return subNodes.first(); }
	Node second() { return subNodes.at(1); }
	void add(Node p) { if (p)subNodes.append(p);  else panic();}
	void setFirst(Node p) { if (p) { subNodes[0] = p; } else panic(); }
	void setSecond(Node p) { if (p) { subNodes[1] = p; }  else panic();}
	void print(uint depth = 0);
	void clean();
	void panic() {
		throw std::runtime_error("parser critical error");
	}
	~STree();
};
#define PRE_DO(x) skipSpace();//qDebug() <<__LINE__<<(#x)<< cur();
class Parser
{
	QString src;
	uint pos;
	Node ast;
	uint line;
	uint linePos;
	uint depth;
public:
	Parser(QString &s) { src = s;pos = 0;line = 1;linePos = 1; }
	QChar cur() {
		if (pos < src.length())return src[pos];else {
			return '\0';
		}
	}
	QChar next() {
		if (cur() == '\n') {
			line++;
			linePos = 1;
		}
		else
			linePos++;
		pos++; 
		return src[pos];
	}
	Node identifier();
	Node block();
	Node expr();
	Node postfix();
	QString op();
	QString assignOp();
	Node stmt();
	Node whileStmt();
	Node forStmt();
	Node cond_stmt();
	Node retStmt();
	Node nativeCallStmt();
	Node var_decl();
	Node method_def();
	Node or_expr();
	Node logic_term();
	Node and_term();
	Node add_expr();
	Node term();
	Node factor();
	Node prim_expr();
	Node idx_expr();
	Node call_expr();
	Node number();
	Node string();
	Node list();
	Node prog();
	Node classDef();
	void newLine();
	void check(QChar);
	void expect(QChar);
	void expect(QString&);
	bool matchNext(QChar);
	bool matchWord(QString&);
	void error(QString&);
	void skipSpace();
	void skipLineAndSpace();
	bool funcDefCheck();
};

SPEKA_END
