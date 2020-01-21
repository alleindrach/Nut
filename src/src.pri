DEFINES += NUT_SHARED_POINTER

HEADERS += \
    $$PWD/generators/sqlgeneratorbase_p.h \
    $$PWD/generators/postgresqlgenerator.h \
    $$PWD/generators/mysqlgenerator.h \
    $$PWD/generators/sqlitegenerator.h \
    $$PWD/generators/sqlservergenerator.h \
    $$PWD/librarymodel.h \
    $$PWD/tablesetbasedata.h \
    $$PWD/types/dbgeography.h \
    $$PWD/tableset.h \
    $$PWD/defines_p.h \
    $$PWD/defines.h \
    $$PWD/query.h \
    $$PWD/databasemodel.h \
    $$PWD/changelogtable.h \
    $$PWD/tablesetbase_p.h \
    $$PWD/querybase_p.h \
    $$PWD/tablemodel.h \
    $$PWD/query_p.h \
    $$PWD/table.h \
    $$PWD/database.h \
    $$PWD/database_p.h \
    $$PWD/serializableobject.h \
    $$PWD/sqlmodel.h \
    $$PWD/sqlmodel_p.h \
    $$PWD/phrase.h \
    $$PWD/phrases/abstractfieldphrase.h \
    $$PWD/phrases/assignmentphrase.h \
    $$PWD/phrases/assignmentphraselist.h \
    $$PWD/phrases/conditionalphrase.h \
    $$PWD/phrases/fieldphrase.h \
    $$PWD/phrases/phrasedata.h \
    $$PWD/phrases/phrasedatalist.h \
    $$PWD/phrases/phraselist.h \
    $$PWD/phrases/datephrase.h \
    $$PWD/table_p.h

SOURCES += \
    $$PWD/generators/sqlgeneratorbase.cpp \
    $$PWD/generators/postgresqlgenerator.cpp \
    $$PWD/generators/mysqlgenerator.cpp \
    $$PWD/generators/sqlitegenerator.cpp \
    $$PWD/generators/sqlservergenerator.cpp \
    $$PWD/librarymodel.cpp \
    $$PWD/types/dbgeography.cpp \
    $$PWD/tableset.cpp \
    $$PWD/query.cpp \
    $$PWD/databasemodel.cpp \
    $$PWD/tablesetbase.cpp \
    $$PWD/changelogtable.cpp \
    $$PWD/querybase.cpp \
    $$PWD/tablemodel.cpp \
    $$PWD/table.cpp \
    $$PWD/database.cpp \
    $$PWD/serializableobject.cpp \
    $$PWD/sqlmodel.cpp \
    $$PWD/phrase.cpp \
    $$PWD/phrases/abstractfieldphrase.cpp \
    $$PWD/phrases/assignmentphrase.cpp \
    $$PWD/phrases/assignmentphraselist.cpp \
    $$PWD/phrases/conditionalphrase.cpp \
    $$PWD/phrases/fieldphrase.cpp \
    $$PWD/phrases/phrasedata.cpp \
    $$PWD/phrases/phrasedatalist.cpp \
    $$PWD/phrases/phraselist.cpp \
    $$PWD/phrases/datephrase.cpp


include($$PWD/../3rdparty/serializer/src/src.pri)
