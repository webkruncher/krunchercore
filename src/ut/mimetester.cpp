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
#include <directory.h>


struct IoFile : ifstream 
{
	IoFile( const string in, const string out )
		: ifstream( in.c_str(), std::ifstream::in ), o( out.c_str() ) 
	{}
	virtual ~IoFile(){}
	virtual void flush(){ o.flush(); }
	virtual size_t read( char* dest, const size_t size )
	{
		//if ( ifstream::fail() ) throw string( "IoFile failed before read") ;
		ifstream::read( dest, size ); 
		//if ( ifstream::fail() ) throw string( "IoFile failed after read") ;
		if ( ifstream::fail() ) cout << "!";
		return size;
	}
	virtual size_t write( char* dest, const size_t size )
	{
		o.write( dest, size );
		return size;
	}
	private:
	ofstream o;
};


struct TestResult
{
	TestResult( const string _headers, const size_t _ContentLength, const binarystring _payload, const bool _compare=true )
		: headers( _headers ), ContentLength( _ContentLength ), payload( _payload ), compare( _compare ) {}
	const string headers;
	size_t ContentLength;
	const binarystring payload;
	const bool compare;

	const string GetRequestName()
	{
		stringvector sv;
		sv.split( headers, "\r\r" );
		for ( stringvector::iterator it=sv.begin();it!=sv.end();it++)
		{
			const string& s( *it );
			const size_t r( s.find("Request:") );
				if ( r!=string::npos )
					return s.substr( r, s.size()-r );
		}
		return "";
	}
};

TestResult Consume( SocketManager& sock )
{
	if ( ! sock ) throw string("Cannot read mime");
	const string& headers( sock.Headers() );
	KruncherTools::stringvector Headers;
	Headers.split( headers, "\r\n" );


	string TransferEncoding;	
	for ( KruncherTools::stringvector::const_iterator hit=Headers.begin();hit!=Headers.end();hit++)
	{
		binarystring payload;
		const string H( *hit );
		if ( H.find("Transfer-Encoding:chunked") == 0 )
		{
			TestResult t( headers, 0, payload, false );

			string line;
			sock.getline( line, 512 );
			char *Ender( NULL );
			const size_t ChunkSize(strtol( line.c_str(), &Ender, 16 ));
			const binarystring& Payload( sock.Payload( ChunkSize ) );
			cout << "Payload:" << endl << (char*) Payload.data() << endl;
			return t;
		}
	}

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

	if ( false )
	{
		const binarystring& payload( sock.Payload( ContentLength ) );
		TestResult result( headers, ContentLength, payload ) ;
		return result;
	} else {
		const size_t pieces( 4 );
		size_t piece( ContentLength / pieces );
		size_t much( 0 );
		while ( much < ContentLength )
		{
			const binarystring& part( sock.Payload( piece ) );
			much+=piece;
			if ( much == ContentLength )
			{
				TestResult result( headers, ContentLength, part ) ;
				return result;
			}
			piece=min( piece, ContentLength-much );
		}
	}
	const binarystring dummy;
	TestResult result( headers, 0, dummy ) ;
	return result;
}

template < size_t chunksize >
	int MimeTest( const string fname, const bool expectation )
{
	const string ipath( string("../../src/tests/" ) + fname );
	const string opath( string("../../src/tests/" ) + fname + string("out") );
	//cout << teal << ipath << normal << endl;
	IoFile io( ipath.c_str(), opath.c_str()  );
	SocketReadWriter< IoFile, chunksize  > sock( io );
	TestResult t( Consume( sock ) );

	//cout << t.payload.c_str() << endl;
	

	if ( t.compare )
	{
		int result( 0 );
		if ( ! expectation )
			result=( t.ContentLength != t.payload.size() );
		else
			result=( t.ContentLength == t.payload.size() );

		
		const string RequestName( t.GetRequestName( ) ); 

		bool Same( true );
		if ( ! RequestName.empty() )
		{
			stringstream compname;
			compname<<"/home/jmt/websites/text/jackmthompson.ninja/" << RequestName;
			if ( KruncherDirectory::FileExists( compname.str().c_str() ) )
			{
				const size_t fsize( KruncherDirectory::FileSize( compname.str().c_str() ) );
				unsigned char* data( (unsigned char*) malloc( fsize ) );
				if ( ! data ) throw ipath;
				KruncherDirectory::LoadBinaryFile( compname.str().c_str() , data, fsize );

				if ( memcmp( data, t.payload.data(), t.ContentLength ) ) Same=false;
				stringstream ssname;
				ssname << opath << "." << chunksize;
				//ofstream o( ssname.str().c_str() );
				//cout << compname.str() << " " << ssname.str() << endl;
				io.write( (char*) t.payload.data(), t.ContentLength );
				free( data );
			}
		}

		
		if ( result ) cout << green; else cout << red; 
		if ( ! Same ) cout << bold << blue << redbk;
		cout << setw( 24 ) << fname << fence << setw( 6 ) << chunksize << fence << setw( 6 ) << t.ContentLength << fence << setw( 6 ) << t.payload.size() << normal << endl;
		return result;	
	}
	return 0;

}

int ShortMimeTester()
{
	int status( 0 );
	map< string, bool > testfiles;
	testfiles[ "go.txt" ] = true;
	for ( map< string, bool >::const_iterator it=testfiles.begin();it!=testfiles.end();it++)
	{
		const string txt( it->first );
		const bool expectation( it->second );
		status|=MimeTest< 12 >( txt, expectation );
	}
	if ( status ) return 0; else return 1;
}

int MimeTester()
{
	//return ShortMimeTester();
	int status( 0 );
	map< string, bool > testfiles;

	testfiles[ "chunked.txt" ] = false;
	testfiles[ "badmime.txt" ] = false;
	testfiles[ "mimetest.txt" ] = true;
	testfiles[ "badbinaryheaders.txt" ] = true;
	testfiles[ "binarypayload.txt" ] = true;

	testfiles[ "shortpayload.txt" ] = true;

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
}
