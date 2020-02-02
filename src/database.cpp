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

#include <QtCore/QMetaProperty>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlResult>

#include "database.h"
#include "table.h"
#include "tableset.h"
#include "database_p.h"
#include "defines.h"
#include "tablemodel.h"
#include "generators/postgresqlgenerator.h"
#include "generators/mysqlgenerator.h"
#include "generators/sqlitegenerator.h"
#include "generators/sqlservergenerator.h"
#include "query.h"
#include "changelogtable.h"

#include <iostream>
#include <cstdarg>

#ifndef __CHANGE_LOG_TABLE_NAME
#   define __CHANGE_LOG_TABLE_NAME "__change_logs"
#endif

NUT_BEGIN_NAMESPACE

qulonglong DatabasePrivate::lastId = 0;
QMap<QString, DatabaseModel> DatabasePrivate::allTableMaps;

DatabasePrivate::DatabasePrivate(Database *parent) : q_ptr(parent),
    port(0), sqlGenertor(nullptr), changeLogs(nullptr),
    isDatabaseNew(false)
{
}

bool DatabasePrivate::open(bool update)
{
    if (db.isOpen())
        return true;
    Q_Q(Database);
//    if (update)
    connectionName = q->metaObject()->className()
                     + QString::number(DatabasePrivate::lastId);

    db = QSqlDatabase::addDatabase(driver, connectionName);
    db.setHostName(hostName);
    if (port)
        db.setPort(port);
    db.setDatabaseName(databaseName);
    db.setUserName(userName);
    db.setPassword(password);

    if (driver.startsWith("qsqlite", Qt::CaseInsensitive)
            && !QFile::exists(databaseName)) {
        //Force to execute update database
        isDatabaseNew = true;
        update = true;
    }
    bool ok = db.open();

    if (!ok) {
        qWarning("Could not connect to database, error = %s",
                 db.lastError().text().toLocal8Bit().data());

        if (db.lastError().text().contains("database \"" + databaseName
                                           + "\" does not exist")
            || db.lastError().text().contains("Cannot open database")
            || db.lastError().text().contains("Unknown database '"
                                              + databaseName + "'")) {

            db.close();
            db.setDatabaseName(sqlGenertor->masterDatabaseName(databaseName));
            ok = db.open();
            qDebug("Creating database");
            if (ok) {
                db.exec("CREATE DATABASE " + databaseName);
                db.close();

                if (db.lastError().type() != QSqlError::NoError) {
                    qWarning("Creating database error: %s",
                             db.lastError().text().toLatin1().data());
                    return false;
                }

                isDatabaseNew = true;
                return open(update);
            }
            qWarning("Unknown error detecting change logs, %s",
                     db.lastError().text().toLatin1().data());

        }
        return false;
    }

//    if(update)
        return updateDatabase();
//    else
//        return true;
}

bool DatabasePrivate::updateDatabase()
{
    Q_Q(Database);

    if (!getCurrectScheema())
        return true;

    DatabaseModel last = isDatabaseNew ? DatabaseModel() : getLastScheema();
    DatabaseModel current = currentModel;

    if (last == current) {
        qDebug("Databse is up-to-date");
        //TODO: crash without this and I don't know why!
        changeLogs->clearChilds();
        return true;
    }

    foreach (TableModel *tm, current) {
        foreach (FieldModel *fm, tm->fields()) {
            if (sqlGenertor->fieldType(fm).isEmpty()) {
                qWarning("The type (%s) is not supported for field %s::%s",
                         QMetaType::typeName(fm->type),
                         qPrintable(tm->className()),
                         qPrintable(fm->name));
                return false;
            }
        }
    }
    if (!last.count())
        qDebug("Databse is new");
    else
        qDebug("Databse is changed");

    QStringList sql = sqlGenertor->diff(last, current);

    db.transaction();
    foreach (QString s, sql) {
        db.exec(s);

        if (db.lastError().type() != QSqlError::NoError) {
            qWarning("Error executing sql command `%s`, %s",
                     qPrintable(s),
                     db.lastError().text().toLatin1().data());
            return false;
        }
    }
    putModelToDatabase();
    bool ok = db.commit();

    if (db.lastError().type() == QSqlError::NoError) {

        q->databaseUpdated(last.version(), current.version());
        if (!last.count())
            q->databaseCreated();

    } else {
        qWarning("Unable update database, error = %s",
                 db.lastError().text().toLatin1().data());
    }

    return ok;
}

