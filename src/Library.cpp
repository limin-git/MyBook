#include "StdAfx.h"
#include "Library.h"


Library::Library( const path& p )
    : m_path( p ),
      m_regex( "^[A-Z][-.0-9A-Z]*((：|=)[-.0-9a-zA-Z]+)?" ),
      m_file_count( 0 ),
      m_folder_count( 0 ),
      m_current_top_class( 'A' )
{
    m_path_exist = ( false == m_path.empty() && true == boost::filesystem::exists( m_path ) );
    m_it = boost::filesystem::recursive_directory_iterator( m_path );
}


void Library::add_book( const path& book_path, const std::string& folder_name )
{
    if ( ! boost::filesystem::exists( book_path ) )
    {
        std::cout << "找不到：" << book_path.string() << std::endl;
        return;
    }

    if ( ! is_clc_folder_name( folder_name ) )
    {
        std::cout << "不符合图书馆分类法："<< folder_name << std::endl;
        return;
    }

    create_index( folder_name[0] );

    std::map<std::string, path>::iterator it = m_class_path_map.find( folder_name );

    if ( it == m_class_path_map.end() )
    {
        if ( false == creaate_clc_folder( folder_name ) )
        {
            std::cout << "创建文件夹失败。" << std::endl;
            return;
        }

        it = m_class_path_map.find( folder_name );
    }

    if ( it == m_class_path_map.end() )
    {
        std::cout << "找不到文件夹：" << folder_name << std::endl;
        return;
    }

    {
        std::map<std::string, path>::iterator find_it = m_file_path_map.find( book_path.filename().string() );

        if ( find_it != m_file_path_map.end() && find_it->second.parent_path() != it->second && find_it->second != book_path )
        {
            std::cout << "已存在，但路径不同：" << find_it->second.string() << std::endl;
        }
    }

    path new_book_path = it->second / book_path.filename();

    if ( new_book_path == book_path )
    {
        return;
    }

    if ( boost::filesystem::exists( new_book_path ) )
    {
        std::cout << "已存在：" << new_book_path.string() << std::endl;

        if ( is_equal_size( book_path, new_book_path ) )
        {
            std::cout << "大小相同。";
            remove_path( book_path, true );
            return;
        }

        for ( size_t i = 1; true ; ++i )
        {
            std::string post_fix = "(" + boost::lexical_cast<std::string>(i) + ")";
            new_book_path = it->second / ( book_path.stem().string() + post_fix + book_path.extension().string() );

            if ( ! boost::filesystem::exists( new_book_path ) )
            {
                std::cout << "大小不同，重命名为：" << new_book_path.filename().string() << std::endl;
                break;
            }
            else
            {
                if ( is_equal_size( book_path, new_book_path ) )
                {
                    std::cout << "大小相同。";
                    remove_path( book_path, true );
                    return;
                }
            }
        }
    }

    std::cout << "正在移动文件……";

    boost::system::error_code ec;
    boost::filesystem::rename( book_path, new_book_path, ec );

    if ( ec )
    {
        std::cout << "\r移动文件失败：" << ec.value() << ": " << ec.message() << std::endl;
        return;
    }

    // update cache
    m_file_path_map[book_path.filename().string()] = new_book_path;
    m_file_count++;
    std::cout << "\r成功：" << new_book_path.string() << std::endl;
}


void Library::add_directory( const path& dir )
{
    if ( ! boost::filesystem::exists( dir ) )
    {
        std::cout << "目录不存在：" << dir << std::endl;
        return;
    }

    if ( ! boost::filesystem::is_directory( dir ) )
    {
        std::cout << "不是目录：" << dir << std::endl;
        return;
    }

    if ( ! is_clc_folder_name( boost::to_upper_copy( dir.filename().string() ) ) )
    {
        std::cout << "不符合图书馆分类法：" << dir << std::endl;
        return;
    }

    boost::filesystem::directory_iterator end; // default construction yields past-the-end
    boost::filesystem::directory_iterator it( dir );

    for ( ; it != end; ++it )
    {
        const path& p = *it;

        if ( true == is_directory( it->status() ) )
        {
            add_directory( p );
        }
        else
        {
            add_book( p, boost::to_upper_copy( p.parent_path().filename().string() ) );
        }
    }

    remove_path( dir );
}


bool Library::creaate_clc_folder( const std::string& folder_name )
{
    std::map<std::string, path>::iterator it = m_class_path_map.find( folder_name );

    if ( it != m_class_path_map.end() )
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
        it = m_class_path_map.find( n );

        if ( it != m_class_path_map.end() )
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
        std::cout << "失败，找不到文件夹：" << parent.string() << std::endl;
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

        if ( sub_dirs[i] == '-' || '=' == sub_dirs[i] )
        {
            i = sub_dirs.size();
        }

        path new_name = path( base_name.string() + sub_dirs.substr(0, i + 1) );
        path new_path = new_parent / new_name;
        std::map<std::string, path>::iterator find_it = m_class_path_map.find( new_name.string() );

        if ( find_it != m_class_path_map.end() )
        {
            std::cout << "成功，但新文件夹已存在： " << new_name.string() << std::endl;
            return true;
        }

        if ( false == boost::filesystem::create_directory( new_path ) )
        {
            std::cout << "失败，系统错误：" << new_path.string() << std::endl;
            return false;
        }

        m_class_path_map[new_name.string()] = new_path;
        new_parent = new_path;
    }

    return true;
}


