#include "StdAfx.h"
#include "Library.h"


Library::Library( const path& p )
    : m_path( p ),
      m_regex( "^[A-Z][-.0-9A-Z]*((��|=)[-.0-9a-zA-Z]+)?" ),
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
        std::cout << "�Ҳ�����" << book_path.string() << std::endl;
        return;
    }

    if ( ! is_clc_folder_name( folder_name ) )
    {
        std::cout << "������ͼ��ݷ��෨��"<< folder_name << std::endl;
        return;
    }

    create_index( folder_name[0] );

    std::map<std::string, path>::iterator it = m_class_path_map.find( folder_name );

    if ( it == m_class_path_map.end() )
    {
        if ( false == creaate_clc_folder( folder_name ) )
        {
            std::cout << "�����ļ���ʧ�ܡ�" << std::endl;
            return;
        }

        it = m_class_path_map.find( folder_name );
    }

    if ( it == m_class_path_map.end() )
    {
        std::cout << "�Ҳ����ļ��У�" << folder_name << std::endl;
        return;
    }

    {
        std::map<std::string, path>::iterator find_it = m_file_path_map.find( book_path.filename().string() );

        if ( find_it != m_file_path_map.end() && find_it->second.parent_path() != it->second  )
        {
            std::cout << "�Ѵ��ڣ���·����ͬ��" << find_it->second.string() << std::endl;
        }
    }

    path new_book_path = it->second / book_path.filename();

    if ( boost::filesystem::exists( new_book_path ) )
    {
        std::cout << "�Ѵ��ڣ�" << new_book_path.string() << std::endl;

        if ( is_equal_size( book_path, new_book_path ) )
        {
            std::cout << "��С��ͬ��ɾ���ļ���" << book_path.string() << std::endl;
            remove_path( book_path );
            return;
        }

        for ( size_t i = 1; true ; ++i )
        {
            std::string post_fix = "(" + boost::lexical_cast<std::string>(i) + ")";
            new_book_path = it->second / ( book_path.stem().string() + post_fix + book_path.extension().string() );

            if ( ! boost::filesystem::exists( new_book_path ) )
            {
                std::cout << "��С��ͬ��������Ϊ��" << new_book_path.filename().string() << std::endl;
                break;
            }
            else
            {
                if ( is_equal_size( book_path, new_book_path ) )
                {
                    std::cout << "��С��ͬ��ɾ���ļ���" << book_path.string() << std::endl;
                    remove_path( book_path );
                    return;
                }
            }
        }
    }

    std::cout << "�����ƶ��ļ�����";

    boost::system::error_code ec;
    boost::filesystem::rename( book_path, new_book_path, ec );

    if ( ec )
    {
        std::cout << "\r�ƶ��ļ�ʧ�ܣ�" << ec.message() << std::endl;
        return;
    }

    // update cache
    m_file_path_map[book_path.filename().string()] = new_book_path;
    m_file_count++;
    std::cout << "\r�ɹ���" << new_book_path.string() << std::endl;
}


void Library::add_directory( const path& dir )
{
    if ( ! boost::filesystem::exists( dir ) )
    {
        std::cout << "Ŀ¼�����ڣ�" << dir << std::endl;
        return;
    }

    if ( ! boost::filesystem::is_directory( dir ) )
    {
        std::cout << "����Ŀ¼��" << dir << std::endl;
        return;
    }

    if ( ! is_clc_folder_name( boost::to_upper_copy( dir.filename().string() ) ) )
    {
        std::cout << "������ͼ��ݷ��෨��" << dir << std::endl;
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

    std::cout << "�������ļ��У�" << folder_name << std::endl;

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
        std::cout << "ʧ�ܣ��Ҳ������ļ��С�" << std::endl;
        return false;
    }

    if ( ! boost::filesystem::exists( parent ) )
    {
        std::cout << "ʧ�ܣ��Ҳ����ļ��У�" << parent.string() << std::endl;
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
            std::cout << "�ɹ��������ļ����Ѵ��ڣ� " << new_name.string() << std::endl;
            return true;
        }

        if ( false == boost::filesystem::create_directory( new_path ) )
        {
            std::cout << "ʧ�ܣ�ϵͳ����" << new_path.string() << std::endl;
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
        const std::string s = p.filename().string();
        boost::smatch m;

        if ( boost::regex_search( s, m, m_regex ) )
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
                if ( find_it->second.filename().string().find( "������" ) != std::string::npos && is_parent_path( find_it->second, p ) )
                {
                    m_class_path_map[class_name] = p;
                }
                else
                {
                    std::cout << "�ļ�������ͬ��" << p.string() << " <<==>> " << find_it->second.string() << std::endl;
                }
            }

            if ( m_current_top_class != class_name[0] )
            {
                std::cout << "������" << m_current_top_class << std::endl;
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
        std::cout << "������ɣ��� " << m_file_count << " ���ļ���" << m_folder_count << " ���ļ��С�" << std::endl;
    }
}


