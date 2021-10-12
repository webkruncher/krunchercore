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
namespace KruncherMimes
{
		using namespace std;
		template< typename SocketType, int chunksize >
			struct Chunk
		{
			Chunk() : many( 0 ), where( 0 ) { memset( bytes, 0, chunksize ); } 

			size_t read( SocketType& sock )
			{
				const size_t b4( sock.tellg() );
				sock.read( (char*) bytes, chunksize );
				many=(size_t)sock.tellg()-b4;
//cerr << (char*) bytes << endl;
//cerr << "many:" << many << endl;
				return many;
			}

			unsigned char operator[]( const size_t offset ) const
			{
				if ( offset > chunksize ) throw string("Bad chunk offset");
				return bytes[ offset ];
			}

			string operator()( const size_t much )
			{
				const size_t get( ( much > many ) ? many : much );
				string ret( (const char*) &bytes[ where ], get );
				where+=get;
				return ret;
			}
	
			private:
			unsigned char bytes[ chunksize ];
			size_t many;
			size_t where;
		};

		struct Matcher
		{
			Matcher() : matched( 0 )
			{
				memcpy( pattern, "\r\n\r\n", 4 );
			}

			bool operator()( const unsigned char c )
			{
//cerr << "C:" << c << ", matched:" << matched << ", pattern:" << sizeof(pattern) << endl;
				if ( pattern[ matched ] == c )
				{
					matched++;
					if ( matched == sizeof( pattern ) ) return true;
				} else {
					matched=0;
				}
				return false;
			}
			private:
			size_t matched;
			unsigned char pattern[ 4 ];
		};


		template < typename SocketType, int chunksize >
			struct SocketReader : vector< Chunk< SocketType, chunksize > > 
		{
			SocketReader( SocketType& _sock ) : sock( _sock ), ndx( 0 ) {}
			operator bool ()
			{
				size_t bread( 0 );
				do
				{
//usleep( 1e5 );
					Chunk< SocketType, chunksize > C; 
					push_back( C );
					Chunk< SocketType, chunksize >& chunk( this->back() );
					bread=chunk.read( sock );
				} while ( ! eomime( bread ) );
				return true;
			}
			string& Headers()
			{
//cerr << "ndx:" << ndx << endl;
				vector< Chunk< SocketType, chunksize > >& me( *this );
				size_t len( 0 );
				while ( len < ndx )
				{
					const size_t bucket( len / chunksize );
					const string what( me[ bucket ]( ndx - len ) );
					headers+=what;
					len+=what.size();
				}
				return headers;
			}
			string& Payload( const size_t len )
			{
				const size_t bucket( ndx / chunksize );
				vector< Chunk< SocketType, chunksize > >& me( *this );
				payload=me[ bucket ]( len );
				return payload;
			}
			private:

			unsigned char Next( const size_t where )
			{
				const vector< Chunk< SocketType, chunksize > >& me( *this );
				const size_t offset( where % chunksize );
				const size_t bucket( where / chunksize );
//cerr << "chunksize:" << chunksize << ", where:" << where << ", bucket:" << bucket << ", offset:" << offset << endl;
				return me[ bucket ][ offset ];
			}

			bool eomime( const size_t bread )
			{
				for ( size_t j=0;j<bread;j++ )
				{
//usleep( 1e5 );
					const unsigned char C( Next( ndx++ ) );
					if ( matcher( C ) )
					{
						
//cerr << "EOM" << endl << endl;
						return true;
					}
				}
//cerr << "NOEOM" << endl << endl;
				return false;
			}

			string headers, payload;
			SocketType& sock;
			Matcher matcher;
			size_t ndx;
		};
} // namespace KruncherMimes
#endif // KRUNCHER_MIME_READER_H