void Library::create_index( const char top_class_name )
{
    if ( false == m_path_exist )
    {
        return;
    }

    if ( top_class_name < m_current_top_class )
    {
        return;
    }

    for ( ; m_it != m_end; ++m_it )
    {
        if ( false == is_directory( m_it->status() ) )
        {
            std::string filename = m_it->path().filename().string();
            std::map<std::string, path>::iterator find_it = m_file_path_map.find( filename );

            if ( find_it == m_file_path_map.end() )
            {
                m_file_path_map[filename] = *m_it;
                m_file_count++;
            }

            continue;
        }

        const path& p = *m_it;
        const std::string filename = p.filename().string();
        boost::smatch m;

        if ( boost::regex_search( filename, m, m_regex ) )
        {
            if ( false == is_clc_directory(p) )
            {
                //std::cout << p.string() << std::endl;
                continue;
            }

            //std::cout << p.string() << std::endl;
            //std::cout << p.filename().string() << std::endl;
            std::string class_name = m.str();
            std::map<std::string, path>::iterator find_it = m_class_path_map.find( class_name );

            if ( find_it == m_class_path_map.end())
            {
                m_class_path_map[class_name] = p;
                m_folder_count++;
            }
            else
            {
                if ( find_it->second.filename().string().find( "依世界" ) != std::string::npos && is_parent_path( find_it->second, p ) )
                {
                    m_class_path_map[class_name] = p;
                }
                else
                {
                    std::cout << "文件夹名相同：" << p.string() << " <<==>> " << find_it->second.string() << std::endl;
                }
            }

            if ( m_current_top_class != class_name[0] )
            {
                std::cout << "索引：" << m_current_top_class << std::endl;
                m_current_top_class = class_name[0];

                if ( top_class_name < m_current_top_class )
                {
                    m_it++;
                    return;
                }
            }
        }
    }

    if ( m_it == m_end )
    {
        m_current_top_class++;
        std::cout << "索引完成，共 " << m_file_count << " 个文件，" << m_folder_count << " 个文件夹。" << std::endl;
    }
}


void Library::recreate_index()
{
    m_current_top_class = 'A';
    m_file_count = 0;
    m_folder_count = 0;
    m_class_path_map.clear();
    m_file_path_map.clear();
    m_path_exist = ( false == m_path.empty() && true == boost::filesystem::exists( m_path ) );
    m_it = boost::filesystem::recursive_directory_iterator( m_path );

    create_index();
}


bool Library::is_equal_size( const path& lhs, const path& rhs )
{
    boost::system::error_code ec1, ec2;
    boost::uintmax_t size_1 = boost::filesystem::file_size( lhs, ec1 );
    boost::uintmax_t size_2 = boost::filesystem::file_size( rhs, ec2 );

    if ( ec1 )
    {
        std::cout << "不能获取文件大小：" << lhs << ", 错误码：" << ec1.message() << std::endl;
        return false;
    }

    if ( ec2 )
    {
        std::cout << "不能获取文件大小：" << rhs << ", 错误码：" << ec2.message() << std::endl;
        return false;
    }

    return size_1 == size_2;
}


void Library::remove_path( const path& book_path, bool is_output )
{
    if ( true == is_output )
    {
        std::cout << "删除：" << book_path.string();
    }

    boost::system::error_code ec;
    boost::filesystem::remove( book_path, ec );

    if ( ec )
    {
        std::cout << "\r删除：" << book_path.string() << " 失败，错误码：" << ec.message() << std::endl;
    }
    else
    {
        if ( true == is_output )
        {
            std::cout << std::endl;
        }

        std::map<std::string, path>::iterator it = m_file_path_map.find( book_path.filename().string() );

        if ( it != m_file_path_map.end() && it->second == book_path )
        {
            m_file_path_map.erase( it );
        }
    }
}


void Library::rename_remove_string( const std::string& search, const std::string& replace )
{
    create_index();

    std::vector<std::pair<std::map<std::string, path>::iterator, path> > new_paths;

    for ( std::map<std::string, path>::iterator it = m_file_path_map.begin(); it != m_file_path_map.end(); ++it )
    {
        if ( it->first.find( search ) == std::string::npos )
        {
            continue;
        }

        std::string old_name = it->first;
        std::string new_name = it->first;

        boost::algorithm::replace_all( new_name, search, replace );
        boost::trim( new_name );

        if ( old_name == new_name )
        {
            continue;
        }

        path new_path = it->second.parent_path() / new_name;
        new_paths.push_back( std::make_pair( it, new_path ) );
        std::cout << std::setw(80) << std::setiosflags(std::ios::left) << new_name << "\t" << old_name << std::endl;
    }

    if ( new_paths.empty() )
    {
        return;
    }

    std::cout << "要重命名这 " << new_paths.size() << " 个文件吗（y/n）？" << std::endl;
    std::string command;
    std::getline( std::cin, command );

    if ( command == "y" || command == "yes" )
    {
        rename_impl( new_paths );
    }
}


