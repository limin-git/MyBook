#include "StdAfx.h"
#include "Library.h"


void main(int argc, char* argv[])
{
    if ( 1 == argc )
    {
        std::cout << "请指定图书馆路径。" << std::endl;
        return;
    }
    else if ( 2 < argc )
    {
        std::cout << "参数太多。" << std::endl;
        return;
    }

    path p = argv[1];

    if ( ! boost::filesystem::exists( p ) )
    {
        std::cout << "不能访问：" << p.string() << std::endl;
        return;
    }

    Library my_library(p);
    std::cout << p.string () << std::endl;

    //Library my_library(  "D:\\My Book" );
    //Library my_library(  "C:\\Temp" );
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
            if ( 2 == args.size() && "rename" == args[0])
            {
                my_library.rename_remove_string( args[1] );
                continue;
            }
            else if ( 2 == args.size() && "regex_rename" == args[0])
            {
                my_library.rename_remove_string( args[1], true );
                continue;
            }

            if ( 1 == args.size() )
            {
                if ( boost::filesystem::is_directory( args[0] ) )
                {
                    my_library.add_directory( args[0] );
                }
                else
                {
                    my_library.is_book_exist( args[0], true );
                }
            }
            else if ( 2 == args.size() )
            {
                if ( my_library.is_clc_folder_name( boost::to_upper_copy(args[0]) ) )
                {
                    my_library.add_book( args[1], boost::to_upper_copy(args[0]) );
                }
                else
                {
                    my_library.add_book( args[0], boost::to_upper_copy(args[1]) );
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
