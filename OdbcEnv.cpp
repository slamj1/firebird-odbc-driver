/*
 *  
 *     The contents of this file are subject to the Initial 
 *     Developer's Public License Version 1.0 (the "License"); 
 *     you may not use this file except in compliance with the 
 *     License. You may obtain a copy of the License at 
 *     http://www.ibphoenix.com/idpl.html. 
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

// OdbcEnv.cpp: implementation of the OdbcEnv class.
//
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "OdbcJdbc.h"
#include "OdbcEnv.h"
#include "OdbcConnection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OdbcEnv::OdbcEnv()
{
	connections = NULL;

#ifndef _WIN32
	if (!(odbcIniFileName = getenv ("ODBCINI")))
#endif
		odbcIniFileName = "ODBC.INI";
}

OdbcEnv::~OdbcEnv()
{

}

OdbcObjectType OdbcEnv::getType()
{
	return odbcTypeEnv;
}

RETCODE OdbcEnv::allocHandle(int handleType, SQLHANDLE * outputHandle)
{
	clearErrors();
	*outputHandle = SQL_NULL_HDBC;

	if (handleType != SQL_HANDLE_DBC)
		return sqlReturn (SQL_ERROR, "HY000", "General Error");

	OdbcConnection *connection = new OdbcConnection (this);
	connection->next = connections;
	connections = connections;
	*outputHandle = (SQLHANDLE)connection;

	return sqlSuccess();
}

RETCODE OdbcEnv::sqlEndTran(int operation)
{
	clearErrors();
	RETCODE ret = SQL_SUCCESS;

	for (OdbcConnection *connection = connections; connection;
		 connection = (OdbcConnection*) connection->next)
		{
		RETCODE retcode = connection->sqlEndTran (operation);
		if (retcode != SQL_SUCCESS)
			ret = retcode;
		}

	return ret;
}

void OdbcEnv::connectionClosed(OdbcConnection * connection)
{
	for (OdbcObject **ptr = (OdbcObject**) &connections; *ptr; ptr =&((*ptr)->next))
		if (*ptr == connection)
			{
			*ptr = connection->next;
			break;
			}
}

RETCODE OdbcEnv::sqlSetEnvAttr(int attribute, SQLPOINTER value, int length)
{
	clearErrors();

	return sqlSuccess();
}