bool DatabasePrivate::getCurrectScheema()
{
    Q_Q(Database);

    //is not first instanicate of this class
    if (allTableMaps.contains(q->metaObject()->className())) {
        currentModel = allTableMaps[q->metaObject()->className()];
        return false;
    }

    QMap<QString, QString> tables;
    tables.clear();

    // TODO: change logs must not be in model
    int changeLogTypeId = qRegisterMetaType<ChangeLogTable*>();

    currentModel.append(
        new TableModel(changeLogTypeId, __CHANGE_LOG_TABLE_NAME));
    tables.insert(ChangeLogTable::staticMetaObject.className(),
                  __CHANGE_LOG_TABLE_NAME);

    changeLogs = new TableSet<ChangeLogTable>(q);

    for (int i = 0; i < q->metaObject()->classInfoCount(); i++) {
        QString type;
        QString name;
        QString value;

        if (!nutClassInfoString(q->metaObject()->classInfo(i),
                                type, name, value)) {

            errorMessage = QString("No valid table in %1")
                    .arg(q->metaObject()->classInfo(i).value());
            continue;
        }
        if (type == __nut_TABLE) {
            //name: table class name
            //value: table variable name (table name in db)
            tables.insert(name, value);

            int typeId = QMetaType::type(name.toLocal8Bit() + "*");

            if (!typeId)
                qFatal("The class %s is not registered with qt meta object", qPrintable(name));

            TableModel *sch = new TableModel(typeId, value);
            currentModel.append(sch);
        }

        if (type == __nut_DB_VERSION) {
            bool ok;
            int version = value.toInt(&ok);
            if (!ok)
                qFatal("NUT_DB_VERSION macro accept version in format 'x'");
            currentModel.setVersion(version);
        }
    }



    for (int i = 1; i < q->metaObject()->propertyCount(); i++) {
        QMetaProperty tableProperty = q->metaObject()->property(i);
        int typeId = QMetaType::type(tableProperty.typeName());

        if (tables.values().contains(tableProperty.name())
            && (unsigned)typeId >= QVariant::UserType) {
            TableModel *sch = new TableModel(typeId, tableProperty.name());

            currentModel.append(sch);
        }
    }

    for (int i = 0; i < q->metaObject()->classInfoCount(); i++) {
        QString type;
        QString name;
        QString value;

        if (!nutClassInfoString(q->metaObject()->classInfo(i),
                                type, name, value)) {

            errorMessage = QString("No valid table in %1")
                    .arg(q->metaObject()->classInfo(i).value());
            continue;
        }
        if (type == __nut_REF_TYPE) {
            foreach (TableModel *table, currentModel) {
                if(table->name()==type){
                    table->setRefType(value.toInt());
                }
            }
        }
    }
    foreach (TableModel *table, currentModel) {
        foreach (FieldModel *f, table->fields()) {
            if (f->isPrimaryKey && ! sqlGenertor->supportPrimaryKey(f->type))
                qFatal("The field of type %s does not support as primary key",
                       qPrintable(f->typeName));

            if (f->isAutoIncrement && ! sqlGenertor->supportAutoIncrement(f->type))
                qFatal("The field of type %s does not support as auto increment",
                       qPrintable(f->typeName));
        }

        foreach (RelationModel *fk, table->foreignKeys())
            fk->masterTable = currentModel.tableByClassName(fk->masterClassName);
    }

    allTableMaps.insert(q->metaObject()->className(), currentModel);
    return true;
}

