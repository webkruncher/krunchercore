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

#ifndef KRUNCHER_MIME_READER_H
#define  KRUNCHER_MIME_READER_H
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <infotools.h>
#include <tracer.h>
namespace KruncherMimes
{
	using namespace std;
	using namespace KruncherTools;

	struct SocketManager 
	{
		virtual string& Headers() = 0;
		virtual const binarystring& Payload( const size_t len ) = 0;
		virtual void flush() = 0;
		virtual void write( const unsigned char*, size_t ) = 0;
		virtual string getline( const size_t limit ) = 0;
	};


	struct Matcher
	{
		Matcher( const char* _pattern ) : matched( 0 ), matchlen( strlen( _pattern ) )
			{ memcpy( pattern, _pattern, strlen( _pattern ) ); }

		bool operator()( const unsigned char c )
		{
			if ( pattern[ matched ] == c )
			{
				matched++;
				if ( matched == matchlen ) return true;
			} else {
				matched=0;
			}
			return false;
		}
		private:
		size_t matched;
		unsigned char pattern[ 4 ];
		const size_t matchlen;
	};

	template < typename SocketType >
		struct SocketReadWriter 
			: SocketManager
	{
		SocketReadWriter( SocketType& _sock ) 
			: sock( _sock ), 
				EoMimeMatcher( "\r\n\r\n" ),
				EoLineMatcher( "\r\n" ) {}
		virtual void flush() { sock.flush(); }
		virtual void write( const unsigned char* data, size_t datalen)
			{ sock.write( (char*) data, datalen ); }


		string getline( const size_t limit )
		{
			string line;	
			line.reserve( limit );
			size_t len( 0 );
			while ( true )
			{
				unsigned char byte;	
				read( &byte );
				line.append( (char*) &byte, 1 );
				if ( line.size() > limit ) break;
				if ( eol( line ) ) break;
			}
			
			return line;
		}

		string& Headers()
		{
			headers.clear();
			headers.reserve( 512 );
			size_t len( 0 );
			while ( true )
			{
				unsigned char byte;	
				read( &byte );
				headers.append( (char*) &byte, 1 );
				if ( eoh() ) break;
			}
			
			return headers;
		}

		const binarystring& Payload( const size_t want )
		{
			payload.clear();
			unsigned char* cc( (unsigned char*) malloc( want ) );
			if ( ! cc ) return payload;
			read( cc, want );
			payload.append( cc, want );
			free( cc );
			return payload;
		}

		private:

		bool eoh()
		{
			const size_t sz( headers.size() );
			if ( sz < 4 ) return false;
			const string e4( headers.substr( sz-4, 4 ) );
			if ( e4 == "\r\n\r\n" ) return true;
			const string e2( headers.substr( sz-2, 2 ) );
			if ( e2 == "\r\r" ) return true;
			if ( e2 == "\n\n" ) return true;
			return false;
		}

		bool eol( const string line )
		{
			const size_t sz( line.size() );
			if ( sz < 2 ) return false;
			const string e2( line.substr( sz-2, 2 ) );
			if ( e2 == "\r\n" ) return true;
			return false;
		}
                void read( unsigned char* bytes, const size_t much=1 )
                {
                        sock.read( (char*) bytes, much );
                }


		string headers;
		binarystring  payload;
		SocketType& sock;
		Matcher EoMimeMatcher;
		Matcher EoLineMatcher;
	};
} // namespace KruncherMimes
#endif // KRUNCHER_MIME_READER_H


