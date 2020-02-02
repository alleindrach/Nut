/**************************************************************************
**
** This file is part of Nut project.
** https://github.com/HamedMasafi/Nut
**
** Nut is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Nut is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Nut.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QDebug>

#include <QJsonArray>
#include <QJsonObject>

#include "tablemodel.h"
#include "defines.h"
#include "table.h"
NUT_BEGIN_NAMESPACE

QString TableModel::name() const
{
    return _name;
}

void TableModel::setName(const QString &name)
{
    _name = name;
}

QString TableModel::className() const
{
    return _className;
}

void TableModel::setClassName(const QString &className)
{
    _className = className;
}

int TableModel::typeId() const
{
    return _typeId;
}

void TableModel::setTypeId(const int &typeId)
{
    _typeId = typeId;
}

int TableModel::refType() const
{
    return _refType;
}

void TableModel::setRefType(const int &refType)
{
    _refType = refType;
}

FieldModel *TableModel::field(int n) const
{
    if (n < 0 || n >= _fields.count())
        return nullptr;

    return _fields.at(n);
}

FieldModel *TableModel::field(const QString &name) const
{
    foreach (FieldModel *f, _fields)
        if(f->name == name)
            return f;
    
    return nullptr;
}


QList<FieldModel *> TableModel::fields() const
{
    return _fields;
}
QMap<QString ,QList<FieldModel*>> TableModel::catalogs() const{
    return _catFields;
}
QList<FieldModel *> TableModel::fieldsByCat(QString cat) const
{
    QList<FieldModel * > catFields;
    foreach(FieldModel * field,_fields){
        if(field->catalog==cat){
            catFields.append(field);
        }
    }
    std::sort(catFields.begin(),catFields.end());
    return catFields;
}


QList<RelationModel *> TableModel::foreignKeys() const
{
    return _foreignKeys;
}

QList<LibraryModel *> TableModel::libraryRefs() const{
    return _libraryRefs;
}

QStringList TableModel::fieldsNames() const
{
    QStringList ret;
    foreach (FieldModel *f, _fields)
        ret.append(f->name);
    return ret;
}

bool TableModel::operator ==(const TableModel &t) const{
    if(_name != t.name())
        return false;

    if(fields().count() != t.fields().count())
        return false;

    foreach (FieldModel *f, _fields) {
        FieldModel *tf = t.field(f->name);
        if(!tf)
            return false;

        if(*f != *tf)
            return false;
    }

    return true;
}

bool TableModel::operator !=(const TableModel &t) const
{
    return !(*this == t);
}

TableModel::TableModel(int typeId, const QString &tableName)
{
    //TODO: check that
    //    if  (findByTypeId(typeId))
    //        return;

    const QMetaObject *tableMetaObject = QMetaType::metaObjectForType(typeId);

    _typeId = typeId;
    _name = tableName;
    _className = tableMetaObject->className();

    //#ifdef NUT_NAMESPACE
    //    if(_className.startsWith(QT_STRINGIFY(NUT_NAMESPACE) "::"))
    //        _className = _className.replace(QT_STRINGIFY(NUT_NAMESPACE) "::", "");
    //#endif

    // get fields names
    for(int j = 0; j < tableMetaObject->classInfoCount(); j++){
        QString type;
        QString name;
        QString value;

        if (!nutClassInfoString(tableMetaObject->classInfo(j),
                                type, name, value)) {
            continue;
        }

        if(type == __nut_FIELD){
            auto *f = new FieldModel;
            f->name = f->displayName = name;
            _fields.append(f);
        }
        if(type== __nut_CATALOG){
            QList<FieldModel*> catList=_catFields.value(value);
            _catFields.insert(value,catList);
        }
    }

    // Browse all fields
    for(int j = 1; j < tableMetaObject->propertyCount(); j++){
        QMetaProperty fieldProperty = tableMetaObject->property(j);

        FieldModel *fieldObj = field(fieldProperty.name());
        foreach (FieldModel *f, _fields)
            if(f->name == fieldProperty.name())
                f = fieldObj;
        if(!fieldObj)
            continue;
        fieldObj->type = static_cast<QMetaType::Type>(fieldProperty.type());
        fieldObj->typeName = QString(fieldProperty.typeName());
    }

    // Browse class infos
    for(int j = 0; j < tableMetaObject->classInfoCount(); j++){
        QString type;
        QString name;
        QString value;

        if (!nutClassInfoString(tableMetaObject->classInfo(j),
                                type, name, value)) {
            continue;
        }

        if(type == __nut_FOREIGN_KEY){
            auto *fk = new RelationModel;
            fk->slaveTable = this;
            fk->localColumn = name + "Id";
            fk->localProperty = name;
            fk->foreignColumn = value;
            fk->masterClassName = value;
            _foreignKeys.append(fk);
        }

        if(type == __nut_FIELD){

        }


        FieldModel *f = field(name);
        if (!f)
            continue;

        if (type == __nut_LEN)
            f->length = value.toInt();
        else if (type == __nut_NOT_NULL)
            f->notNull = true;
        else if (type == __nut_DEFAULT_VALUE)
            f->defaultValue = value;
        else if (type == __nut_PRIMARY_KEY)
            f->isPrimaryKey = true;
        else if (type == __nut_AUTO_INCREMENT)
            f->isAutoIncrement = true;
        else if (type == __nut_UNIQUE)
            f->isUnique = true;
        else if (type == __nut_DISPLAY)
            f->displayName =QString( QObject::tr( value.toUtf8().data()));
        else if (type == __nut_PRIMARY_KEY_AI) {
            f->isPrimaryKey = true;
            f->isAutoIncrement = true;
        }else if(type == __nut_CATALOG_INDEX){
            f->catIndex=value.toInt();
        }else if(type == __nut_CATALOG){
            f->catalog=value;
            QList<FieldModel*> catList=_catFields.value(value);
            catList.insert(f->catIndex,f);
            _catFields.insert(value,catList);
        }else if(type == __nut_CAL_EXPRESSION){
            f->calExpress=value;
        }else if(type == __nut_INPUT_TYPE){
            QMetaEnum metaEnum = QMetaEnum::fromType<Table::INPUT_TYPE>();
            f->inputType=metaEnum.keyToValue(value.toLatin1().data());
        }else if(type == __nut_DATE_SPAN_INDEX){
            f->inputDateSpanIndex=value.toInt();
        }else if(type == __nut_DATE_SPAN_POS){
            f->inputDateSpanPos=value.toInt();
        }else if(type == __nut_INPUT_OPTIONS){
            // [{"display":"a","value":1},{"display":"b","value":2}]
            QJsonValue json=QJsonValue(value);
            QJsonArray jsonInputOptions=json.toArray();
            foreach(QVariant var, jsonInputOptions.toVariantList()){
                QJsonObject opt=var.toJsonObject();
                QString k=opt.take("display").toString();
                QVariant v=opt.take("value");
                f->inputOptions.append(InputOption(k,v));
            }
        }else if(type==__nut_LIBREF_NAME){
            LibraryModel * libRef=nullptr;
            foreach(LibraryModel * lib,_libraryRefs){
                if(lib->librefName()==value){
                    libRef=lib;
                    break;
                }
            }
            if(!libRef){
                LibraryModel * libmodelRef=new LibraryModel();
                libmodelRef->setLibrefName(value);
                libRef=libmodelRef;
                _libraryRefs.append(libmodelRef);
            }
            f->libref=libRef;
        }else if(type==__nut_LIBREF_TABLE_NAME)
        {
            if(f->libref->masterClassName().isEmpty())
                f->libref->setMasterClassName(value);
        }else if(type==__nut_LIBREF_TABLE_FIELD_NAME)
        {
            LibraryModel * ref=f->libref;
            QStringList list=ref->refColumns();
            list.append(value);
            ref->setRefColumns(list);

            list=ref->localColumns();
            list.append(f->name);
            ref->setLocalColumns(list);

            list=ref->localProperties();
            list.append(f->name);
            ref->setLocalProperties(list);
        }
        else if(type == __nut_LIBREF_KEY){
            QJsonObject jso=QJsonObject(QJsonValue(value).toObject());
            LibraryModel* libmodelRef=new LibraryModel(jso);
            this->_libraryRefs.append(libmodelRef);
        }

    }
}

/*
 "comment": {
        "auto_increment": "id",
        "fields": {
            "id": {
                "name": "id",
                "type": "int"
            },
            "userId": {
                "name": "userId",
                "type": "int"
            }
        },
        "primary_key": "id",
        "libref_keys":{
            "postlibref":{
                "name":"postlibref",
                "localColumns":["post_user_name","post_date"],
                "localProperties":["postUserName","postDate"],
                "masterClassName":"Post"
                "refColumns":["user_name","date"]
            }
        },
        "lforeign_keys":{
            "postid":{
                "localColumn":"postid",
                "localProperty":"postid",
                "masterClassName": "Post",
                "foreignColumn":"id"
            }
        }
    },
*/

