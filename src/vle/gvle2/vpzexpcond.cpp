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

#include <QString>
#include <QDebug>
#include "vpzexpcond.h"
#include "vlevpz.h"

vpzExpCond::vpzExpCond(vleVpz *vpz)
{
    (void)vpz;

    mPluginName.clear();
}

vpzExpCond::~vpzExpCond()
{
    while (mLinkedModels.length())
    {
        vleVpzModel *model = mLinkedModels.takeLast();
        model->removeCondition(this);
    }

    while (mPorts.length())
    {
        delete mPorts.takeLast();
    }
}

void vpzExpCond::copyFromExpCond(vpzExpCond *source)
{
    QListIterator<vpzExpCondPort*> ports(*source->getPorts());
    while( ports.hasNext() )
    {
        vpzExpCondPort *port = ports.next();

        vpzExpCondPort *newPort = new vpzExpCondPort();
        newPort->setName(port->getName());

        QListIterator<vpzExpCondValue *> values(*port->getValues());
        while( values.hasNext() )
        {
            vpzExpCondValue * value = values.next();
            if (value->type() != vpzExpCondValue::TypeDouble)
                continue;

            vpzExpCondValue *newValue;
            newValue = newPort->createValue(vpzExpCondValue::TypeDouble);
            newValue->setDouble( value->getDouble() );
        }

        addPort(newPort);
    }
}
QString vpzExpCond::getName()
{
    return mName;
}

void vpzExpCond::setName(QString name)
{
    mName = name;
}

void vpzExpCond::setPlugin(QString name)
{
    mPluginName = name;
}

bool vpzExpCond::hasPlugin()
{
    if (mPluginName.isEmpty())
        return false;
    return true;
}

QString vpzExpCond::getPlugin()
{
    return mPluginName;
}

void vpzExpCond::addPort(vpzExpCondPort *newPort)
{
    newPort->setParent(this);
    mPorts.append(newPort);
}

vpzExpCondPort *vpzExpCond::getPort(QString name)
{
    for (int i = 0; i < mPorts.length(); i++)
    {
        vpzExpCondPort *item = mPorts.at(i);
        if (item->getName() == name)
            return item;
    }
    return 0;
}

bool vpzExpCond::removePort(vpzExpCondPort *port)
{
    for (int i = 0; i < mPorts.length(); i++)
    {
        vpzExpCondPort *item = mPorts.at(i);
        if (item != port)
            continue;
        mPorts.removeAt(i);
        delete item;
        return true;
    }
    return false;
}

void vpzExpCond::registerModel (vleVpzModel *model)
{
    if (mLinkedModels.contains(model))
        return;

    mLinkedModels.append(model);
}
void vpzExpCond::unRegisterModel(vleVpzModel *model)
{
    mLinkedModels.removeOne(model);
}

// --------------------  --------------------

vpzExpCondPort::vpzExpCondPort(vpzExpCond *parent)
{
    mValueIndex = 0;
    mParent = parent;
}

vpzExpCondPort::vpzExpCondPort(vpzExpCond *parent, vpzExpCondPort *source)
{
    mValueIndex = 0;
    mParent = parent;

    if (source == 0)
        return;

    mName = source->getName();

    // ToDo : Copy ports
//    vpzExpCondValue * value;
//    for (value = source->getValue(); value != 0; value = source->getNextValue())
//    {
//        //
//    }
}

vpzExpCondPort::~vpzExpCondPort()
{
    while(mValues.length())
    {
        vpzExpCondValue *v = mValues.first();
        mValues.removeFirst();
        delete v;
    }
}

QString vpzExpCondPort::getName()
{
    return mName;
}

void vpzExpCondPort::setName(QString name)
{
    mName = name;
}

vpzExpCond *vpzExpCondPort::getParent()
{
    return mParent;
}

void vpzExpCondPort::setParent(vpzExpCond *parent)
{
    mParent = parent;
}

vpzExpCondValue* vpzExpCondPort::createValue(vpzExpCondValue::ValueType type)
{
    vpzExpCondValue *newVal = new vpzExpCondValue();
    newVal->setType(type);
    mValues.append(newVal);

    return newVal;
}