bool Library::is_equal_size( const path& lhs, const path& rhs )
{
    boost::system::error_code ec1, ec2;
    boost::uintmax_t size_1 = boost::filesystem::file_size( lhs, ec1 );
    boost::uintmax_t size_2 = boost::filesystem::file_size( rhs, ec2 );

    if ( ec1 )
    {
        std::cout << "���ܻ�ȡ�ļ���С��" << lhs << ", �����룺" << ec1.message() << std::endl;
        return false;
    }

    if ( ec2 )
    {
        std::cout << "���ܻ�ȡ�ļ���С��" << rhs << ", �����룺" << ec2.message() << std::endl;
        return false;
    }

    return size_1 == size_2;
}


void Library::remove_path( const path& book_path )
{
    boost::system::error_code ec;
    boost::filesystem::remove( book_path, ec );

    if ( ec )
    {
        std::cout << "ɾ����" << book_path.string() << " ʧ�ܣ������룺" << ec.message() << std::endl;
    }
}


void Library::rename_remove_string( const std::string& s, bool is_regex )
{
    create_index();

    std::vector<std::map<std::string, path>::iterator> to_be_rename;

    if ( false == is_regex )
    {
        for ( std::map<std::string, path>::iterator it = m_file_path_map.begin(); it != m_file_path_map.end(); ++it )
        {
            if ( it->first.find( s ) != std::string::npos )
            {
                to_be_rename.push_back( it );
            }
        }
    }
    else
    {
        boost::regex e(s);

        for ( std::map<std::string, path>::iterator it = m_file_path_map.begin(); it != m_file_path_map.end(); ++it )
        {
            std::string s = path(it->first).stem().string();

            if ( boost::regex_search( s, e ) )
            {
                to_be_rename.push_back( it );
            }
        }
    }

    if ( true == to_be_rename.empty() )
    {
        return;
    }

    std::cout
        << std::endl
        << "�ҵ� " << to_be_rename.size() << " ���ļ���" << std::endl
        << "---------------" << std::endl;

    std::vector<path> new_paths;

    for ( size_t i = 0; i < to_be_rename.size(); ++i )
    {
        std::string old_name = to_be_rename[i]->second.filename().string();
        std::string new_name = to_be_rename[i]->second.filename().string();

        if ( false == is_regex )
        {
            boost::algorithm::replace_all( new_name, s, "" );
        }
        else
        {
            boost::regex e(s);
            std::string s = path(old_name).stem().string();
            new_name = boost::regex_replace( s, e, "" ) + path(old_name).extension().string();
        }

        const path& old_path = to_be_rename[i]->second;
        path new_path = to_be_rename[i]->second.parent_path() / new_name;

        if ( boost::filesystem::exists( new_path ) )
        {
            for ( size_t i = 1; true; ++i)
            {
                std::string stem = path(new_name).stem().string();
                std::string extension = path(new_name).extension().string();
                std::string post_fix = "(" + boost::lexical_cast<std::string>(i) + ")";
                new_name = stem + post_fix + extension;
                new_path = to_be_rename[i]->second.parent_path() / new_name;

                if ( ! boost::filesystem::exists( new_path ) && new_name != old_name )
                {
                    break;
                }
            }
        }

        new_paths.push_back( new_path );
        std::cout << std::setw(100) << std::setiosflags(std::ios::left) << new_name << old_name << " : " << std::endl;
    }

    std::cout << std::endl << "y/n?" << std::endl;
    std::string command;
    std::cin >> command;

    if ( command != "y" )
    {
        return;
    }

    for ( size_t i = 0; i < to_be_rename.size(); ++i )
    {
        boost::system::error_code ec;
        boost::filesystem::rename( to_be_rename[i]->second, new_paths[i], ec );

        if ( ec )
        {
            std::cout << "������ʧ�ܣ�ԭ��" << ec.message() << "��" << to_be_rename[i]->second.string() << std::endl;
        }
        else
        {
            m_file_path_map.erase( to_be_rename[i] );
            m_file_path_map[ new_paths[i].filename().string() ] = new_paths[i];
            std::cout << new_paths[i].string() << std::endl;
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
                    std::cout << "���ܻ�ȡ " << it->second.string() << " �Ĵ�С�������룺" << ec.message() << std::endl;
                }
                else
                {                
                    std::locale ori = std::cout.imbue( std::locale("chs") );
                    std::cout << std::setw(7) << ::ceil( size / 1024.0 ) << " KB  " << it->second.string() << std::endl;
                    std::cout.imbue( ori );
                }
            }

            is_exist = true;
        }
    }

    return false;
}