TableModel::TableModel(const QJsonObject &json, const QString &tableName) : _typeId(0)
{
    _name = tableName;

    QJsonObject fields = json.value(__FIELDS).toObject();
    QJsonObject relations = json.value(__FOREIGN_KEYS).toObject();
    QJsonObject libRefs = json.value(__LIBREF_KEYS).toObject();
    foreach (QString key, fields.keys()) {
        QJsonObject fieldObject = fields.value(key).toObject();
        //TODO: use FieldModel(QJsonObject) ctor
        auto *f = new FieldModel;
        f->name = fieldObject.value(__NAME).toString();
        f->type = static_cast<QMetaType::Type>(QMetaType::type(fieldObject.value(__TYPE).toString().toLatin1().data()));
        f->typeName = QMetaType::typeName(f->type);

        if(fieldObject.contains(__nut_NOT_NULL))
            f->notNull = fieldObject.value(__nut_NOT_NULL).toBool();

        if(fieldObject.contains(__nut_LEN))
            f->length = fieldObject.value(__nut_LEN).toInt();

        if(fieldObject.contains(__nut_DEFAULT_VALUE))
            f->defaultValue = fieldObject.value(__nut_DEFAULT_VALUE).toString();
        _fields.append(f);
    }

    foreach (QString key, relations.keys()) {
        QJsonObject relObject = relations.value(key).toObject();
        _foreignKeys.append(new RelationModel(relObject));
    }
    foreach (QString key, libRefs.keys()) {
        QJsonObject relObject = libRefs.value(key).toObject();
        _libraryRefs.append(new LibraryModel(relObject));
    }
}

