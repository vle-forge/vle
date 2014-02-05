/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VPZEXPCOND_H
#define VPZEXPCOND_H
#include <QList>
#include <QString>
#include <QDomNode>

class vleVpz;
class vleVpzModel;
class vpzExpCond;

class vpzExpCondValue
{
public:
    enum ValueType { TypeUnknown, TypeBoolean, TypeDouble, TypeInteger,
		     TypeNull, TypeString, TypeComplex, TypeMap, TypeMatrix,
		     TypeSet, TypeTable, TypeTuple, TypeXml};

public:
    vpzExpCondValue(vpzExpCondValue *parent = 0);
    ~vpzExpCondValue();
    ValueType type();
    void      setType(ValueType type);
    QString   getTypeName();
    bool      isAtomic();
    void      setName(QString name);
    QString   getName();
    QString   getDisplayName();
    QString   getDisplayValue();
    void      addObject(vpzExpCondValue *obj);
    void      delObject(vpzExpCondValue *obj);
    bool      getBoolean();
    void      setBoolean(bool b);
    double    getDouble();
    void      setDouble(double v);
    int       getInteger();
    void      setInteger(int i);
    QString   getString();
    void      setString(QString text);

    // Accessors for complex types
    vpzExpCondValue *getMapLast();
    QList <vpzExpCondValue *> *getMap()
    {
        return &mMap;
    }
    QList <vpzExpCondValue *> *getMatrix();
    void      getMatrixSize (int *row, int *col);
    void      setMatrixSize (int  row, int  col);
    QList <vpzExpCondValue *> *getSet()
    {
        return &mSet;
    }
    QList <vpzExpCondValue *> *getTable();
    void      setTableSize (int  row, int  col);
    void      getTableSize (int *row, int *col);
    void      setTableIndex(int  row, int  col);
    void      getTableIndex(int *row, int *col);
    QList <vpzExpCondValue *> *getTuple();
    int       getTupleIndex();
    void      setTupleIndex(int idx);
    QString   getXml();
    void      setXml(QString xml);

public:
    void    xLoadTable(const QDomNode *node);
    bool    xSave(QDomDocument *doc, QDomElement *baseNode, vpzExpCondValue *value = 0);

public: // For debug only
    QString   mTmpName;
private:
    vpzExpCondValue *mParent;
    ValueType  mType;
    QString    mName;
    bool       mBoolean;
    double     mDouble;
    int        mInteger;
    QString    mString;
    int        mSizeRow;
    int        mSizeCol;
    QList <vpzExpCondValue *> mMap;
    QList <vpzExpCondValue *> mMatrix;
    QList <vpzExpCondValue *> mSet;
    QList <vpzExpCondValue *> mTable;
    QList <vpzExpCondValue *> mTuple;
    int        mTableRow;
    int        mTableCol;
    int        mTupleIndex;
    QString    mXml;
};

class vpzExpCondPort
{
public:
    vpzExpCondPort(vpzExpCond *parent = 0);
    vpzExpCondPort(vpzExpCond *parent, vpzExpCondPort *source);
    ~vpzExpCondPort();
    QString getName();
    void    setName(QString name);
    vpzExpCond *getParent();
    void    setParent(vpzExpCond *parent);
    void    xLoadNode(const QDomNode &baseNode, vpzExpCondValue *baseValue = 0);
    void    xLoadMatrix(QDomNode *baseNode, vpzExpCondValue *baseValue = 0);
    bool    xSave(QDomDocument *doc, QDomElement *baseNode);
    vpzExpCondValue* createValue(vpzExpCondValue::ValueType type);
    bool    removeValue(vpzExpCondValue *value);
    vpzExpCondValue *getValue();
    QList <vpzExpCondValue *> *getValues()
    {
        return &mValues;
    }
    vpzExpCondValue *getNextValue();
private:
    vpzExpCond *mParent;
    QString     mName;
    QList <vpzExpCondValue *> mValues;
    int         mValueIndex;
};

class vpzExpCond
{
public:
    vpzExpCond(vleVpz *vpz = 0);
    ~vpzExpCond();
    void    copyFromExpCond(vpzExpCond *source);
    QString getName();
    void    setName(QString name);
    void    xLoadNode(const QDomNode &node);
    void    addPort(vpzExpCondPort *newPort);
    bool    removePort(vpzExpCondPort *port);
    vpzExpCondPort *getPort(QString name);
    QList <vpzExpCondPort *> *getPorts()
    {
        return &mPorts;
    }
    void registerModel(vleVpzModel *model);
    void unRegisterModel(vleVpzModel *model);
    bool    hasPlugin();
    void    setPlugin(QString name);
    QString getPlugin();

private:
    QString mName;
    QList <vpzExpCondPort *> mPorts;
    QList <vleVpzModel *>    mLinkedModels;
    QString                  mPluginName;
};

#endif // VPZEXPCOND_H
