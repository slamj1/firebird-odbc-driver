/*
 *  
 *     The contents of this file are subject to the Initial 
 *     Developer's Public License Version 1.0 (the "License"); 
 *     you may not use this file except in compliance with the 
 *     License. You may obtain a copy of the License at 
 *     http://www.ibphoenix.com/main.nfs?a=ibphoenix&page=ibp_idpl.
 *
 *     Software distributed under the License is distributed on 
 *     an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either 
 *     express or implied.  See the License for the specific 
 *     language governing rights and limitations under the License.
 *
 *
 *  The Original Code was created by James A. Starkey for IBPhoenix.
 *
 *  Copyright (c) 1999, 2000, 2001 James A. Starkey
 *  All Rights Reserved.
 */

// IscTablesResultSet.cpp: implementation of the IscTablesResultSet class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include "IscDbc.h"
#include "IscTablesResultSet.h"
#include "IscConnection.h"
#include "IscDatabaseMetaData.h"
#include "IscResultSet.h"
#include "IscPreparedStatement.h"
#include "IscBlob.h"

#define TABLE_TYPE	4
#define REMARKS		5

namespace IscDbcLibrary {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IscTablesResultSet::IscTablesResultSet(IscDatabaseMetaData *metaData)
		: IscMetaDataResultSet(metaData)
{
}

void IscTablesResultSet::getTables(const char * catalog, const char * schemaPattern, const char * tableNamePattern, int typeCount, const char * * types)
{
	char sql[2048] =  "select cast (NULL as varchar(7)) as table_cat,\n"				// 1
			          "cast (tbl.rdb$owner_name as varchar(31)) as table_schem,\n"		// 2
					  "cast (tbl.rdb$relation_name as varchar(31)) as table_name,\n"	// 3
					  "cast ('TABLE' as varchar(13)) as table_type,\n"					// 4
					  "cast (NULL as varchar(255)) as remarks,\n"						// 5
					  "tbl.rdb$system_flag,\n"											// 6
					  "tbl.rdb$view_blr as view_blr,\n"									// 7
					  "tbl.rdb$description as remarks_blob\n"							// 8
					  "from rdb$relations tbl\n";

	char * ptFirst = sql + strlen(sql);
	const char *sep = " where (";
	bool firstWhere = true;

	if (schemaPattern && *schemaPattern)
	{
		expandPattern (ptFirst, " where ","tbl.rdb$owner_name", schemaPattern);
		sep = " and (";
		firstWhere = false;
	}

	if (tableNamePattern && *tableNamePattern)
	{
		expandPattern (ptFirst, firstWhere ? " where " : " and ", "tbl.rdb$relation_name", tableNamePattern);
		sep = " and (";
	}

	if ( !metaData->allTablesAreSelectable() )
	{
		metaData->existsAccess(ptFirst, sep, "tbl", 0, ")\n");
		sep = " and (";
	}

	char * pt = ptFirst;
		
	for (int n = 0; n < typeCount; ++n)
		if (!strcmp (types [n], "TABLE"))
			{
			addString(pt, sep);
			addString(pt, "(tbl.rdb$view_blr is null and tbl.rdb$system_flag = 0)");
			sep = " or ";
			}
		else if (!strcmp (types [n], "VIEW"))
			{
			addString(pt, sep);
			addString(pt, "tbl.rdb$view_blr is not null");
			sep = " or ";
			}
		else if (!strcmp (types [n], "SYSTEM TABLE"))
			{
			addString(pt, sep);
			addString(pt, "(tbl.rdb$view_blr is null and tbl.rdb$system_flag = 1)");
			sep = " or ";
			}

	if ( pt > ptFirst )
		{
		ptFirst = pt;
		addString(ptFirst, ")\n");
		}

	addString(ptFirst, " order by tbl.rdb$system_flag desc, tbl.rdb$owner_name, tbl.rdb$relation_name");

	prepareStatement (sql);
	numberColumns = 5;
}

bool IscTablesResultSet::nextFetch()
{
	if (!IscResultSet::nextFetch())
		return false;

	if ( !metaData->getUseSchemaIdentifier() )
		sqlda->setNull(2);

	if ( sqlda->getShort (6) )
		sqlda->updateVarying (4, "SYSTEM TABLE");
	else if ( !sqlda->isNull(7) )
		sqlda->updateVarying (4, "VIEW");

	if ( !sqlda->isNull(8) )
		convertBlobToString( 5, 8 );

	return true;
}

}; // end namespace IscDbcLibrary