vpzExpCondValue* vpzExpCondPort::getValue()
{
    if (mValues.count() == 0)
        return 0;

    mValueIndex = 0;

    return mValues.at(0);
}

vpzExpCondValue *vpzExpCondPort::getNextValue()
{
    mValueIndex++;

    if (mValueIndex >= mValues.count())
        return 0;

    return mValues.at(mValueIndex);
}

bool vpzExpCondPort::removeValue(vpzExpCondValue *value)
{
    for (int i = 0; i < mValues.length(); i++)
    {
        vpzExpCondValue *selValue = mValues.at(i);
        if (selValue != value)
            continue;

        // Remove the value from the list ...
        mValues.removeAt(i);
        // ... and free memory
        delete value;

        return true;
    }
    return false;
}

void vpzExpCondPort::xLoadNode(const QDomNode &node, vpzExpCondValue *baseValue)
{
    QDomNodeList list = node.childNodes();
    for (unsigned int i=0; i < list.length(); i++)
    {
        QDomNode item = list.item(i);

        vpzExpCondValue *newVal = new vpzExpCondValue();

        if (item.nodeName() == "boolean")
        {
            QString nodeValue = item.firstChild().nodeValue();
            newVal->setType(vpzExpCondValue::TypeBoolean);
            if (nodeValue == "true")
                newVal->setBoolean(true);
            else if (nodeValue == "false")
                newVal->setBoolean(false);
            else
            {
                delete newVal;
                continue;
            }
        }
        else if (item.nodeName() == "double")
        {
            QString nodeValue = item.firstChild().nodeValue();
            newVal->setType(vpzExpCondValue::TypeDouble);
            newVal->setDouble(nodeValue.toDouble());
        }
        else if (item.nodeName() == "integer")
        {
            QString nodeValue = item.firstChild().nodeValue();
            newVal->setType(vpzExpCondValue::TypeInteger);
            newVal->setInteger(nodeValue.toInt());
        }
        else if (item.nodeName() == "null")
        {
            newVal->setType(vpzExpCondValue::TypeNull);
        }
        else if (item.nodeName() == "string")
        {
            newVal->setType(vpzExpCondValue::TypeString);
            newVal->setString(item.firstChild().nodeValue());
        }
        else if (item.nodeName() == "map")
        {
            newVal->setType(vpzExpCondValue::TypeMap);
            QDomNodeList keyList = item.childNodes();
            for (unsigned int j=0; j < keyList.length(); j++)
            {
                QDomNode kItem = keyList.item(j);

                // Get the key name from attributes
                QString kItemName;
                QDomNamedNodeMap kAttrMap = kItem.attributes();
                if (kAttrMap.contains("name"))
                    kItemName = kAttrMap.namedItem("name").nodeValue();

                // Load the map-key content usig recursive call
                xLoadNode(kItem, newVal);

                // Get the last inserted item into the map
                vpzExpCondValue *mapVal = newVal->getMapLast();
                mapVal->setName(kItemName);
            }
        }
        else if (item.nodeName() == "matrix")
        {
            xLoadMatrix(&item, newVal);
        }
        else if (item.nodeName() == "set")
        {
            newVal->setType(vpzExpCondValue::TypeSet);

            // Load the set content using recursive call
            xLoadNode(item, newVal);
        }
        else if (item.nodeName() == "table")
        {
            newVal->xLoadTable(&item);
        }
        else if (item.nodeName() == "tuple")
        {
            newVal->setType(vpzExpCondValue::TypeTuple);
            QString tuple = item.firstChild().nodeValue();
            QStringList tupleFields = tuple.split(" ");
            for (int j=0; j < tupleFields.length(); j++)
            {
                bool isOk;
                QString tfval = tupleFields.at(j);
                double dval = tfval.toDouble(&isOk);
                if (!isOk)
                    continue;
                vpzExpCondValue *tupleVal = new vpzExpCondValue(newVal);
                tupleVal->setType(vpzExpCondValue::TypeDouble);
                tupleVal->setTupleIndex(j + 1);
                tupleVal->setDouble(dval);
                newVal->addObject(tupleVal);
            }
        }
        else if (item.nodeName() == "xml")
        {
            newVal->setType(vpzExpCondValue::TypeXml);
            newVal->setXml(item.firstChild().nodeValue());
        }
        if (baseValue == 0)
            mValues.append(newVal);
        else
            baseValue->addObject(newVal);
    }
}