DatabaseModel DatabasePrivate::getLastScheema()
{
    Row<ChangeLogTable> u = changeLogs->query()
            ->orderBy(!ChangeLogTable::idField())
            ->first();

//    DatabaseModel ret(q->metaObject()->className());

    if (u) {
        QJsonParseError e;
        QJsonObject json
            = QJsonDocument::fromJson(u->data().replace("\\\"", "\"").toUtf8(), &e).object();

         qDebug()<<u->data().replace("\\\"", "\"");


        DatabaseModel ret = json;
        return ret;
        /*
        foreach (QString key, json.keys()) {
            TableModel *sch = new TableModel(json.value(key).toObject(), key);
            ret.append(sch);
        }*/
    }
    return DatabaseModel();

    //    QSqlQuery query = q->exec("select * from __change_logs order by id
    //    desc limit 1");
    //    DatabaseModel ret;
    //    if(query.next()){
    //        QJsonObject json =
    //        QJsonDocument::fromJson(query.value("data").toByteArray()).object();

    //        foreach (QString key, json.keys()) {
    //            TableModel *sch = new TableModel(json.value(key).toObject(),
    //            key);
    //            ret.append(sch);
    //        }
    //    }
    //    return ret;
}

bool DatabasePrivate::putModelToDatabase()
{
    Q_Q(Database);
    DatabaseModel current = currentModel;
    /*current.remove(__CHANGE_LOG_TABLE_NAME)*/;

    auto changeLog = create<ChangeLogTable>();
    changeLog->setData(QJsonDocument(current.toJson()).toJson(QJsonDocument::Compact));
    changeLog->setVersion(current.version());
    changeLogs->append(changeLog);
    q->saveChanges(true);
    changeLog->deleteLater();

    return true;

    //    QSqlQuery query(db);
    //    query.prepare("insert into __change_logs (data) values (:data)");
    //    query.bindValue(":data",
    //    QString(QJsonDocument(currentModel.toJson()).toJson()));
    //    bool ret = query.exec();
    //    if(query.lastError().type() != QSqlError::NoError)
    //        qWarning(QString("storeScheemaInDB" +
    //        query.lastError().text()).toLatin1().data());
    //    return ret;
}

void DatabasePrivate::createChangeLogs()
{
    //    currentModel.model("change_log")
    QStringList diff = sqlGenertor->diff(nullptr, currentModel.tableByName("__change_log"));

    foreach (QString s, diff)
        db.exec(s);
}

/*!
 * \class Database
 * \brief Database class
 */

Database::Database(QObject *parent)
    : QObject(parent), d_ptr(new DatabasePrivate(this))
{
//    _d = new QSharedDataPointer<DatabasePrivate>(new DatabasePrivate(this));
    DatabasePrivate::lastId++;
}

Database::Database(const Database &other)
    : QObject(other.parent()), d_ptr(new DatabasePrivate(this))
{
    DatabasePrivate::lastId++;
//    _d = other._d;

    setDriver(other.driver());
    setHostName(other.hostName());
    setPort(other.port());
    setDatabaseName(other.databaseName());
    setUserName(other.userName());
    setPassword(other.password());
}

Database::Database(const QSqlDatabase &other)
{
    //TODO: make a polish here
    DatabasePrivate::lastId++;

//    setDriver(other.driver());
    setHostName(other.hostName());
    setPort(other.port());
    setDatabaseName(other.databaseName());
    setUserName(other.userName());
    setPassword(other.password());
    qRegisterMetaType<ChangeLogTable*>();
}

Database::~Database()
{
    Q_D(Database);
    if (d->db.isOpen())
        d->db.close();

    delete d_ptr;
}

QString Database::databaseName() const
{
    Q_D(const Database);
    return d->databaseName;
}

QString Database::hostName() const
{
    Q_D(const Database);
    return d->hostName;
}

int Database::port() const
{
    Q_D(const Database);
    return d->port;
}

QString Database::userName() const
{
    Q_D(const Database);
    return d->userName;
}

QString Database::password() const
{
    Q_D(const Database);
    return d->password;
}

/*!
 * \brief Database::connectionName
 * \return Connection name of current Database \l QSqlDatabase::connectionName
 */
QString Database::connectionName() const
{
    Q_D(const Database);
    return d->connectionName;
}

