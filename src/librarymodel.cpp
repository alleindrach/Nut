#include "librarymodel.h"
#include  "QJsonObject"
#include  "QJsonArray"
#include "QJsonValue"
//LibraryModel::LibraryModel(QObject *parent) : QObject(parent)
//{

//}

//"postlibref":{
//    "name":"postlibref",
//    "localColumns":["post_user_name","post_date"],
//    "localProperties":["postUserName","postDate"],
//    "masterClassName":"Post"
//    "refColumns":["user_name","date"]
//}
LibraryModel::LibraryModel(const QJsonObject &obj)
{
    m_librefName=obj.value("name").toString();

    m_localColumns.clear();
    QVariantList  vlist=obj.value("localColumns").toArray().toVariantList();

    foreach(QVariant v,vlist){
        m_localColumns.append(v.toString());
    }

    m_localProperties.clear();
    vlist=obj.value("localProperties").toArray().toVariantList();
    foreach(QVariant v,vlist){
        m_localProperties.append(v.toString());
    }

    m_libraryTableName = obj.value("libraryTableName").toString();

    m_refColumns.clear();
    vlist=obj.value("refColumns").toArray().toVariantList();
    foreach(QVariant v,vlist){
        m_refColumns.append(v.toString());
    }
    m_thisTable = m_libraryTable = nullptr;
}

QJsonObject LibraryModel::toJson() const
{
    QJsonObject o;
    o.insert("name",m_librefName);
    o.insert("localColumns",QJsonArray::fromStringList( m_localColumns));
    o.insert("localProperties", QJsonArray::fromStringList(m_localProperties));
    o.insert("libraryTableName", m_libraryTableName);
    o.insert("refColumns", QJsonArray::fromStringList(m_refColumns));
    return o;
}


bool operator ==(const LibraryModel &l, const LibraryModel &r)
{
    if(l.localColumnsKey() != r.localColumnsKey()){
        return false;
    }
    if(l.refColumnsKey() != r.refColumnsKey()){
        return false;
    }
    if(l.localPropertiesKey() != r.localPropertiesKey()){
        return false;
    }
//    if(r.masterClassName() != l.masterClassName())
//    {
//        return false;
//    }
    return true;
}

bool operator !=(const LibraryModel &l, const LibraryModel &r)
{
    return !(l == r);
}

QString  LibraryModel::localColumnsKey() const{
    QStringList  tmp=this->m_localColumns;
    tmp.sort();
    return tmp.join("_");
}
QString  LibraryModel::localPropertiesKey() const{
    QStringList tmp=this->m_localProperties;
    tmp.sort();
    return tmp.join("_");
}
QString  LibraryModel::refColumnsKey() const {
    QStringList tmp=this->m_refColumns;
    tmp.sort();
    return tmp.join("_");
}


QString LibraryModel::librefName() const{
    return m_librefName;
}
QStringList LibraryModel::localColumns() const{
    return m_localColumns;
}
QStringList LibraryModel::localProperties()  const{
    return m_localProperties;
}
QStringList LibraryModel::refColumns() const{
    return m_refColumns;
}
TableModel* LibraryModel::thisTable() const{
    return m_thisTable;
}
TableModel* LibraryModel::libraryTable() const{
    return m_libraryTable;
}
QString LibraryModel::libraryTableName() const{
    return m_libraryTableName;
}

void LibraryModel::setLibrefName(QString  value) {
    this->m_librefName=value;
}
void LibraryModel::setLocalColumns(QStringList value) {
    this->m_localColumns=value;
}
void LibraryModel::setLocalProperties(QStringList value) {
    this->m_localProperties=value;
}

void LibraryModel::setRefColumns(QStringList value) {
    this->m_refColumns=value;
}

void LibraryModel::setThisTable(TableModel * value) {
    this->m_thisTable=value;
}

void LibraryModel::setLibraryTable(TableModel * value) {
    this->m_libraryTable=value;
}
void LibraryModel::setLibraryTableName(QString value) {
    this->m_libraryTableName=value;
}
