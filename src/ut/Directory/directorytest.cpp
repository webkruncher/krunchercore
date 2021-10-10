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

#include <infotools.h>
using namespace KruncherTools;
#include <directory.h>
using namespace KruncherDirectory;

namespace DirectoryTester
{
	struct Dir : Directory
	{
		Dir( const string& _where, const bool _recurse ) : Directory( _where, _recurse ) {}
		Directory& NewSub( const string _where, const bool _recurse );
		private:
		vector< Dir > subs;
		friend ostream& operator<<(ostream&,const Dir&);
		ostream& operator<<(ostream& o) const
		{
			o << "[directories]" << endl << directories ;
			const stringvector& me( *this );
			o << "[files]"  << endl << me;
			for ( vector< Dir >::const_iterator sit=subs.begin();sit!=subs.end(); sit++ )
			{
				const Dir& sub( *sit );
				o << sub;
			}
			return o;
		}
	}; 

	inline ostream& operator<<(ostream& o,const Dir& m) { return m.operator<<(o); }

	inline Directory& Dir::NewSub( const string _where, const bool _recurse )
	{
		Dir tmp( _where, recurse );
		subs.push_back( tmp );
		return subs.back();
	}

	int Simple( int, char** )
	{
		Dir dir( "/home/jmt/Info", true );
		if ( ! dir ) return -1;
		cerr << dir ;
		return 0;
	}
} // DirectoryTester
 