void vpzExpCondPort::xLoadMatrix(QDomNode *node, vpzExpCondValue *baseValue)
{
    if (node->nodeName() != "matrix")
        return;

    baseValue->setType(vpzExpCondValue::TypeMatrix);

    QDomNamedNodeMap itemAttrMap = node->attributes();
    if ( ! itemAttrMap.contains("rows"))
        return;
    if ( ! itemAttrMap.contains("columns"))
        return;

    int colCount, rowCount;
    colCount = itemAttrMap.namedItem("columns").nodeValue().toInt();
    rowCount = itemAttrMap.namedItem("rows").nodeValue().toInt();

    baseValue->setMatrixSize(rowCount, colCount);

    xLoadNode(*node, baseValue);
}


bool vpzExpCondPort::xSave(QDomDocument *doc, QDomElement *baseNode)
{
    baseNode->setAttribute("name", mName);

    for (int j = 0; j < mValues.length(); j++)
    {
        vpzExpCondValue *val = mValues.at(j);
        val->xSave(doc, baseNode);
    }
    return true;
}

// --------------------  --------------------

/**
 * @brief vpzExpCondValue::vpzExpCondValue
 *        Default constructor for Experimental Conditions value
 *
 */
vpzExpCondValue::vpzExpCondValue(vpzExpCondValue *parent)
{
    mType = TypeUnknown;
    mName = "";
    mParent = 0;

    mSizeRow = 0;
    mSizeCol = 0;

    mTupleIndex = 0;
    mTableRow   = 0;
    mTableCol   = 0;

    mBoolean = false;
    mDouble  = 0;
    mInteger = 0;
    mString  = "";
    mMap.clear();
    mSet.clear();
    mTable.clear();
    mTuple.clear();

    if (parent)
        mParent = parent;
}

/**
 * @brief vpzExpCondValue::~vpzExpCondValue
 *        Default destructor
 *
 */
vpzExpCondValue::~vpzExpCondValue()
{
    while(mMap.length())
    {
        vpzExpCondValue *mv = mMap.first();
        mMap.removeFirst();
        delete mv;
    }

    while(mSet.length())
    {
        vpzExpCondValue *mv = mSet.first();
        mSet.removeFirst();
        delete mv;
    }
    while(mTable.length())
    {
        vpzExpCondValue *mv = mTable.first();
        mTable.removeFirst();
        delete mv;
    }
    while(mTuple.length())
    {
        vpzExpCondValue *mv = mTuple.first();
        mTuple.removeFirst();
        delete mv;
    }
}

/**
 * @brief vpzExpCondValue::type
 *        Return the data type of this value
 *
 */
vpzExpCondValue::ValueType vpzExpCondValue::type()
{
    return mType;
}

/**
 * @brief vpzExpCondValue::setType
 *        Set the data type (must be set prior to data access)
 *
 */
void vpzExpCondValue::setType(ValueType type)
{
    mType = type;
}

/**
 * @brief vpzExpCondValue::getTypeName
 *        Return a human readable string that define the data type
 *
 */
QString vpzExpCondValue::getTypeName()
{
    QString typeName;
    if (mType == TypeBoolean)
        typeName = QObject::tr("boolean");
    else if (mType == TypeDouble)
        typeName = QObject::tr("double");
    else if (mType == TypeInteger)
        typeName = QObject::tr("integer");
    else if (mType == TypeNull)
        typeName = QObject::tr("null");
    else if (mType == TypeString)
        typeName = QObject::tr("string");
    else if (mType == TypeMap)
        typeName = QObject::tr("map");
    else if (mType == TypeMatrix)
        typeName = QObject::tr("matrix");
    else if (mType == TypeSet)
        typeName = QObject::tr("set");
    else if (mType == TypeTable)
        typeName = QObject::tr("table");
    else if (mType == TypeTuple)
        typeName = QObject::tr("tuple");
    else if (mType == TypeXml)
        typeName = QObject::tr("xml");

    return typeName;
}