TableModel::~TableModel()
{
    qDeleteAll(_fields);
    qDeleteAll(_foreignKeys);
    qDeleteAll(_libraryRefs);

    //    qDeleteAll(_catFields);
}

QJsonObject TableModel::toJson() const
{
    QJsonObject obj;
    QJsonObject fieldsObj;
    QJsonObject foreignKeysObj;
    QJsonObject librefKeysObj;
    foreach (FieldModel *f, _fields) {
        QJsonObject fieldObj;
        fieldObj.insert(__NAME, f->name);
        fieldObj.insert(__TYPE, QString(QVariant::typeToName(f->type)));

        if(f->length)
            fieldObj.insert(__nut_LEN, f->length);

        if(f->notNull)
            fieldObj.insert(__nut_NOT_NULL, f->notNull);

        if(!f->defaultValue.isNull())
            fieldObj.insert(__nut_DEFAULT_VALUE, f->defaultValue);

        if(f->isAutoIncrement)
            obj.insert(__nut_AUTO_INCREMENT, f->name);

        if(f->isPrimaryKey)
            obj.insert(__nut_PRIMARY_KEY, f->name);

        fieldsObj.insert(f->name, fieldObj);
    }
    foreach (RelationModel *rel, _foreignKeys)
        foreignKeysObj.insert(rel->localColumn, rel->toJson());
    foreach (LibraryModel * lml,_libraryRefs)
    {
        librefKeysObj.insert(lml->librefName(),lml->toJson());
    }
    obj.insert(__FIELDS, fieldsObj);
    obj.insert(__FOREIGN_KEYS, foreignKeysObj);
    obj.insert(__LIBREF_KEYS,librefKeysObj);

    return obj;
}

RelationModel *TableModel::foreignKey(const QString &otherTable) const
{
    foreach (RelationModel *fk, _foreignKeys)
        if(fk->masterClassName == otherTable)
            return fk;

    return nullptr;
}

RelationModel *TableModel::foreignKeyByField(const QString &fieldName) const
{
    foreach (RelationModel *fk, _foreignKeys)
        if(fk->localColumn == fieldName)
            return fk;

    return nullptr;
}

LibraryModel * TableModel::libraryRef(const QString &otherTable) const
{
    foreach (LibraryModel *lm, _libraryRefs)
        if(lm->masterClassName() == otherTable)
            return lm;

    return nullptr;
}
LibraryModel *TableModel::libraryRefByName(const QString &librefName) const{
    foreach (LibraryModel *lm, _libraryRefs)
    {
        if(lm->librefName()==librefName){
            return lm;
        }
    }
    return nullptr;
}
LibraryModel *TableModel::libraryRefByField(const QStringList &refFieldNames) const
{
    foreach (LibraryModel *lm, _libraryRefs)
    {
        QString key=lm->localColumnsKey();
        QStringList tmpRefFieldNames=refFieldNames;
        tmpRefFieldNames.sort();
        QString keyRef=tmpRefFieldNames.join("_");
        if(keyRef == key)
            return lm;
    }
    return nullptr;
}

