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
 *  The Original Code was created by Vladimir Tsvigun for IBPhoenix.
 *
 *  Copyright (c) 2003 Vladimir Tsvigun
 *  All Rights Reserved.
 */

//  
// IscArray.h: interface for the IscArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_IscArray_H_)
#define _IscArray_H_

#include "BinaryBlob.h"
#include "Connection.h"

namespace IscDbcLibrary {

class IscConnection;
class IscStatement;
class Value;

class CAttrArray
{
public:
	CAttrArray()	{ memset( this, 0, sizeof ( *this) ); }
	void			loadAttributes ( IscConnection *connection, char * nameRelation, char * nameFields, int sqlsubtype );
	int				getPrecisionInternal();
	int				getBufferLength();
	JString			getFbSqlType();

public:
	ISC_ARRAY_DESC	arrDesc;
	void*			arrBufData;
	int				arrBufDataSize;
	int				arrCountElement;
	int				arrSizeElement;
	int				arrTypeElement;
	int				arrSubTypeElement;
	int				arrOctetLength;
};

class IscArray : public BinaryBlob, public CAttrArray
{
protected:
	void convStringToArray( char *data, long length );

public:
	void attach(char * pointBlob, bool fetched, bool clear);
	void attach(CAttrArray * arr, bool fetchBinary = true, bool bClear = false);
	void detach(CAttrArray * arr);
	void removeBufData();
	void getBytesFromArray();
	void fetchArrayToString();
	void writeBlob(char * sqldata);
	void writeStreamHexToBlob(char * sqldata) {};
	void writeBlob(char * sqldata, char *data, long length);
	void writeStringHexToBlob(char * sqldata, char *data, long length);
	void writeArray(Value * value);

	void init();
	void bind(IscConnection	*parentConnection,XSQLVAR *var);
	void bind(Connection *connect, char * sqldata);
	void getBinary(long pos, long length, void * address);
	int length();
	int getSegment (int offset, int length, void *address);
	int	getLength();

	IscArray();
	IscArray(CAttrArray * ptArr);
	IscArray(IscConnection	*parentConnection,XSQLVAR *var);
	~IscArray();

	IscConnection	*connection;
	ISC_QUAD		arrayId;
	bool			clearData;
	bool			fetched;
	bool			fetchedBinary;
};

}; // end namespace IscDbcLibrary

#endif // !defined(_IscArray_H_)
