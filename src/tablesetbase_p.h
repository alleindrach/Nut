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

#ifndef TABLESETBASE_H
#define TABLESETBASE_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>
#include <QtCore/QSet>
#include <QExplicitlySharedDataPointer>

#include "defines.h"

NUT_BEGIN_NAMESPACE

class Table;
class TableModel;
class Database;
class TableSetBaseData;
class TableSetBase : public QObject
{

public:
    explicit TableSetBase(Database *parent);
    explicit TableSetBase(Table *parent);
    virtual ~TableSetBase();

    virtual int save(Database *db, bool cleanUp = false);
    void clearChilds();
    QString childClassName() const;

    Database *database() const;
    void setDatabase(Database *database);

protected:
    QExplicitlySharedDataPointer<TableSetBaseData> data;

public://TODO: change this to private
//    void add(Table* t);
//    void remove(Table *t);

    void add(Row<Table> t);
    void remove(Row<Table> t);
    friend class Table;
    friend class QueryBase;
};

NUT_END_NAMESPACE

#endif // TABLESETBASE_H
