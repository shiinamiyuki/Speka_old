#pragma once
#include <qvector.h>
#include <qstring.h>
#include <qmap.h>
#include <qhash.h>
#include <qsharedpointer.h>
#include <memory>
#include <qdebug.h>
#include <qstack.h>
#include <Windows.h>


#define SPEKA_BEGIN namespace Speka{
#define SPEKA_END }

#define Vec QVector
#define Map QHash
#define Ptr QSharedPointer
#define Stack QStack
typedef long long SInt;
typedef double SFloat;
typedef QString SStr;
typedef  QString SKey;