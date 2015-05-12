#include "StdAfx.h"
#include "MyBook.h"


MyBook::MyBook( const path& p )
    : m_path( p )
{
    static const boost::regex e
    (
        "^[-.0-9a-zA-Z]+((：|=)[-.0-9a-zA-Z]+)?"
    );

    boost::filesystem::recursive_directory_iterator end; // default construction yields past-the-end
    boost::filesystem::recursive_directory_iterator it( m_path );

    for ( ; it != end; ++it )
    {
        if ( false == is_directory( it->status() ) )
        {
            continue;
        }

        const path& p = *it;
        const std::string s = p.filename().string();
        boost::smatch m;

        if ( boost::regex_search( s, m, e ) )
        {
            {
                // 检查是否为图书馆分类法目录
                path check = p;
                bool result = true;

                while ( check != m_path )
                {
                    if ( ! boost::regex_search( check.filename().string(), e ) )
                    {
                        result = false;
                        //std::cout << check.string() << std::endl;
                        break;
                    }

                    check = check.parent_path();
                }

                // 不是
                if ( false == result )
                {
                    continue;
                }
            }

            //std::cout << p.string() << std::endl;
            //std::cout << p.filename().string() << std::endl;
            std::string n = m.str();
            std::map<std::string, path>::iterator find_it = m_path_map.find( n );

            if ( find_it == m_path_map.end() )
            {
                m_path_map[n] = p;
            }
            else
            {
                if ( find_it->second.filename().string().find( "依世界" ) != std::string::npos && left_is_parent_of_right( find_it->second, p ) )
                {
                    m_path_map[n] = p;
                }
                else
                {
                    std::cout << "CONFLICT_ERROR: " << p.string() << " <<==>> " << find_it->second.string() << std::endl;
                }
            }
        }
    }
}


bool MyBook::left_is_parent_of_right( const path& left, const path& right )
{
    path p = right;

    while ( p.has_parent_path() )
    {
        p = p.parent_path();

        if ( p == left )
        {
            return true;
        }
    }

    return false;
}


bool MyBook::create_sub_directories( const path& current, const std::string& sub_dirs )
{
    if ( ! boost::filesystem::exists( current ) )
    {
        std::cout << "not exist." << std::endl;
        return false;
    }

    path parent = current;
    path name = current.filename();
    //std::cout << name.string() << std::endl;

    for ( size_t i = 0; i < sub_dirs.size(); ++i )
    {
        if ( sub_dirs[i] == '.' || sub_dirs[i] == '-' )
        {
            continue;
        }
        
        path new_name = path( name.string() + sub_dirs.substr(0, i + 1) );
        path new_path = parent / new_name;
        std::map<std::string, path>::iterator find_it = m_path_map.find( new_name.string() );

        if ( find_it != m_path_map.end() )
        {
            std::cout << "exist." << std::endl;
            return true;
        }

        if ( false == boost::filesystem::create_directory( new_path ) )
        {
            std::cout << "create failed: " << new_path.string() << std::endl;
            return false;
        }

        m_path_map[new_name.string()] = new_path;
        parent = new_path;
    }

    return true;
}
