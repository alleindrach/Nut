#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H
#include "defines.h"
#include <QObject>
class TableModel;
class NUT_EXPORT LibraryModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString librefName READ librefName WRITE setLibrefName NOTIFY librefNameChanged)
    Q_PROPERTY(QStringList localColumns READ localColumns WRITE setLocalColumns NOTIFY localColumnsChanged)
    Q_PROPERTY(QStringList localProperties READ localProperties WRITE setLocalProperties NOTIFY localPropertiesChanged)
    Q_PROPERTY(TableModel* thisTable  READ thisTable WRITE setThisTable NOTIFY thisTableChanged)
    Q_PROPERTY(QStringList refColumns READ refColumns WRITE setRefColumns NOTIFY refColumnsChanged)
    Q_PROPERTY(TableModel* libraryTable READ libraryTable WRITE setLibraryTable NOTIFY libraryTableChanged)
    Q_PROPERTY(QString masterClassName READ masterClassName WRITE setMasterClassName NOTIFY masterClassNameChanged)
public:
//    explicit LibraryModel(QObject *parent = nullptr);

    LibraryModel() :m_librefName(QString()), m_localColumns(QStringList()), m_localProperties(QStringList()),
        m_thisTable(nullptr), m_refColumns(QStringList()), m_masterClassName(QString())
    {
    }
    explicit LibraryModel(const QJsonObject &obj);


    QJsonObject toJson() const;

    QString localColumnsKey() const;
    QString localPropertiesKey() const;
    QString refColumnsKey() const ;

    QString librefName() const;
    QStringList localColumns() const;
    QStringList localProperties()  const;
    QStringList refColumns() const;
    TableModel* thisTable() const;
    TableModel* libraryTable() const;
    QString masterClassName() const;

    void setLibrefName(QString) ;
    void setLocalColumns(QStringList) ;
    void setLocalProperties(QStringList)  ;
    void setRefColumns(QStringList) ;
    void setThisTable(TableModel *) ;
    void setLibraryTable(TableModel *) ;
    void setMasterClassName(QString) ;
signals:
    void librefNameChanged();
    void localColumnsChanged();
    void localPropertiesChanged();
    void thisTableChanged();
    void refColumnsChanged();
    void libraryTableChanged();
    void masterClassNameChanged();
public slots:
private:
//    //slave
    QStringList m_localColumns;
    QStringList m_localProperties;
    TableModel *m_thisTable{nullptr};
//    //master
    QStringList m_refColumns;
    TableModel *m_libraryTable{nullptr};

    QString m_masterClassName;
    QString m_librefName;
};

bool operator ==(const LibraryModel &l, const LibraryModel &r);
bool operator !=(const LibraryModel &l, const LibraryModel &r);


#endif // LIBRARYMODEL_H