/**
 * @brief vpzExpCondValue::isAtomic
 *        Test if
 *
 */
bool vpzExpCondValue::isAtomic()
{
    if (mType < TypeComplex)
        return true;
    return false;
}

/**
 * @brief vpzExpCondValue::getName
 *        Return the name of the value (for types who provide it)
 *
 */
QString vpzExpCondValue::getName()
{
    return mName;
}

/**
 * @brief vpzExpCondValue::getDisplayName
 *        Return a human readable string that define the value (type and name)
 *
 */
QString vpzExpCondValue::getDisplayName()
{
    QString objectName;

    if (mName != "")
        objectName = QString("%1 (%2)").arg(mName).arg(getTypeName());
    else
    {
        if (mParent)
        {
            if (mParent->type() == TypeMatrix)
            {
                objectName = QString("%1")
                                 .arg(getTypeName());
            }
            if (mParent->type() == TypeTuple)
                objectName = QString("tuple(%1)").arg(mTupleIndex);
            if (mParent->type() == TypeTable)
                objectName = QString("table(%1:%2)").arg(mTableCol).arg(mTableRow);
        }
        else
            objectName = getTypeName();
    }

    return objectName;
}

/**
 * @brief vpzExpCondValue::getDisplayValue
 *        Return a human readable string that conatain the value
 *
 */
QString vpzExpCondValue::getDisplayValue()
{
    QString value;

    if (mType == TypeBoolean)
    {
        if (getBoolean())
            value = QObject::tr("true");
        else
            value = QObject::tr("false");
    }
    else if (mType == TypeDouble)
        value = QString("%1").arg(getDouble());
    else if (mType == TypeInteger)
        value = QString("%1").arg(getInteger());
    else if (mType == TypeNull)
        value = QObject::tr("null");
    else if (mType == TypeString)
        value = getString();
    else
        value = QString();

    return value;
}

/**
 * @brief vpzExpCondValue::setName
 *        Set the name of the current value
 *
 */
void vpzExpCondValue::setName(QString name)
{
    mName = name;
}

/**
 * @brief vpzExpCondValue::addObject
 *        Insert a subitem of the current value
 *
 */
void vpzExpCondValue::addObject(vpzExpCondValue *obj)
{
    if (mType == TypeMap)
        mMap.append(obj);
    else if (mType == TypeMatrix)
        mMatrix.append(obj);
    else if (mType == TypeSet)
        mSet.append(obj);
    else if (mType == TypeTable)
        mTable.append(obj);
    else if (mType == TypeTuple)
        mTuple.append(obj);
}

/**
 * @brief vpzExpCondValue::delObject
 *        Remove a subitem of the current (complex) value
 *
 */
void vpzExpCondValue::delObject(vpzExpCondValue *obj)
{
    QList <vpzExpCondValue *> *objList = 0;

    // Get the value list according to type
    if (mType == TypeMap)
        objList = getMap();
    else if (mType == TypeSet)
        objList = getSet();
    else if (mType == TypeTuple)
        objList = getTuple();

    if (objList == 0)
        return;

    // Search the position of element to remove
    for (int i = 0; i < objList->length(); i++)
    {
        if (objList->at(i) == obj)
        {
            objList->removeAt(i);
            break;
        }
    }
}

bool vpzExpCondValue::getBoolean()
{
    if (mType != TypeBoolean)
    {
        qDebug() << "vpzExpCondValue::getBoolean() to a non-bool object";
        return false;
    }
    return mBoolean;
}

void vpzExpCondValue::setBoolean(bool b)
{
    if (mType != TypeBoolean)
    {
        qDebug() << "vpzExpCondValue::setBoolean() to a non-bool object";
        return;
    }
    mBoolean = b;
}

QString vpzExpCondValue::getString()
{
    if (mType != TypeString)
    {
        qDebug() << "vpzExpCondValue::getString() to a non-string object";
        return QString();
    }
    return mString;
}

void vpzExpCondValue::setString(QString text)
{
    if (mType != TypeString)
    {
        qDebug() << "vpzExpCondValue::setString() to a non-string object";
        return;
    }
    mString = text;
}

