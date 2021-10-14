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
	int MimeTest( const string txt, const bool expectation )
{
	const string path( string("../../src/" ) + txt );
	ifstream in( path.c_str() );
	SocketReader< istream, chunksize  > mime( in );
	if ( ! mime ) return 1;
	const string& headers( mime.Headers() );
	KruncherTools::stringvector Headers;
	Headers.split( headers, "\r\n" );
	//cerr << "Headers:" << endl << Headers;

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
	const basic_string<unsigned char>& payload( mime.Payload( ContentLength ) );
	int result( 0 );
	if ( ! expectation )
		result=( ContentLength != payload.size() );
	else
		result=( ContentLength == payload.size() );
	
	if ( result ) cout << green; else cout << red; 
	cout << setw( 24 ) << txt << fence << setw( 6 ) << chunksize << fence << setw( 6 ) << ContentLength << fence << setw( 6 ) << payload.size() << normal << endl;
	return result;
	//cout << "Payload:" << endl << (char*)payload.data() << endl;
}


int MimeTester()
{
	int status( 0 );
	map< string, bool > testfiles;
	testfiles[ "badmime.txt" ] = false;
	testfiles[ "mimetest.txt" ] = true;
	testfiles[ "badbinaryheaders.txt" ] = true;
	testfiles[ "binarypayload.txt" ] = true;
	for ( map< string, bool >::const_iterator it=testfiles.begin();it!=testfiles.end();it++)
	{
		const string txt( it->first );
		const bool expectation( it->second );
		status|=MimeTest< 116 >( txt, expectation );
		status|=MimeTest< 12 >( txt, expectation );
		status|=MimeTest< 8192 >( txt, expectation );
		status|=MimeTest< 4495 >( txt, expectation );
		status|=MimeTest< 4608 >( txt, expectation );
	}
	if ( status ) return 0; else return 1;
	return status;
}


