#ifndef TABLEPRIVATE_H
#define TABLEPRIVATE_H

#include "defines.h"

#include <QtCore/QSet>
#include <QSharedData>

NUT_BEGIN_NAMESPACE

class TableModel;
class Table;
class TableSetBase;
class TablePrivate : public QSharedData {
    Table *q_ptr;
    Q_DECLARE_PUBLIC(Table)

public:
    TablePrivate();


    TableModel *model;
    int status;
    QSet<QString> changedProperties;
    TableSetBase *parentTableSet;
    QSet<TableSetBase*> childTableSets;
    void refreshModel();
};

NUT_END_NAMESPACE

#endif // TABLEPRIVATE_H