double vpzExpCondValue::getDouble()
{
    if (mType != TypeDouble)
    {
        qDebug() << "vpzExpCondValue::getDouble() to a non-double object";
        return 0;
    }
    return mDouble;
}

void vpzExpCondValue::setDouble(double v)
{
    if (mType != TypeDouble)
    {
        qDebug() << "vpzExpCondValue::setDouble() to a non-double object";
        return;
    }
    mDouble = v;
}

int vpzExpCondValue::getInteger()
{
    if (mType != TypeInteger)
    {
        qDebug() << "vpzExpCondValue::getInteger() to a non-int object";
        return 0;
    }
    return mInteger;
}

void vpzExpCondValue::setInteger(int i)
{
    if (mType != TypeInteger)
    {
        qDebug() << "vpzExpCondValue::setInteger() to a non-int object";
        return;
    }
    mInteger = i;
}

vpzExpCondValue *vpzExpCondValue::getMapLast()
{
    if (mType != TypeMap)
    {
        qDebug() << "vpzExpCondValue::getMapLast() to a non-map object";
        return 0;
    }

    if (mMap.isEmpty())
        return 0;
    return mMap.last();
}

QList <vpzExpCondValue *> *vpzExpCondValue::getMatrix()
{
    if (mType != TypeMatrix)
    {
        qDebug() << "vpzExpCondValue::getMatrix() to a non-matrix object";
        return 0;
    }

    return &mMatrix;
}

void vpzExpCondValue::getMatrixSize (int *row, int *col)
{
    if (row != 0)
        *row = mSizeRow;

    if (col != 0)
        *col = mSizeCol;
}

void vpzExpCondValue::setMatrixSize (int  row, int  col)
{
    mSizeRow = row;
    mSizeCol = col;
}

QList <vpzExpCondValue *> *vpzExpCondValue::getTable()
{
    return &mTable;
}

void vpzExpCondValue::setTableSize (int row, int col)
{
    mSizeRow = row;
    mSizeCol = col;
}

void vpzExpCondValue::getTableSize(int *row, int *col)
{
    if (row != 0)
        *row = mSizeRow;

    if (col != 0)
        *col = mSizeCol;
}

void vpzExpCondValue::setTableIndex(int row, int col)
{
    mTableRow = row;
    mTableCol = col;
}

void vpzExpCondValue::getTableIndex(int *row, int *col)
{
    if (row != 0)
        *row = mTableRow;

    if (col != 0)
        *col = mTableCol;
}

QList <vpzExpCondValue *> *vpzExpCondValue::getTuple()
{
    return &mTuple;
}

int  vpzExpCondValue::getTupleIndex()
{
    if (mType != TypeTuple)
        return -1;
    return mTupleIndex;
}

void vpzExpCondValue::setTupleIndex(int idx)
{
    mTupleIndex = idx;
}

QString vpzExpCondValue::getXml()
{
    if (mType != TypeXml)
    {
        qDebug() << "vpzExpCondValue::getXml() to a non-xml object";
        return QString();
    }
    return mXml;
}

void vpzExpCondValue::setXml(QString xml)
{
    if (mType != TypeXml)
    {
        qDebug() << "vpzExpCondValue::setXml() to a non-xml object";
        return;
    }
    mXml = xml;
}

void vpzExpCondValue::xLoadTable(const QDomNode *node)
{
    if (node->nodeName() != "table")
        return;

    setType(vpzExpCondValue::TypeTable);

    int width, height;
    QDomNamedNodeMap itemAttrMap = node->attributes();
    if ( ! itemAttrMap.contains("width"))
        return;
    if ( ! itemAttrMap.contains("height"))
        return;

    width = itemAttrMap.namedItem("width").nodeValue().toInt();
    height = itemAttrMap.namedItem("height").nodeValue().toInt();

    setTableSize(height, width);

    QString flatTable = node->firstChild().nodeValue();

    QStringList tableList = flatTable.split(" ");

    for(int i = 0; i < height; i++)
    {
        int startIdx = (i * width);
        for (int j = 0; j < width; j++)
        {
            if (tableList.length() < startIdx+j)
                break;
            QString v = tableList.at(startIdx+j);
            vpzExpCondValue *subVal = new vpzExpCondValue(this);
            subVal->setType(vpzExpCondValue::TypeDouble);
            subVal->setTableIndex(i, j);
            subVal->setDouble(v.toDouble());
            addObject(subVal);
        }
    }
}

