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
namespace KruncherMimes
{
	using namespace std;
	using namespace KruncherTools;

	struct SocketManager 
	{
		virtual string& Headers() = 0;
		virtual const binarystring& Payload( const size_t len ) = 0;
		virtual operator bool () = 0;
		virtual void flush() = 0;
		virtual void write( const unsigned char*, size_t ) = 0;
	};

	struct ChunkBase
	{
		private:
		friend ostream& operator<<( ostream&, const ChunkBase&); 
		virtual ostream& operator<<( ostream&) const = 0;
	};
	inline ostream& operator<<( ostream& o, const ChunkBase& c ) { return c.operator<<(o);}

	template< typename SocketType, size_t chunksize >
		struct Chunk : ChunkBase
	{
		Chunk() : many( 0 ), where( 0 ), got( 0 ) { memset( bytes, 0, chunksize ); } 

		size_t read( SocketType& sock, const size_t much=chunksize )
		{
			const size_t get( min( chunksize, much ) );
			sock.read( (char*) bytes, get );
			many=sock.gcount();
			return many;
		}

		unsigned char operator[]( const size_t offset ) const
			{ return bytes[ offset ]; }

		binarystring operator()( const size_t much )
		{
			got=min( many, much );
			binarystring ret( (unsigned char*) &bytes[ where ], got );
			where+=got;
			return ret;
		}

		void operator()( binarystring& s, const size_t much )
		{
			got=min( many, much );
			s.append( &bytes[ where ], got );
			where+=got;
		}

		const size_t Got() const { return got; }

		void operator >> (binarystring& payload )
		{
			payload.append( &bytes[ 0 ], many );
		}
		private:
		ostream& operator<<( ostream& o ) const
		{
			o.write( (char*) &bytes[ 0 ], many );
			return o;
		}
		unsigned char bytes[ chunksize ];
		size_t many;
		size_t where;
		size_t got;
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
		struct SocketReadWriter 
			: vector< Chunk< SocketType, chunksize > > ,
				SocketManager
	{
		typedef Chunk< SocketType, chunksize > ChunkType;
		typedef vector< ChunkType > ChunksType;
		SocketReadWriter( SocketType& _sock ) : sock( _sock ), ndx( 0 ), HeaderReadLength( 0 ) {}
		virtual void flush() { sock.flush(); }
		virtual void write( const unsigned char* data, size_t datalen)
			{ sock.write( (char*) data, datalen ); }
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
				const binarystring what( me[ bucket ]( ndx - len ) );
				const pair<bool, size_t > nontext( what.nontext() );
				if ( nontext.first )
				{
					headers.append( (char*) what.data(), nontext.second ); break; 
					len+=nontext.second;
				} else {
					headers+=(char*)what.data();
					len+=what.size();
				}
			}
			HeaderReadLength=headers.size();
			const size_t eoh( headers.find( "\r\n\r\n" ) );
			if ( eoh!=string::npos ) 
			{
				headers.resize( eoh  );
				headers[ eoh ] = 0;
			} else headers.clear();
			
			return headers;
		}

		const binarystring& Payload( const size_t len )
		{
			const size_t L( len + HeaderReadLength );
			while ( ndx < L )
			{
				ChunkType C;
				push_back( C );
				ChunkType& chunk( this->back() );
				const size_t bread( chunk.read( sock, L-ndx ) );
				ndx+=bread;
				if ( bread != chunksize ) break;
			} 

			for ( typename ChunksType::iterator cit=ChunksType::begin(); cit!=this->end(); cit++ )
			{
				ChunkType& ch( *cit );
				ch >> payload;
			}

			payload.erase(0, headers.size() +strlen( "\r\n\r\n" ) );
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
			if ( ! bread ) return true;
			for ( size_t j=0;j<bread;j++ )
			{
				const unsigned char C( Next( ndx++ ) );
				if ( matcher( C ) )
					return true;
			}
			return false;
		}

		string headers;
		binarystring  payload;
		SocketType& sock;
		Matcher matcher;
		size_t ndx;
		size_t HeaderReadLength;
	};
} // namespace KruncherMimes
#endif // KRUNCHER_MIME_READER_H


