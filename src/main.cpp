#include "StdAfx.h"
#include "Library.h"


void main(int argc, char* argv[])
{
    Library library(  "D:\\My Book" );
    //Library library(  "C:\\Temp" );
    std::string line;
    static const boost::regex e
    (
        "(?x)"
        "[^\" ]+| \"([^\"]+)\""
    );

    while ( true )
    {
        std::cout << "添加：";
        std::getline( std::cin, line );

        if ( line.empty() )
        {
            continue;
        }

        std::vector<std::string> args;
        boost::sregex_iterator it( line.begin(), line.end(), e );
        boost::sregex_iterator end;

        for ( ; it != end; ++it )
        {
            std::string arg = it->str();
            boost::algorithm::replace_all( arg, "\"", "" );
            args.push_back( arg );
        }

        try
        {
            if ( 1 == args.size() )
            {
                if ( boost::filesystem::is_directory( args[0] ) )
                {
                    library.add_directory( args[0] );
                }
                else
                {
                    library.is_book_exist( args[0], true );
                }
            }
            else if ( 2 == args.size() )
            {
                if ( library.is_clc_folder_name( boost::to_upper_copy(args[0]) ) )
                {
                    library.add_book( args[1], boost::to_upper_copy(args[0]) );
                }
                else
                {
                    library.add_book( args[0], boost::to_upper_copy(args[1]) );
                }
            }
            else
            {
                std::cout << "参数不正确：folder | path folder | folder path." << std::endl;
            }
        }
        catch ( ... )
        {
            std::cout << "异常。" << std::endl;
        }
    }
}
