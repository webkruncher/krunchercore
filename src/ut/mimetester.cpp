/*
 * Copyright (c) Jack M. Thompson WebKruncher.com, exexml.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebKruncher nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Jack M. Thompson ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Jack M. Thompson BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <fstream>
#include <mimes.h>
using namespace std;
using namespace KruncherMimes;
#include <infotools.h>

template < size_t chunksize >
	int MimeTest()
{
	ifstream in( "../../src/mimetest.txt" );
	SocketReader< istream, chunksize  > mime( in );
	if ( ! mime ) return 1;
	const string& headers( mime.Headers() );
	KruncherTools::stringvector Headers;
	Headers.split( headers, "\r\n" );
	cerr << "Headers:" << endl << Headers;

	size_t ContentLength( 0 );
	for ( KruncherTools::stringvector::const_iterator hit=Headers.begin();hit!=Headers.end();hit++)
	{
		const string H( *hit );
		if ( H.find("Content-Length:") == 0 )
		{ 
			const size_t coln( H.find( ":" ) );
			if ( coln == string::npos ) throw H;
			const string cls( H.substr( coln+1, H.size()-1 ) );
			char *Ender( NULL );
			ContentLength=strtol( cls.c_str(), &Ender, 10 );
		}
	}
	const string& payload( mime.Payload( ContentLength ) );
	cout << "Payload:" << endl << payload << endl;
	return 0;
}


int MimeTester()
{
	cout << "\033[31m"; MimeTest< 116 >();
#if 0
	cout << "\033[31m"; MimeTest< 116 >();
	cout << "\033[32m"; MimeTest< 12 >();
	cout << "\033[34m"; MimeTest< 8192 >();
	cout << "\033[35m"; MimeTest< 4495 >();
	cout << "\033[36m"; MimeTest< 4608 >();
#endif
	cout << "\033[0m";
	return 0;
}


