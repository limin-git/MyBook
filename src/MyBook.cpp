#include "StdAfx.h"
#include "MyBook.h"


MyBook::MyBook( const path& p )
    : m_path( p ),
      m_regex( "^[-.0-9a-zA-Z]+((��|=)[-.0-9a-zA-Z]+)?" ),
      m_file_count( 0 ),
      m_folder_count( 0 ),
      m_top( 'A' )
{
    m_path_valid = ( false == m_path.empty() && true == boost::filesystem::exists( m_path ) );
    m_it = boost::filesystem::recursive_directory_iterator( m_path );
}


bool MyBook::is_parent_sub_path( const path& parent, const path& sub )
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


bool MyBook::is_valid_path( const path& p )
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


bool MyBook::is_book_exist( const path& file_path, bool is_output  )
{
    std::string filename = file_path.filename().string();

    std::map<std::string, path>::iterator it = m_file_path_map.find( filename );

    if ( it != m_file_path_map.end() )
    {
        if ( true == is_output )
        {
            std::cout << it->second.string() << std::endl;
        }

        return true;
    }

    return false;
}


// 
bool MyBook::create_folder( const std::string& folder_name )
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

        if ( sub_dirs[i] == '-' )
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


void MyBook::add_book( const path& book_path, const std::string& folder_name )
{
    if ( ! boost::filesystem::exists( book_path ) )
    {
        std::cout << "�Ҳ�����" << book_path.string() << std::endl;
        return;
    }

    if ( ! boost::regex_match( folder_name, m_regex ) )
    {
        std::cout << "������ͼ��ݷ��෨��"<< folder_name << std::endl;
        return;
    }

    initialize( folder_name );

    std::map<std::string, path>::iterator it = m_class_path_map.find( folder_name );

    if ( it == m_class_path_map.end() )
    {
        if ( false == create_folder( folder_name ) )
        {
            std::cout << "�����ļ���ʧ�ܣ�����������飺" << book_path.string() << std::endl;
            return;
        }

        it = m_class_path_map.find( folder_name );
    }

    if ( it == m_class_path_map.end() )
    {
        std::cout << "�Ҳ����ļ��У�" << folder_name << std::endl;
        return;
    }

    path new_book_path = it->second / book_path.filename();

    if ( boost::filesystem::exists( new_book_path ) )
    {
        std::cout << "�Ѵ��ڣ�" << new_book_path.string() << std::endl;
        return;
    }

    boost::system::error_code ec;
    boost::filesystem::rename( book_path, new_book_path );

    if ( ec )
    {
        std::cout << "�ƶ��ļ�ʧ�ܣ�" << ec.message() << std::endl;
        return;
    }

    // update cache
    m_file_path_map[book_path.filename().string()] = new_book_path;
    std::cout << "�ɹ���" << new_book_path.string() << std::endl;
}


void MyBook::add_folder( const path& folder )
{
    if ( ! boost::filesystem::exists( folder ) )
    {
        std::cout << "�ļ��в����ڣ�" << folder << std::endl;
        return;
    }

    if ( ! boost::filesystem::is_directory( folder ) )
    {
        std::cout << "�����ļ��У�" << folder << std::endl;
        return;
    }

    if ( ! is_folder_name( folder.filename().string() ) )
    {
        std::cout << "������ͼ��ݷ��෨��" << folder << std::endl;
        return;
    }

    boost::filesystem::directory_iterator end; // default construction yields past-the-end
    boost::filesystem::directory_iterator it( folder );

    for ( ; it != end; ++it )
    {
        const path& p = *it;

        if ( true == is_directory( it->status() ) )
        {
            add_folder( p );
        }
        else
        {
            add_book( p, p.parent_path().filename().string() );
        }
    }

    boost::filesystem::remove( folder );
}


void MyBook::initialize( const std::string& folder_name )
{
    if ( false == m_path_valid )
    {
        return;
    }

    if ( folder_name.size() && folder_name[0] < m_top )
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
            else
            {
                //std::cout << "�ļ����ظ���" << m_it->path().string() << " <<==>> " << find_it->second.string() << std::endl;
            }

            continue;
        }

        const path& p = *m_it;
        const std::string s = p.filename().string();
        boost::smatch m;

        if ( boost::regex_search( s, m, m_regex ) )
        {
            {
                // ����Ƿ�Ϊͼ��ݷ��෨Ŀ¼
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

                // ����
                if ( false == result )
                {
                    continue;
                }
            }

            //std::cout << p.string() << std::endl;
            //std::cout << p.filename().string() << std::endl;
            std::string n = m.str();
            std::map<std::string, path>::iterator find_it = m_class_path_map.find( n );

            if ( find_it == m_class_path_map.end())
            {
                m_class_path_map[n] = p;
                m_folder_count++;
            }
            else
            {
                if ( find_it->second.filename().string().find( "������" ) != std::string::npos && is_parent_sub_path( find_it->second, p ) )
                {
                    m_class_path_map[n] = p;
                }
                else
                {
                    std::cout << "�ļ�������ͬ��" << p.string() << " <<==>> " << find_it->second.string() << std::endl;
                }
            }

            if ( n.size() && m_top != n[0] )
            {
                m_top = n[0];
                std::cout << "������ɣ�" << n[0] << std::endl;

                if ( folder_name.size() && folder_name[0] < m_top )
                {
                    m_it++;
                    return;
                }
            }
        }
    }

    if ( m_it == m_end )
    {
        m_top = 'z';
        std::cout << "��ʼ����ɣ��� " << m_file_count << " ���ļ���" << m_folder_count << " ���ļ��С�" << std::endl;
    }
}
