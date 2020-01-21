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

#ifndef DEFINES_P_H
#define DEFINES_P_H

#define __NAME                  "name"
#define __TYPE                  "type"
#define __FIELDS                "fields"
#define __FOREIGN_KEYS          "foreign_keys"
#define __nut_FIELD             "field"

#define __nut_NAME_PERFIX       "nut_info::"
#define __nut_DB_VERSION        "database_version"
#define __nut_PRIMARY_KEY       "primary_key"
#define __nut_AUTO_INCREMENT    "auto_increment"
#define __nut_PRIMARY_KEY_AI    "primary_key_ai"
#define __nut_UNIQUE            "unique"
#define __nut_TABLE             "table"
#define __nut_TABLE_NAME        "table_name"

#define __nut_DISPLAY           "display"
#define __nut_LEN               "len"
#define __nut_DEFAULT_VALUE     "def"
#define __nut_NOT_NULL          "notnull"

#define __nut_FOREIGN_KEY      "foreign_key"
#define __nut_NEW               "new"
#define __nut_REMOVE            "remove"
#define __nut_CHANGE            "change"

#define __nut_CATALOG           "catalog"
#define __nut_LIBREF_NAME    "ref_name"
#define __nut_LIBREF_TABLE_NAME    "ref_table_name"
#define __nut_LIBREF_TABLE_FIELD_NAME "ref_table_field_name"
#define __nut_LIBREF_CATALOG       "ref_catalog"
#define __nut_LIBREF_CATALOG_INDEX "ref_catalog_index"

#define __nut_CAL_EXPRESSION    "cal_expression"
#define __nut_INPUT_TYPE        "input_type"
#define __nut_DATE_SPAN_INDEX   "input_date_span_index"
#define __nut_DATE_SPAN_POS     "input_date_span_pos"
#define __nut_INPUT_OPTIONS     "input_options"

#define __LIBREF_KEYS          "libref_keys"
#define __nut_LIBREF_KEY             "libref_key"
#define __nut_LIBREF_FIELDS             "libref_fields"
#ifdef NUT_NAMESPACE
#   define NUT_BEGIN_NAMESPACE      namespace NUT_NAMESPACE{
#   define NUT_END_NAMESPACE        }
#   define NUT_WRAP_NAMESPACE(x)    NUT_NAMESPACE::x
#else
#   define NUT_BEGIN_NAMESPACE
#   define NUT_END_NAMESPACE
#   define NUT_WRAP_NAMESPACE(x)    x
#endif

#endif // DEFINES_P_H
