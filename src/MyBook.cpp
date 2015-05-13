#include "StdAfx.h"
#include "MyBook.h"


MyBook::MyBook( const path& p )
    : m_path( p ),
      m_regex( "^[-.0-9a-zA-Z]+((：|=)[-.0-9a-zA-Z]+)?" ),
      m_file_count( 0 ),
      m_folder_count( 0 )
{
    std::cout << "正在初始化：" << m_path.string() << " ";

    boost::filesystem::recursive_directory_iterator end; // default construction yields past-the-end
    boost::filesystem::recursive_directory_iterator it( m_path );

    for ( ; it != end; ++it )
    {
        if ( false == is_directory( it->status() ) )
        {
            m_file_count++;
            continue;
        }

        const path& p = *it;
        const std::string s = p.filename().string();
        boost::smatch m;

        if ( boost::regex_search( s, m, m_regex ) )
        {
            {
                // 检查是否为图书馆分类法目录
                path check = p;
                bool result = true;

                while ( check != m_path )
                {
                    if ( ! boost::regex_search( check.filename().string(), m_regex ) )
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
                m_folder_count++;
            }
            else
            {
                if ( find_it->second.filename().string().find( "依世界" ) != std::string::npos && is_parent_sub_folder( find_it->second, p ) )
                {
                    m_path_map[n] = p;
                }
                else
                {
                    std::cout << "文件夹名相同：" << p.string() << " <<==>> " << find_it->second.string() << std::endl;
                }
            }
        }
    }

    std::cout
        << "完成。" << std::endl
        << "共：" << m_file_count << " 个文件， " << m_folder_count << " 个文件夹。" << std::endl;

#if 0
    for ( std::map<std::string, path>::iterator it = m_path_map.begin(); it != m_path_map.end(); ++it )
    {
        std::cout << it->first << " : " << it->second << std::endl;
    }
#endif
}


bool MyBook::is_parent_sub_folder( const path& parent, const path& sub )
{
    path p = sub;

    while ( p.has_parent_path() )
    {
        p = p.parent_path();

        if ( p == parent )
        {
            return true;
        }
    }

    return false;
}


bool MyBook::is_folder_name( const std::string& folder_name )
{
    return boost::regex_match( folder_name, m_regex );
}


// 
bool MyBook::create_folder( const std::string& folder_name )
{
    std::map<std::string, path>::iterator it = m_path_map.find( folder_name );

    if ( it != m_path_map.end() )
    {
        return true;
    }

    std::cout << "创建新文件夹：" << folder_name << std::endl;

    path parent;
    path base_name;
    std::string sub_dirs;
    size_t size = folder_name.size();

    for ( size_t i = 0; i < size; ++i )
    {
        std::string n = folder_name.substr( 0, size - i );
        it = m_path_map.find( n );

        if ( it != m_path_map.end() )
        {
            sub_dirs = folder_name.substr( size - i );
            parent = it->second;
            base_name = it->first;
            break;
        }
    }

    if ( sub_dirs.empty() || parent.empty() )
    {
        std::cout << "失败，找不到父文件夹。" << std::endl;
        return false;
    }

    if ( ! boost::filesystem::exists( parent ) )
    {
        std::cout << "失败，父文件夹不存在：" << parent.string() << std::endl;
        return false;
    }

    path new_parent = parent;
    //std::cout << base_name.string() << std::endl;

    for ( size_t i = 0; i < sub_dirs.size(); ++i )
    {
        if ( sub_dirs[i] == '.' )
        {
            continue;
        }

        if ( sub_dirs[i] == '-' )
        {
            i = sub_dirs.size();
        }

        path new_name = path( base_name.string() + sub_dirs.substr(0, i + 1) );
        path new_path = new_parent / new_name;
        std::map<std::string, path>::iterator find_it = m_path_map.find( new_name.string() );

        if ( find_it != m_path_map.end() )
        {
            std::cout << "成功，但新文件夹已存在： " << new_name.string() << std::endl;
            return true;
        }

        if ( false == boost::filesystem::create_directory( new_path ) )
        {
            std::cout << "失败，系统错误：" << new_path.string() << std::endl;
            return false;
        }

        m_path_map[new_name.string()] = new_path;
        new_parent = new_path;
    }

    return true;
}


void MyBook::add_book( const path& book_path, const std::string& folder_name )
{
    if ( ! boost::filesystem::exists( book_path ) )
    {
        std::cout << "找不到：" << book_path.string() << std::endl;
        return;
    }

    if ( ! boost::regex_match( folder_name, m_regex ) )
    {
        std::cout << "不符合图书馆分类法："<< folder_name << std::endl;
        return;
    }

    std::map<std::string, path>::iterator it = m_path_map.find( folder_name );

    if ( it == m_path_map.end() )
    {
        if ( false == create_folder( folder_name ) )
        {
            return;
        }

        it = m_path_map.find( folder_name );
    }

    if ( it == m_path_map.end() )
    {
        return;
    }

    path new_book_path = it->second / book_path.filename();

    if ( boost::filesystem::exists( new_book_path ) )
    {
        std::cout << "已存在：" << new_book_path.string() << std::endl;
        return;
    }

    boost::system::error_code ec;
    boost::filesystem::rename( book_path, new_book_path );

    if ( ec )
    {
        std::cout << "移动文件失败：" << ec.message() << std::endl;
        return;
    }

    std::cout << "添加成功：" << new_book_path.string() << std::endl;
}


void MyBook::add_books( const path& folder )
{
    if ( ! boost::filesystem::exists( folder ) )
    {
        std::cout << "文件夹不存在：" << folder << std::endl;
        return;
    }

    if ( ! boost::filesystem::is_directory( folder ) )
    {
        std::cout << "不是文件夹：" << folder << std::endl;
        return;
    }

    boost::filesystem::directory_iterator end; // default construction yields past-the-end
    boost::filesystem::directory_iterator it( folder );

    for ( ; it != end; ++it )
    {
        const path& p = *it;

        if ( true == is_directory( it->status() ) )
        {
            add_books( p );
        }
        else
        {
            add_book( p, p.parent_path().filename().string() );
        }
    }

    boost::filesystem::remove( folder );
}
