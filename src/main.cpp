#include "StdAfx.h"
#include "MyBook.h"


void main(int argc, char* argv[])
{
    MyBook mybook(  "D:\\My Book" );
    //MyBook mybook(  "C:\\Temp" );
    std::string line;
    static const boost::regex e
    (
        "(?x)"
        "[^\" ]+| \"([^\"]+)\""
    );

    while ( true )
    {
        std::cout << "添加新书：";
        std::getline( std::cin, line );

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
                mybook.add_books( args[0] );
            }
            else if ( 2 == args.size() )
            {
                if ( mybook.is_folder_name( args[0] ) )
                {
                    mybook.add_book( args[1], boost::to_upper_copy(args[0]) );
                }
                else
                {
                    mybook.add_book( args[0], boost::to_upper_copy(args[1]) );
                }
            }
            else
            {
                std::cout << "参数不正确：folder | path folder | folder path." << std::endl;
            }
        }
        catch ( ... )
        {
        }
    }
}
