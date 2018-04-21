#include "SObject.h"
SPEKA_BEGIN


SObject::SObject()
{
	t = Type::none;
	table.clear();
	function.clear();
	str.clear();
}


void SObject::setInt(SInt i)
{
	t = Type::Int;
	iVal = i;
}

void SObject::setFloat(SFloat f)
{
	t = Type::Float;
	dVal = f;
}

void SObject::setString(Ptr<SStr> s)
{
	t = Type::String;
	str = s;
}

void SObject::setTable(Ptr<STable> p)
{
	t = Type::Table;
	table = p;
}

void SObject::setClosure(Ptr<SClosure> c)
{
	t = Type::Closure;
	function = c;
}
#define SOBJECT_OP_TEMPLATE(op)  static SObject o; if (isInt()) { \
										if (rhs.isInt()) \
										{ \
											o.setInt(intValue() op rhs.intValue()); \
										} \
										else if (rhs.isFloat()) { \
											o.setFloat(intValue() op rhs.floatValue()); \
										} \
										else \
										{ \
											error(QString("illegal right operand type ").append(QString::number(int(t)))); \
										} \
										} \
										else if (isFloat()) { \
											if (rhs.isFloat()) { \
												o.setFloat(floatValue() op rhs.floatValue()); \
											} \
											else if (rhs.isInt()) { \
												o.setFloat(floatValue() op rhs.intValue()); \
											} \
											else { \
												error(QString("illegal right operand type ").append(QString::number(int(t)))); \
											} \
										} else { \
											error(QString("illegal left operand type" ).append(QString::number(int(t)))); \
										} \
										return o;
#define SOBJECT_LOGIC_OP_TEMPLATE(op)  static SObject o; if (isInt()) { \
										if (rhs.isInt()) \
										{ \
											o.setInt(intValue() op rhs.intValue()); \
										} \
										else if (rhs.isFloat()) { \
											o.setInt(intValue() op rhs.floatValue()); \
										} \
										else \
										{ \
											error(QString("illegal right operand type ").append(QString::number(int(t)))); \
										} \
										} \
										else if (isFloat()) { \
											if (rhs.isFloat()) { \
												o.setInt(floatValue() op rhs.floatValue()); \
											} \
											else if (rhs.isInt()) { \
												o.setInt(floatValue() op rhs.intValue()); \
											} \
											else { \
												error(QString("illegal right operand type ").append(QString::number(int(t)))); \
											} \
										} else { \
											error(QString("illegal left operand type ").append(QString::number(int(t)))); \
										} \
										return o;

#if 1
void SObject::newArray(uint size)
{
	Ptr<STable> tab(new STable());
	tab->arr.resize(size);
	setTable(tab);
}
uint SObject::arrayLength() const
{
	if (isTable()) {
		return table->arr.length();
	}
	else {
		error(QString("only table objects can call arrayLenght()"));
	}
}
SObject  SObject::operator+(const SObject &rhs)const
{
	SOBJECT_OP_TEMPLATE(+)
}

SObject  SObject::operator-(const SObject &rhs)const
{
	SOBJECT_OP_TEMPLATE(-)
}

SObject  SObject::operator*(const SObject &rhs)const
{
	SOBJECT_OP_TEMPLATE(*)
}

SObject  SObject::operator/(const SObject &rhs)const
{
	SOBJECT_OP_TEMPLATE(/)
}
SObject  SObject::operator&&(const SObject &rhs)const
{
	SOBJECT_LOGIC_OP_TEMPLATE(&&)
}

SObject  SObject::operator||(const SObject &rhs)const
{
	SOBJECT_LOGIC_OP_TEMPLATE(||)
}



SObject  SObject::operator>(const SObject &rhs)const
{
	SOBJECT_LOGIC_OP_TEMPLATE(>)
}

SObject  SObject::operator>=(const SObject &rhs)const
{
	SOBJECT_LOGIC_OP_TEMPLATE(>=)
}

SObject  SObject::operator<=(const SObject &rhs)const
{
	SOBJECT_LOGIC_OP_TEMPLATE(<=)
}
SObject  SObject::lt(const SObject &rhs)const
{
	SOBJECT_LOGIC_OP_TEMPLATE(<)
}

SObject  SObject::operator!=(const SObject &rhs)const
{
	SObject o;
	o.setInt(!(*this == rhs));
	return o;
}

SObject  SObject::eq(const SObject &rhs)const
{
	SObject o;
	o.setInt(*this == rhs);
	return o;

}
SObject & SObject::operator=(const SObject & o)
{
	this->t = o.t;
	switch (o.type())
	{
	case Type::Closure:
		function = o.function;
		break;
	case Type::Table:
		table = o.table;
		break;
	case Type::Float:
		dVal = o.dVal;
		break;
	case Type::Int:
		iVal = o.iVal;
		break;
	case Type::String:
		str = o.str;
		break;
	default:
		break;
	}	
	return *this;
}
#endif
void SObject::neg()
{
	if (isInt()) {
		iVal = -iVal;
	}
	else if (isFloat()) {
		dVal = -dVal;
	}
	else {
		error(QString("illegal operand for neg"));
	}
}

