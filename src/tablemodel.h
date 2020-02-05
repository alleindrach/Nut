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

#ifndef TABLESCHEEMA_H
#define TABLESCHEEMA_H

#include <QtCore/QVariant>
#include <QDebug>
#include "defines.h"
#include "QJsonObject"
#include "librarymodel.h"
class QJsonObject;

NUT_BEGIN_NAMESPACE

class TableModel;
struct FieldModel;
struct Catalog{
    QString displayName;
    QList<FieldModel*> fields;
};
struct InputOption{
    QString displayName;
    QVariant value;
    explicit InputOption(QString d,QVariant v):displayName(d),value(v){

    }
    QJsonObject toJson() const{
        QJsonObject jso;
        jso.insert("display",this->displayName);
        jso.insert("value",QJsonValue::fromVariant(this->value));
        return jso;
    }
    static InputOption fromJson(QJsonObject jso)
    {
        return InputOption(jso.take("display").toString(),jso.take("value"));
    }
};

struct FieldModel{
    explicit FieldModel() : name(QString()), defaultValue(QString())
    {

    }

    explicit FieldModel(const QJsonObject &json);

    QString name;
    QMetaType::Type type;
    QString typeName;
    int length{0};
    QString defaultValue;
    bool notNull{false};
    bool isPrimaryKey{false};
    bool isAutoIncrement{false};
    bool isUnique{false};
    QString displayName;
    QString tableClassName;
    bool operator ==(const FieldModel &f) const{

        bool b = name.toLower() == f.name.toLower()
                && type == f.type
                && length == f.length
                && defaultValue == f.defaultValue
                && notNull == f.notNull;

        return b;
    }

    bool operator !=(const FieldModel &f) const{
        return !(*this == f);
    }

    QJsonObject toJson() const;
//alleindrach
    QString catalog;
    int catIndex=0;
    QString calExpress;
    int inputType=0;
    int inputDateSpanIndex=0;
    int inputDateSpanPos=0;
    int reftype=0;//only one
    QList<InputOption>  inputOptions;
    LibraryModel * libref=nullptr;
    QString refField;
    QString  info;
};

struct RelationModel{
    RelationModel() : localColumn(QString()), localProperty(QString()),
        slaveTable(nullptr), foreignColumn(QString()), masterClassName(QString())
    {}
    explicit RelationModel(const QJsonObject &obj);

    //slave
    QString localColumn;
    QString localProperty;
    TableModel *slaveTable{nullptr};
    //master
    QString foreignColumn;
    TableModel *masterTable{nullptr};

    QString masterClassName;

    QJsonObject toJson() const;
};

bool operator ==(const RelationModel &l, const RelationModel &r);
bool operator !=(const RelationModel &l, const RelationModel &r);

class NUT_EXPORT TableModel
{
public:
    explicit TableModel(int typeId, const QString &tableName = QString());
    explicit TableModel(const QJsonObject &json, const QString &tableName);
    virtual ~TableModel();

    QJsonObject toJson() const;

    FieldModel *field(int n) const;
    FieldModel *field(const QString &name) const;
    RelationModel *foreignKey(const QString &otherTable) const;
    RelationModel *foreignKeyByField(const QString &fieldName) const;

    LibraryModel *libraryRef(const QString &otherTable) const;
    LibraryModel *libraryRefByField(const QString fieldName) const;
    LibraryModel *libraryRefByName(const QString &librefName) const;
    QString toString() const;

    QString primaryKey() const;
    bool isPrimaryKeyAutoIncrement() const;

    QString name() const;
    void setName(const QString &name);

    QString className() const;
    void setClassName(const QString &className);

    int typeId() const;
    void setTypeId(const int &typeId);
    QList<FieldModel *> fields() const;
    QList<FieldModel *> fieldsByCat(QString cat) const;
    QList<RelationModel *> foreignKeys() const;
    QList<LibraryModel *> libraryRefs() const;
    QStringList fieldsNames() const;
    QList<Catalog*> catalogs() const;
    bool operator ==(const TableModel &t) const;
    bool operator !=(const TableModel &t) const;

    int refType() const;
    void setRefType( const int& );
    Catalog * searchCatlog(QString  name);
    QString info();
    void  setInfo(QString info);
private:
    QString _name;
    QString _className;
    int _typeId;
    QList<FieldModel*> _fields;
    QList<RelationModel*> _foreignKeys;
    QList<LibraryModel*> _libraryRefs;
    int _refType;
    QList<Catalog*> _catlogs;
    QString _info;

};

NUT_END_NAMESPACE

#endif // TABLESCHEEMA_H
