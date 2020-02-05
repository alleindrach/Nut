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

#include <QMetaMethod>
#include <QVariant>
#include <QSqlQuery>
#include <QScriptEngine>
#include <QUuid>
#include "table.h"
#include "table_p.h"
#include "database.h"
#include "databasemodel.h"
#include "generators/sqlgeneratorbase_p.h"
#include "tablesetbase_p.h"

NUT_BEGIN_NAMESPACE

/*
 * FIXME:
 *  Qt can not access metaObject inside of constructor
 *  so, if we can't initalize myModel inside of ctor. in
 *  other side myModel inited in propertyChanged signal, so
 *  any method that uses myModel (like: primaryKey, ...) can't
 *  be accessed before any property set. So ugly, but there are
 *  no other way for now.
 *
 *  This should be fixed to v1.2
 */

Table::Table(QObject *parent) : QObject(parent),
    d(new TablePrivate)
{ }

Table::~Table()
{
    //Q_D(Table);

    //    if (d->parentTableSet)
    //        d->parentTableSet->remove(this);
}

void Table::add(TableSetBase *t)
{
    //Q_D(Table);
    d->childTableSets.insert(t);
}

//QString Table::primaryKey() const
//{
//    //Q_D(const Table);
//    return d->model->primaryKey();
//}

//bool Table::isPrimaryKeyAutoIncrement() const
//{
//    //Q_D(const Table);
//    FieldModel *pk = d->model->field(d->model->primaryKey());
//    if (!pk)
//        return false;
//    return pk->isAutoIncrement;
//}


//QVariant Table::primaryValue() const
//{
//    return property(primaryKey().toLatin1().data());
//}

void Table::propertyChanged(const QString &propName)
{
    //Q_D(Table);
    //    if (!d->model)
    //         d->model = TableModel::findByClassName(metaObject()->className());

    //    if (!d->model)
    //        qFatal ("model for class '%s' not found", qPrintable(metaObject()->className()));

    //    foreach (FieldModel *f, d->model->fields())
    //        if(f->isPrimaryKey && propName == f->name && f->isAutoIncrement)
    //            return;

    d.detach();
    d->changedProperties.insert(propName);
    if (d->status == FeatchedFromDB)
        d->status = Modified;

    if (d->status == NewCreated)
        d->status = Added;
}

void Table::setModel(TableModel *model)
{
    //Q_D(Table);
    d->model = model;
}

TableModel * Table::model()  const{
    return d->model;
}

QVariant Table::evaluate(QString fieldname,QList<FieldModel *> selectedFields) const
{
    if(selectedFields.isEmpty())
    {
        selectedFields.append(d->model->fields());
    }
    QScriptEngine engine;
    const FieldModel * field =model()->field(fieldname);
    foreach(Nut::FieldModel * field, selectedFields){
        QVariant v = this->property(field->name.toLocal8Bit().data());
        qDebug()<<fieldname<<", Name:"<<field->name<<",Table:"<<field->tableClassName<<",Type:"<<v.type();
        if(v.type()==QVariant::Int||v.type()==QMetaType::Short){
            engine.globalObject().setProperty(field->name,v.toInt());
            engine.globalObject().setProperty(QString("%1_%2").arg(field->tableClassName).arg(field->name),v.toInt());
        }
        if(v.type()==QVariant::Bool)
        {
            engine.globalObject().setProperty(field->name,v.toBool());
            engine.globalObject().setProperty(QString("%1_%2").arg(field->tableClassName).arg(field->name),v.toBool());
        }
        if(v.type()==QVariant::Date)
        {
            engine.globalObject().setProperty(field->name,v.toDate().toString());
            engine.globalObject().setProperty(QString("%1_%2").arg(field->tableClassName).arg(field->name),v.toDate().toString());
        }
        if(v.type()==QVariant::DateTime)
        {
            engine.globalObject().setProperty(field->name,v.toDateTime().toString());
            engine.globalObject().setProperty(QString("%1_%2").arg(field->tableClassName).arg(field->name),v.toDateTime().toString());
        }
        if(v.type()==QVariant::Time)
        {
            engine.globalObject().setProperty(field->name,v.toTime().toString());
            engine.globalObject().setProperty(QString("%1_%2").arg(field->tableClassName).arg(field->name),v.toTime().toString());
        }
        if(v.type()==QVariant::Char||v.type()==QVariant::String)
        {
            engine.globalObject().setProperty(field->name,v.toString());
            engine.globalObject().setProperty(QString("%1_%2").arg(field->tableClassName).arg(field->name),v.toString());
        }
        if(v.type()==QVariant::UInt||v.type()==QMetaType::ULong)
        {
            engine.globalObject().setProperty(field->name,v.toUInt());
            engine.globalObject().setProperty(QString("%1_%2").arg(field->tableClassName).arg(field->name),v.toUInt());
        }
        if(v.type()==QVariant::Uuid)
        {
            engine.globalObject().setProperty(field->name,v.toUuid().toString(QUuid::WithoutBraces));
            engine.globalObject().setProperty(QString("%1_%2").arg(field->tableClassName).arg(field->name),v.toUuid().toString(QUuid::WithoutBraces));
        }
        if( v.type()==QMetaType::Float||v.type()==QVariant::Double||v.type()==QVariant::LongLong||v.type()==QVariant::ULongLong)
        {
            engine.globalObject().setProperty(field->name,v.toDouble());
            engine.globalObject().setProperty(QString("%1_%2").arg(field->tableClassName).arg(field->name),v.toDouble());
        }
    }
    QScriptValue v= engine.evaluate(field->script);

    if(v.isBool())
        return v.toBool();
    else if(v.isDate())
        return v.toDateTime();
    else if(v.isNull())
        return QVariant();
    else if(v.isError()||v.isString())
        return v.toString();
    else if(v.isNumber())
        return v.toNumber();

    return v.toString();
}