void SObject::not()
{
	if (isInt()) {
		iVal = !iVal;
	}
	else if (isFloat()) {
		dVal = !dVal;
	}
	else {
		error(QString("illegal operand for not"));
	}
}

SObject SObject::key(QString& k)
{
	if (isTable()) {
		return table->operator[](k);
	}
	else {
		error(QString("illegal operand for key()").append(" type is ").append(QString::number((int)t)));
	}
}

void SObject::setKey(QString & s,SObject& o)
{
	if (isTable()) {
		auto iter = table->find(s);
		if (iter == table->end()) {
			addPair(s, o);
		}
		else {
			(*table)[s] = o;
		}
	}
	else {
		error(QString("illegal operand for setKey()"));
	}
}

SObject SObject::idx(uint i)
{
	if (isTable()) {
		return table->arr[i];
	}
	else {
		error(QString("illegal operand for key()").append(" type is ").append(QString::number((int)t)));
	}
}

void SObject::setIdx(uint i, SObject &o)
{
	if (isTable()) {
		if (table->arr.size() <= i) {
			error(QString("index out of range"));
		}
		else {
			table->arr[i] = o;
		}
	}
	else {
		error(QString("illegal operand for setKey()"));
	}
}

void SObject::capture(SObject o)
{
	if (isClosure()) {
		function->captureList.append(o);
	}
	else {
		error(QString("illegal operand for capture()"));
	}
}

bool SObject::checkType(SObject & o)
{
	return o.t == t;
}

inline bool SObject::isTable() const
{
	return t == Type::Table;
}

inline bool SObject::isClosure() const
{
	return t == Type::Closure;
}

inline bool SObject::isString() const
{
	return t == Type::String;
}

inline Ptr<STable> SObject::tableValue()const
{
	return table;
}

inline Ptr<SClosure> SObject::functionValue()const
{
	return function;
}

inline Ptr<SStr> SObject::strValue()const
{
	return str;
}

inline SFloat SObject::floatValue()const
{
	return dVal;
}

inline SInt SObject::intValue()const
{
	return iVal;
}

void SObject::error(QString& s)const
{
	QString msg = "internal error ";
	msg.append(s);
	throw std::runtime_error(msg.toStdString());
}



QString SObject::repr() const
{
	QString s;
	if (isInt()) {
		s = QString::number(iVal);
	}
	else if (isFloat()) {
		s = QString::number(dVal);
	}
	else if (isString()) {
		s = QString("\"").append(*str).append("\"");
	}
	else if (isTable()) {
		s = "{";
		for (auto i : table->keys()) {
			s.append(i).append(":").append(table->value(i).repr()).append(" ");
		}
		s.append("}");
	}
	return s;
}

void SObject::addPair(const QString& k, const SObject & v)
{
	if (isTable()) {
		if(v.t != Type::none)
			tableValue()->insert(k, v);
		else {
			tableValue()->remove(k);
		}
	}
	else {
		error(QString("object must have a table type to use addPair()"));
	}
}

inline bool SObject::isFloat() const
{
	return t == Type::Float;
}

inline bool SObject::isInt() const
{
	return t == Type::Int;
}



SObject::~SObject()
{
}

bool operator == (const SObject & a, const SObject & b)
{
	if (a.type() != b.type())
		return false;
	if (a.type() == SObject::Type::Int) {
		return a.intValue() == b.intValue();
	}
	if (a.type() == SObject::Type::Float) {
		return a.floatValue() == b.floatValue();
	}
	if (a.type() == SObject::Type::Table) {
		return a.tableValue() == b.tableValue();
	}
	if (a.type() == SObject::Type::String) {
		return a.strValue() == b.strValue();
	}
	if (a.type() == SObject::Type::Closure) {
		return a.functionValue() == b.functionValue();
	}
	return true;
}
inline uint qHash(const SObject & a, uint seed)
{
	if (a.type() == SObject::Type::Int) {
		return ::qHash(a.intValue(),seed);
	}
	if (a.type() == SObject::Type::Float) {
		return ::qHash(a.floatValue(), seed);
	}
	if (a.type() == SObject::Type::Table) {
		return ::qHash(a.tableValue().data(), seed);
	}
	if (a.type() == SObject::Type::String) {
		return ::qHash(a.strValue().data(),seed);
	}
	if (a.type() == SObject::Type::Closure) {
		return ::qHash(a.functionValue().data(),seed);
	}
	return ::qHash(0, seed);
}

SPEKA_END