bool vpzExpCondValue::xSave(QDomDocument *doc, QDomElement *baseNode, vpzExpCondValue *value)
{
    bool addContent;
    QString content;
    QDomText xContent;

    if (value == 0)
        value = this;

    // Create a tag based on type
    QDomElement xEntry = doc->createElement( value->getTypeName() );

    addContent = false;
    switch (value->type())
    {
        case TypeBoolean:
        {
            if (value->getBoolean())
                content = "true";
            else
                content = "false";
            addContent = true;
            break;
        }
        case TypeDouble:
        {
            double d = value->getDouble();
            content = QString("%1").arg(d, 0, 'f', 15);
            addContent = true;
            break;
        }
        case TypeInteger:
        {
            int i = value->getInteger();
            content = QString("%1").arg(i);
            addContent = true;
            break;
        }
        case TypeNull:
        {
            // Nothing to do :)
            break;
        }
        case TypeString:
        {
            content = value->getString();
            addContent = true;
            break;
        }

        case TypeComplex:
        {
            break;
        }
        case TypeMap:
        {
            QList <vpzExpCondValue *> *mapList = value->getMap();
            for (int i = 0; i < mapList->length(); i++)
            {
                vpzExpCondValue *mapValue = mapList->at(i);

                QDomElement xKey = doc->createElement("key");
                xKey.setAttribute("name", mapValue->getName());

                xSave(doc, &xKey, mapValue);

                xEntry.appendChild(xKey);
            }
            break;
        }
        case TypeMatrix:
        {
            int rows, cols;
            value->getMatrixSize(&rows, &cols);
            xEntry.setAttribute("rows",    rows);
            xEntry.setAttribute("columns", cols);
            xEntry.setAttribute("columnmax", cols);
            xEntry.setAttribute("rowmax",    rows);
            xEntry.setAttribute("columnstep", "0");
            xEntry.setAttribute("rowstep",    "0");

            QList <vpzExpCondValue *> *matrixList = value->getMatrix();
            for (int i = 0; i < matrixList->length(); i++)
            {
                vpzExpCondValue *matrixValue = matrixList->at(i);
                xSave(doc, &xEntry, matrixValue);
            }
            break;
        }
        case TypeSet:
        {
            QList <vpzExpCondValue *> *setList = value->getSet();
            for (int i = 0; i < setList->length(); i++)
            {
                vpzExpCondValue *setValue = setList->at(i);
                xSave(doc, &xEntry, setValue);
            }
            break;
        }
        case TypeTable:
        {
            content = "";
            QList <vpzExpCondValue *> *tableList = value->getTable();

            int rows, cols;
            value->getTableSize(&rows, &cols);
            xEntry.setAttribute("width",  cols);
            xEntry.setAttribute("height", rows);

            for (int i = 0; i < tableList->length(); i++)
            {
                vpzExpCondValue *tableValue = tableList->at(i);
                if (i)
                    content.append(" ");
                double d = tableValue->getDouble();
                content.append( QString("%1").arg(d, 0, 'f', 15) );
                addContent = true;
            }
            break;
        }
        case TypeTuple:
        {
            content = "";
            QList <vpzExpCondValue *> *tupleList = value->getTuple();
            for (int i = 0; i < tupleList->length(); i++)
            {
                vpzExpCondValue *tupleValue = tupleList->at(i);
                if (i)
                    content.append(" ");
                double d = tupleValue->getDouble();
                content.append( QString("%1").arg(d, 0, 'f', 15) );
                addContent = true;
            }
            break;
        }
        case TypeXml:
        {
            content = value->getXml();
            addContent = true;
            break;
        }

        case TypeUnknown:
            break;
    }

    if (addContent)
    {
        xContent = doc->createTextNode(content);
        xEntry.appendChild(xContent);
    }

    baseNode->appendChild(xEntry);
    return true;
}