void Table::clear()
{
    //Q_D(Table);
    d->changedProperties.clear();
}

QSet<QString> Table::changedProperties() const
{
    //Q_D(const Table);
    return d->changedProperties;
}

bool Table::setParentTable(Table *master, TableModel *masterModel, TableModel *model)
{
    //Q_D(Table);
    d.detach();

    QString masterClassName = master->className();
    d->refreshModel();

    //    if (!d->model)
    //        d->model = TableModel::findByClassName(metaObject()->className());

    foreach (RelationModel *r, model->foreignKeys())
        if(r->masterClassName == masterClassName)
        {
            setProperty(QString(r->localColumn).toLatin1().data(),
                        master->property(masterModel->primaryKey().toUtf8().data()));
            d->changedProperties.insert(r->localColumn);
            return true;
        }

    return false;
}

TableSetBase *Table::parentTableSet() const
{
    //Q_D(const Table);
    return d->parentTableSet;
}

void Table::setParentTableSet(TableSetBase *parent)
{
    //Q_D(Table);
    d->parentTableSet = parent;

    //    if (parent)
    //        d->parentTableSet->add(this);
}

TableSetBase *Table::childTableSet(const QString &name) const
{
    //Q_D(const Table);
    foreach (TableSetBase *t, d->childTableSets)
        if (t->childClassName() == name)
            return t;
    return Q_NULLPTR;
}

int Table::save(Database *db)
{
    //Q_D(Table);

    QSqlQuery q = db->exec(db->sqlGenertor()->saveRecord(this, this->className()));

    auto model = db->model().tableByClassName(this->className());
    if(status() == Added && model->isPrimaryKeyAutoIncrement())
        setProperty(model->primaryKey().toLatin1().data(), q.lastInsertId());

    foreach(TableSetBase *ts, d->childTableSets)
        ts->save(db);
    setStatus(FeatchedFromDB);

    return q.numRowsAffected();
}

Table::Status Table::status() const
{
    //Q_D(const Table);
    return static_cast<Status>(d->status);
}

void Table::setStatus(const Status &status)
{
    //Q_D(Table);
    d->status = status;
}



TablePrivate::TablePrivate() : QSharedData(),
    model(nullptr), status(Table::NewCreated), parentTableSet(nullptr)
{

}

void TablePrivate::refreshModel()
{
    //    Q_Q(Table);
    //    if (!model)
    //        model = TableModel::findByClassName(q->metaObject()->className());
}
QVariant Table::primaryValue() const {

}
void Table::setPrimaryValue(const QVariant &value) {

}
QString Table::className() const {
    TableModel * m=this->d->model;
    if(m)
        return m->className();
    else
        return metaObject()->className();
}

NUT_END_NAMESPACE
