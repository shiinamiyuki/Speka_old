#pragma once
#include "Speka.h"
SPEKA_BEGIN
class SObject;
class STable;
class SClosure;
class SObject
{
	SInt iVal;
	SFloat dVal;
	Ptr<STable> table;
	Ptr<SClosure> function;
	Ptr<SStr> str;
	enum class Type;
	Type t;
public:
	enum class Type
	{
		none,
		Int,
		Float,
		Table,
		String,
		Closure
	};
	SObject();
	void setInt(SInt);
	void setFloat(SFloat);
	void setString(Ptr<SStr>);
	void setTable(Ptr<STable>);
	void setClosure(Ptr<SClosure>);
	void setNil() { t = Type::none; }
	void newArray(uint size);
	uint arrayLength()const;
	SObject operator + (const SObject&)const;
	SObject operator - (const SObject&)const;
	SObject operator * (const SObject&)const;
	SObject operator / (const SObject&)const;
	SObject operator && (const SObject&)const;
	SObject operator || (const SObject&)const;
	SObject operator % (const SObject&)const;
	SObject operator > (const SObject&)const;
	SObject operator >= (const SObject&)const;
	SObject operator <= (const SObject&)const;
	SObject lt (const SObject&)const;
	SObject operator != (const SObject&)const;
	SObject eq (const SObject&)const;
	SObject& operator = (const SObject&);
	void neg();
	void not();
	SObject key(QString&);
	void setKey(QString&,SObject&);
	SObject idx(uint i);
	void setIdx(uint, SObject&);
	void capture(SObject);
	bool checkType(SObject& o);
	inline bool isTable()const;
	inline bool isFloat()const;
	inline bool isInt()const;
	inline bool isClosure()const;
	inline bool isString()const;
	//these methods will do type checks
	inline Ptr<STable> tableValue()const;
	inline Ptr<SClosure> functionValue()const;
	inline Ptr<SStr> strValue()const;
	inline SFloat floatValue()const;
	inline SInt intValue()const;
	void error(QString & s)const;
	inline bool isTrue()const {
		if (isInt())
			return (bool)intValue();
		else if (isFloat())
			return (bool)floatValue();
		else if (isTable())
			return (bool)tableValue();
		else if (isClosure())
			return (bool)functionValue();
		else if (isString())
			return (bool)str;
		return false;
	}
	QString repr()const;
	void addPair(const QString&, const SObject&);
	inline Type type()const { return t; }
	int toInt()const {
		if (isFloat())
			return floatValue();
		else if (isInt())
			return intValue();
		else
			error(QString("illegal type for toInt()"));
	}
	~SObject();
};
bool operator== (const SObject& a, const SObject&b);
class SClosure
{
public:
	Vec<SObject> captureList;
	uint addr;
	SClosure(uint i) { addr = i; }
};
class STable : public Map<QString,SObject>
{
public:
	Vec<SObject> arr;
};
inline uint qHash(const SObject &, uint seed);
SPEKA_END
