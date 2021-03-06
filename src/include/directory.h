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

#ifndef KRUNCHER_DIRECTORY_H
#define  KRUNCHER_DIRECTORY_H
#include <dirent.h>
namespace KruncherDirectory
{
	using namespace KruncherTools;

	inline string pathseparators( const string a, const string b )
	{
		const size_t als( a.rfind( "/" ) );
		const size_t bls( b.find( "/" ) );
		if ( ( als != a.size()-1) && ( bls != 0 ) )
			return a+string("/")+b;
		if ( ( als == a.size()-1) && ( bls == 0 ) )
			return a+b.substr(1,b.size()-1);
		return a+b;
	}

	inline bool DirectoryExists( const string pathname )
	{
		struct stat sb;
		if ( stat( pathname.c_str(), &sb ) != 0 ) return false;
		return S_ISDIR(sb.st_mode);
	}

	struct Directory : stringset
	{
		Directory() : recurse( false ) {}
		Directory( const string& _where ) : where( _where ), recurse( false ) {}
		Directory( const string& _where, const bool _recurse ) : where( _where ), recurse( _recurse ) {}
		virtual ~Directory(){}
		const Directory& operator = (const Directory& that ) const
		{
			if ( this == &that ) return *this;
			where=that.where;
			recurse=that.recurse;
			return *this;
		}
		void operator()( const string& _where, const bool _recurse )  const
		{
			where=_where;
			recurse=_recurse;
		}

		void operator()( const mode_t mode=0777 )
		{
	cerr << yellow << "Directory:" << where << fence;

			split( where, "/" );
			if ( empty() ) return;
			string start( *begin() );
			if ( start == "" )  start="/";
			stringstream sspath;
			for ( iterator it=begin();it!=end();it++ )
			{
				sspath<<*it;
				if ( sspath.str()!="/" ) sspath<<"/";
				if ( DirectoryExists( sspath.str() ) ) continue;
				cerr << "Creating " << sspath.str() << endl;
				mkdir( sspath.str().c_str(), mode );
			}
		}

		virtual operator bool ();
		const stringset& Directories() { return directories; }
		protected:
		mutable string where;
		mutable bool recurse;
		stringset directories;
		private:
		virtual Directory& NewSub( const string _where, const bool _recurse ) 
			{ throw string("No NewSub implemented for Directory" ); return *this;}
		virtual bool Filter( const dirent& ent ) const { return false; }
		virtual void operator+=( const dirent& );
		friend ostream& operator<<(ostream&,const Directory&);
		virtual ostream& operator<<(ostream& o) const
		{
			o << "[directories]" << endl << directories ;
			const stringset& me( *this );
			o << "[files]"  << endl << me;
			return o;
		}
	}; 

	inline ostream& operator<<(ostream& o,const Directory& m) { return m.operator<<(o); }

	inline void Directory::operator+=( const dirent& ent )
	{
		if ( Filter( ent ) ) return;
		if ( ent.d_type == DT_DIR )
			directories.insert( ent.d_name );
		else
			if ( ent.d_type == DT_REG )
				insert( ent.d_name );
	}

	inline Directory::operator bool ()
	{
		Directory& D( *this );
		struct dirent *pDirent( NULL ); 
		DIR* pDir( opendir (where.c_str() ) );
		if ( ! pDir )
		{
			 //throw string("Directory:" ) + where;
			Log( VERB_ALWAYS, "Cannot open directory", where ) ;
			return false;
		}
		pDirent = readdir(pDir);
		while ((pDirent = readdir(pDir)) != NULL) D+=*pDirent;
		closedir (pDir);

		for ( stringset::const_iterator dit=directories.begin();dit!=directories.end();dit++)
		{
			const string name( *dit );
			if ( name == "." ) continue;
			if ( name == ".." ) continue;
			const string subwhere( pathseparators( where, name ) );
			Directory& sub( NewSub( subwhere, recurse ) );
			if ( ! sub ) return false;
		}

		return true;
	}

	inline bool FileExists( const string filename )
	{
		if ( filename.empty() ) return false;
		if ( filename[ filename.size()-1 ] == '/' ) return false;
		struct stat sb;
		const bool ret( stat( filename.c_str(), &sb ) == 0 );
		if ( ret ) 
			Log( VERB_ALWAYS, "FileExists", filename );
		else
			Log( VERB_ALWAYS, "FileNotFound", filename );
		return ret;
	}



	inline size_t FileSize(const string filename)
	{
		struct stat sb;
		memset(&sb, 0, sizeof(sb));
		if (stat(filename.c_str(), &sb)==0) 
			return sb.st_size;
		else
			return 0;
	}

	inline string LoadFile(const string& filename, int maxlines=-1, int maxlinelen=-1 )
	{
		stringstream ss;
		ifstream in(filename.c_str());
		string line;
		if ( in.fail() ) throw string("Cannot read file:" ) + filename;
		while (!in.eof())
		{
			if ( maxlines != -1 )
				if ( !(maxlines--) ) throw string("file too long:" + filename );
			getline(in, line); 
			if ( maxlinelen !=-1 )
				if ( (int) line.size() > maxlinelen ) throw string("Line too long in file:" ) + filename + "->" + line ;
			ss << line << endl;
		}
		return ss.str();
	}

	inline void LoadFile(const string& filename, stringstream& ss, int maxlines=-1, int maxlinelen=-1)
	{
		ifstream in(filename.c_str());
		string line;
		if ( in.fail() ) throw string("Cannot read file:" ) + filename;
		while (!in.eof()) 
		{
			if ( maxlines != -1 )
				if ( !(maxlines--) ) throw string("file too long:" + filename );
			getline(in, line); 
			if ( maxlinelen !=-1 )
				if ( (int) line.size() > maxlinelen ) throw string("Line too long in file:" ) + filename + "->" + line ;
			ss << line << endl;
		}
	}

	inline void LoadBinaryFile(const string& filename, unsigned char* dest, size_t len )
	{
		ifstream in(filename.c_str());
		if ( in.fail() ) throw string("Cannot read binary file:" ) + filename;
		in.read( (char*) dest, len );
		if ( in.fail() ) throw string("Cannot read binary file:" ) + filename;
	}


} // namespace KruncherDirectory
#endif // KRUNCHER_DIRECTORY_H

