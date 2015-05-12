#include "StdAfx.h"
#include "Utility.h"


std::string Utility::get_string_from_file( const path& p, size_t lines )
{
    std::ifstream ifs( p.string().c_str() );

    if ( ! ifs )
    {
        std::cout << "\t" << "can not open file: " << p << std::endl;
        return "";
    }

    if ( 0 == lines)
    {
        return std::string( std::istreambuf_iterator< char >( ifs ), ( std::istreambuf_iterator< char >() ) );
    }
    else
    {
        std::string line;
        std::stringstream strm;

        while ( lines-- )
        {
            if ( ! std::getline( ifs, line ) )
            {
                break;
            }

            strm << line << std::endl;
        }

        return strm.str();
    }
}


std::ostream& Utility::output_paths( std::ostream& os, const std::set<path>& paths )
{
    for ( std::set<path>::const_iterator it = paths.begin(); it != paths.end(); ++it )
    {
        os << it->string() << std::endl;
    }

    return os;
}
