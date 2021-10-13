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
	template< typename SocketType, size_t chunksize >
		struct Chunk
	{
		Chunk() : many( 0 ), where( 0 ) { memset( bytes, 0, chunksize ); } 

		size_t read( SocketType& sock, const size_t much=chunksize )
		{
			const size_t get( min( chunksize, much ) );
			const size_t b4( sock.tellg() );
			sock.read( (char*) bytes, get );
			many=sock.gcount();
cerr << "\033[33mRead:" << many << "->\033[35m" << bytes << "\033[0m;" << endl;
			return many;
		}

		unsigned char operator[]( const size_t offset ) const
			{ return bytes[ offset ]; }

		string operator()( const size_t much )
		{
cerr << "Has:" << many-where << endl;
			const size_t get( min( many, much ) );

			string ret( (char*) &bytes[ where ], many-where );
cerr << "Got:\033[36m" << ret << "\033[0m;" << endl;;


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
			{ memcpy( pattern, "\r\n\r\n", 4 ); }

		bool operator()( const unsigned char c )
		{
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

	template < typename SocketType, size_t chunksize >
		struct SocketReader : vector< Chunk< SocketType, chunksize > > 
	{
		typedef Chunk< SocketType, chunksize > ChunkType;
		typedef vector< ChunkType > ChunksType;
		SocketReader( SocketType& _sock ) : sock( _sock ), ndx( 0 ) {}
		operator bool ()
		{
			size_t bread( 0 );
			do
			{
				ChunkType C;
				push_back( C );
				ChunkType& chunk( this->back() );
				bread=chunk.read( sock );
			} while ( ! eomime( bread ) );
			return true;
		}

		string& Headers()
		{
			ChunksType& me( *this );
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
			//size_t bucket( this->size() );
cerr << "Buckets:" << this->size() << endl;
			size_t bucket( ndx / chunksize );
			const size_t L( len + headers.size() );
			ChunksType& me( *this );
			while ( ndx < L )
			{
				ChunkType C;
				push_back( C );
				ChunkType& chunk( this->back() );
				const size_t bread( chunk.read( sock, L-ndx ) );
				ndx+=bread;
				if ( bread != chunksize ) break;
			} 
cerr << "BucketsNow:" << this->size() << endl;
			while ( payload.size() < len )
			{
cerr << "Adding bucket:" << bucket << endl;
cerr << "Need: " << len-payload.size() << endl;
				payload+=me[ bucket ]( len-payload.size() );
				bucket++;
			}
			return payload;
		}
		private:

		unsigned char Next( const size_t where )
		{
			const ChunksType& me( *this );
			const size_t offset( where % chunksize );
			const size_t bucket( where / chunksize );
			return me[ bucket ][ offset ];
		}

		bool eomime( const size_t bread )
		{
			for ( size_t j=0;j<bread;j++ )
			{
				const unsigned char C( Next( ndx++ ) );
				if ( matcher( C ) )
					return true;
			}
			return false;
		}

		string headers, payload;
		SocketType& sock;
		Matcher matcher;
		size_t ndx;
	};
} // namespace KruncherMimes
#endif // KRUNCHER_MIME_READER_H