QString Database::driver() const
{
    Q_D(const Database);
    return d->driver;
}

/*!
 * \brief Database::model
 * \return The model of this database
 */
DatabaseModel Database::model() const
{
    Q_D(const Database);
    return d->currentModel;
}

QString Database::tableName(QString className)
{
    TableModel *m = model().tableByClassName(className);
    if (m)
        return m->name();
    else
        return QString();
}

void Database::setDatabaseName(QString databaseName)
{
    Q_D(Database);
    d->databaseName = databaseName;
}

void Database::setHostName(QString hostName)
{
    Q_D(Database);
    d->hostName = hostName;
}

void Database::setPort(int port)
{
    Q_D(Database);
    d->port = port;
}

void Database::setUserName(QString username)
{
    Q_D(Database);
    d->userName = username;
}

void Database::setPassword(QString password)
{
    Q_D(Database);
    d->password = password;
}

void Database::setConnectionName(QString connectionName)
{
    Q_D(Database);
    d->connectionName = connectionName;
}

void Database::setDriver(QString driver)
{
    Q_D(Database);
    d->driver = driver.toUpper();
}

SqlGeneratorBase *Database::sqlGenertor() const
{
    Q_D(const Database);
    return d->sqlGenertor;
}

QSqlDatabase Database::database()
{
    Q_D(Database);
    return d->db;
}

void Database::databaseCreated()
{

}

void Database::databaseUpdated(int oldVersion, int newVersion)
{
    Q_UNUSED(oldVersion);
    Q_UNUSED(newVersion);
}


/**
 * @brief Database::open
 * Opens the database connection using the current connection values.
 * Returns true on success; otherwise returns false.
 * @return bool
 */
bool Database::open()
{
    return open(true);
}

bool Database::open(bool updateDatabase)
{
    Q_D(Database);

    if (d->driver == "QPSQL" || d->driver == "QPSQL7")
        d->sqlGenertor = new PostgreSqlGenerator(this);
    else if (d->driver == "QMYSQL" || d->driver == "QMYSQL3")
        d->sqlGenertor = new MySqlGenerator(this);
    else if (d->driver == "QSQLITE" || d->driver == "QSQLITE3")
        d->sqlGenertor = new SqliteGenerator(this);
    else if (d->driver == "QODBC" || d->driver == "QODBC3") {
        QString driverName = QString();
        QStringList parts = d->databaseName.toLower().split(';');
        foreach (QString p, parts)
            if (p.trimmed().startsWith("driver="))
                driverName = p.split('=').at(1).toLower().trimmed();

//        if (driverName == "{sql server}")
            d->sqlGenertor = new SqlServerGenerator(this);
        // TODO: add ODBC driver for mysql, postgres, ...
    }

    if (!d->sqlGenertor) {
        qFatal("Sql generator for driver %s not found",
                 driver().toLatin1().constData());
    }

    return d->open(updateDatabase);
}

void Database::close()
{
    Q_D(Database);
    d->db.close();
}

QSqlQuery Database::exec(const QString &sql)
{
    Q_D(Database);

    QSqlQuery q = d->db.exec(sql);
    if (d->db.lastError().type() != QSqlError::NoError)
        qWarning("Error executing sql command: %s; Command=%s",
                 d->db.lastError().text().toLatin1().data(),
                 sql.toUtf8().constData());
    return q;
}

void Database::add(TableSetBase *t)
{
    Q_D(Database);
    d->tableSets.insert(t);
}

int Database::saveChanges(bool cleanUp)
{
    Q_D(Database);

    if (!d->db.isOpen()) {
        qWarning("Database is not open");
        return 0;
    }

    int rowsAffected = 0;
    foreach (TableSetBase *ts, d->tableSets)
    {
        rowsAffected += ts->save(this, cleanUp);
        qDebug()<<"saveing:"<<ts->childClassName()<<" of "<<  rowsAffected <<"  rows";
    }
    return rowsAffected;
}

void Database::cleanUp()
{
    Q_D(Database);
    foreach (TableSetBase *ts, d->tableSets)
        ts->clearChilds();
}

NUT_END_NAMESPACE