QString TableModel::toString() const
{
    QStringList sl;
    foreach (FieldModel *f, _fields)
        sl.append(f->name + " " + QVariant::typeToName(f->type));

    QString ret = QString("%1 (%2)")
            .arg(_name, sl.join(", "));
    return ret;
}

QString TableModel::primaryKey() const
{
    foreach (FieldModel *f, _fields)
        if(f->isPrimaryKey)
            return f->name;
    return QString();
}

bool TableModel::isPrimaryKeyAutoIncrement() const
{
    FieldModel *pk = field(primaryKey());
    if (!pk)
        return false;
    return pk->isAutoIncrement;
}

FieldModel::FieldModel(const QJsonObject &json)
{
    name = json.value(__NAME).toString();
    type = static_cast<QMetaType::Type>(json.value(__TYPE).toInt());
    length = json.value(__nut_LEN).toInt();
    notNull = json.value(__nut_NOT_NULL).toBool();
    isAutoIncrement = json.value(__nut_AUTO_INCREMENT).toBool();
    isPrimaryKey = json.value(__nut_PRIMARY_KEY).toBool();
    defaultValue = json.value(__nut_DEFAULT_VALUE).toString();
    isUnique = json.value(__nut_UNIQUE).toBool();
    catalog=json.value(__nut_CATALOG).toString();

    calExpress=json.value(__nut_CAL_EXPRESSION).toString();
    inputType=json.value(__nut_INPUT_TYPE).toInt();
    inputDateSpanIndex=json.value(__nut_DATE_SPAN_INDEX).toInt();
    inputDateSpanPos=json.value(__nut_DATE_SPAN_POS).toInt();
    calExpress=json.value(__nut_CAL_EXPRESSION).toString();
    QJsonArray jsoArray=json.value(__nut_INPUT_OPTIONS).toArray();
    foreach(QVariant v,jsoArray.toVariantList()){
        QJsonObject jio=(QJsonValue::fromVariant(v)).toObject();
        InputOption io=InputOption::fromJson(jio);
        this->inputOptions.append(io);
    }
}

QJsonObject FieldModel::toJson() const
{
    QJsonObject fieldObj;
    fieldObj.insert(__NAME, name);
    fieldObj.insert(__TYPE, QString(QVariant::typeToName(type)));
    fieldObj.insert(__nut_LEN, length);
    fieldObj.insert(__nut_NOT_NULL, notNull);
    fieldObj.insert(__nut_AUTO_INCREMENT, isAutoIncrement);
    fieldObj.insert(__nut_PRIMARY_KEY, isPrimaryKey);
    fieldObj.insert(__nut_DEFAULT_VALUE, defaultValue);

    fieldObj.insert(__nut_CATALOG,catalog);
    fieldObj.insert(__nut_CAL_EXPRESSION,calExpress);
    fieldObj.insert(__nut_INPUT_TYPE,inputType);
    fieldObj.insert(__nut_DATE_SPAN_INDEX,this->inputDateSpanIndex);
    fieldObj.insert(__nut_DATE_SPAN_INDEX,this->inputDateSpanPos);
    {
        QJsonArray jsoArray;
        foreach(InputOption io,this->inputOptions){
            jsoArray.append(io.toJson());
        }
        fieldObj.insert(__nut_INPUT_OPTIONS,jsoArray);
    }

    return fieldObj;
}

RelationModel::RelationModel(const QJsonObject &obj)
{
    localColumn = obj.value("localColumn").toString();
    localProperty = obj.value("localProperty").toString();
    masterClassName = obj.value("masterClassName").toString();
    foreignColumn = obj.value("foreignColumn").toString();
    slaveTable = masterTable = nullptr;
}

QJsonObject RelationModel::toJson() const
{
    QJsonObject o;
    o.insert("localColumn", localColumn);
    o.insert("localProperty", localProperty);
    o.insert("masterClassName", masterClassName);
    o.insert("foreignColumn", foreignColumn);
    return o;
}

bool operator ==(const RelationModel &l, const RelationModel &r)
{
    return r.foreignColumn == l.foreignColumn
            && r.localColumn == l.localColumn
            && r.localProperty == l.localColumn
            && r.masterClassName == l.masterClassName;
}

bool operator !=(const RelationModel &l, const RelationModel &r)
{
    return !(l == r);
}


NUT_END_NAMESPACE