void Library::rename_regex( const std::string& search, const std::string& replace )
{
    create_index();

    boost::regex e(search);
    std::vector<std::pair<std::map<std::string, path>::iterator, path> > new_paths;

    for ( std::map<std::string, path>::iterator it = m_file_path_map.begin(); it != m_file_path_map.end(); ++it )
    {
        std::string old_name = it->first;
        std::string new_name = it->first;
        std::string stem = it->second.stem().string();
        std::string extension = it->second.extension().string();

        if ( boost::regex_search( stem, e ) )
        {
            new_name = boost::regex_replace( stem, e, replace );
            boost::trim( new_name );
            new_name += extension;
        }

        if ( old_name == new_name )
        {
            continue;
        }

        path new_path = it->second.parent_path() / new_name;
        new_paths.push_back( std::make_pair( it, new_path ) );
        std::cout << std::setw(100) << std::setiosflags(std::ios::left) << new_name << "\t" << old_name << std::endl;
    }

    if ( new_paths.empty() )
    {
        return;
    }

    std::cout << "要重命名这 " << new_paths.size() << " 个文件吗（y/n）？" << std::endl;
    std::string command;
    std::getline( std::cin, command );

    if ( command == "y" || command == "yes" )
    {
        rename_impl( new_paths );
    }
}


void Library::rename_impl( const std::vector<std::pair<std::map<std::string, path>::iterator, path> >& new_paths )
{
    for ( size_t i = 0; i < new_paths.size(); ++i )
    {
        if ( boost::filesystem::exists( new_paths[i].second ) )
        {
            if ( is_equal_size( new_paths[i].first->second, new_paths[i].second ) )
            {
                remove_path( new_paths[i].first->second, true );
                m_file_path_map.erase( new_paths[i].first );
            }

            continue;
        }

        boost::system::error_code ec;
        boost::filesystem::rename( new_paths[i].first->second, new_paths[i].second, ec );

        if ( ec )
        {
            std::cout << "重命名失败，原因：" << ec.message() << "。" << new_paths[i].first->second.string() << std::endl;
        }
        else
        {
            m_file_path_map.erase( new_paths[i].first );
            m_file_path_map[ new_paths[i].second.filename().string() ] = new_paths[i].second;
            std::cout << new_paths[i].second.string() << std::endl;
        }
    }
}


bool Library::is_parent_path( const path& parent, const path& sub )
{
    if ( false == sub.has_parent_path() )
    {
        return false;
    }

    for ( path p = sub.parent_path(); p.has_parent_path(); p = p.parent_path() )
    {
        if ( p == parent )
        {
            return true;
        }
    }

    return false;
}


bool Library::is_clc_folder_name( const std::string& folder_name )
{
    return boost::regex_match( folder_name, m_regex );
}


bool Library::is_clc_directory( const path& dir )
{
    for ( path p = dir; p != m_path && p.has_parent_path(); p = p.parent_path() )
    {
        if ( ! boost::regex_search( p.filename().string(), m_regex ) )
        {
            return false;
        }
    }

    return true;
}


bool Library::is_valid_clc_directory( const path& p )
{
    if ( p.parent_path() == m_path )
    {
        return true;
    }

    std::string sub_name = p.filename().string();
    std::string parent_name = p.parent_path().filename().string();
    boost::smatch m;
    std::string sub_index;
    std::string parent_index;

    if ( boost::regex_search( sub_name, m, m_regex ) )
    {
        sub_index = m.str();
    }

    if ( boost::regex_search( parent_name, m, m_regex ) )
    {
        parent_index = m.str();
    }

    return sub_index.substr( 0, parent_index.size() ) == parent_index;
}


bool Library::is_book_exist( const path& book_path, bool is_output  )
{
    create_index();

    bool is_exist = false;
    const std::string book_name = book_path.stem().string();

    for ( std::map<std::string, path>::iterator it = m_file_path_map.begin(); it != m_file_path_map.end(); ++it )
    {
        if ( it->first.find( book_name ) != std::string::npos )
        {
            if ( true == is_output )
            {
                boost::system::error_code ec;
                boost::uintmax_t size = boost::filesystem::file_size( it->second, ec);

                if ( ec )
                {
                    std::cout << "不能获取 " << it->second.string() << " 的大小，错误码：" << ec.message() << std::endl;
                }
                else
                {                
                    std::locale ori = std::cout.imbue( std::locale("chs") );
                    std::cout << std::setiosflags(std::ios::right) << std::setw(7) << ::ceil( size / 1024.0 ) << " KB  " << it->second.string() << std::endl;
                    std::cout.imbue( ori );
                }
            }

            is_exist = true;
        }
    }

    return false;
}
