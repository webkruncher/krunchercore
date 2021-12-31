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
#include <mimetester.h>
#include <hypertools.h>


struct IoFile : ifstream 
{
	IoFile( const string in, const string out )
		: ifstream( in.c_str(), std::ifstream::in ), o( out.c_str() ), totalread( 0 ) 
	{
		filesize=FileSize( in );
		{ofstream progress( "progress.txt", ios::app ); progress << in << ":" << filesize << " bytes" << endl; }
	}
	virtual ~IoFile(){}
	virtual void flush(){ o.flush(); }
	virtual size_t read( char* dest, const size_t size )
	{
		if ( ( totalread+=size )  > filesize )
		{
			{
				ofstream progress( "progress.txt", ios::app ); 
				progress << red;
				progress << tab << "You have already read: " << totalread << " bytes" << endl;
				progress << tab << "But you are asking for " << size << " more bytes" << endl;
				progress << tab << "That would be a total of " << (totalread+=size ) << " bytes " << endl;
				progress << normal;
			}
			throw string( "Reading too much" );
		} else {
			ofstream progress( "progress.txt", ios::app ); 
			progress << green;
			progress << tab << "You have already read: " << totalread << " bytes" << endl;
			progress << tab << "Now you are asking for " << size << " more bytes" << endl;
			progress << tab << "For would be a total of " << (totalread+=size ) << " bytes " << endl;
			progress << normal;
		}
		totalread+=size;
		{ofstream progress( "progress.txt", ios::app ); progress << size << " / " << totalread << endl; }
		ifstream::read( dest, size ); 
		return size;
	}
	virtual size_t write( char* dest, const size_t size )
	{
		o.write( dest, size );
		return size;
	}
	private:
	unsigned long filesize;
	unsigned long totalread;
	ofstream o;
};


struct TestResult
{
	TestResult( const Hyper::MimeHeaders _headers, const size_t _ContentLength, const binarystring _payload, const bool _compare=true )
		: headers( _headers ), ContentLength( _ContentLength ), payload( _payload ), compare( _compare ) {}
	const Hyper::MimeHeaders headers;
	size_t ContentLength;
	const binarystring payload;
	const bool compare;

	string requestfile;
	const string GetRequestName() 
	{
		Hyper::MimeHeaders::const_iterator it( headers.find( "Request" ) ); 
		if ( it == headers.end() ) return "";
		requestfile=it->second;
		KruncherTools::trim( requestfile );
		return requestfile;
	}
};

TestResult Consume( SocketManager& sock )
{
	if ( ! sock ) throw string("Cannot read mime");
	const string& headers( sock.Headers() );
	Hyper::MimeHeaders Headers( headers );
	string TransferEncoding( Headers.TransferEncoding() );	

	if ( TransferEncoding ==  "chunked") 
	{
		binarystring payload;
		TestResult t( headers, 0, payload, false );

		string line;
		sock.getline( line, 512 );
		char *Ender( NULL );
		const size_t ChunkSize(strtol( line.c_str(), &Ender, 16 ));
		const binarystring& Payload( sock.Payload( ChunkSize ) );
		//cout << "Payload:" << endl << (char*) Payload.data() << endl;
		return t;
	}

	size_t ContentLength( Headers.ContentLength() );

	if ( true )
	{
		const binarystring& payload( sock.Payload( ContentLength ) );
		TestResult result( Headers, ContentLength, payload ) ;
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
				TestResult result( Headers, ContentLength, part ) ;
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
	{ofstream progress( "progress.txt", ios::app); progress << "MimeTest<" << chunksize << ">(" << fname << " ) " << endl;}
	const string ipath( string("tests/" ) + fname );
	const string opath( string("tests/" ) + fname + string("out") );

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
	{ofstream progress( "progress.txt"); progress << "WIP" << endl;}
	//return ShortMimeTester();
	int status( 0 );
	map< string, bool > testfiles;

	//testfiles[ "chunked.txt" ] = false;
	//testfiles[ "badmime.txt" ] = false;
	testfiles[ "mimetest.txt" ] = true;
	//testfiles[ "badbinaryheaders.txt" ] = true;
	//testfiles[ "binarypayload.txt" ] = true;

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


int JournalTester()
{
	int status( 0 );
	map< string, bool > testfiles;

	regex_t rx;
	const string exp( "^get.*\\.journal$" );
	if ( regcomp( &rx, exp.c_str(), REG_EXTENDED ) ) throw exp;
	MimeTests::Dir dir( "tests", true, rx );
	if ( ! dir ) return -1;

	for ( MimeTests::Dir::const_iterator it=dir.begin();it!=dir.end();it++ )
		testfiles[ *it ] = false;
	
	for ( map< string, bool >::const_iterator it=testfiles.begin();it!=testfiles.end();it++)
	{
		const string txt( it->first );
		const bool expectation( it->second );
		//status|=MimeTest< 116 >( txt, expectation );
		status|=MimeTest< 12 >( txt, expectation );
		//status|=MimeTest< 8192 >( txt, expectation );
		//status|=MimeTest< 4495 >( txt, expectation );
		status|=MimeTest< 4608 >( txt, expectation );
	}
	if ( status ) return 0; else return 1;
}


