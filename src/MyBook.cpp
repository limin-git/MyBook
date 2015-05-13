#include "StdAfx.h"
#include "MyBook.h"


MyBook::MyBook( const path& p )
    : m_path( p ),
      m_regex( "^[-.0-9a-zA-Z]+((��|=)[-.0-9a-zA-Z]+)?" ),
      m_file_count( 0 ),
      m_folder_count( 0 )
{
    std::cout << "���ڳ�ʼ����" << m_path.string();

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
            std::map<std::string, path>::iterator find_it = m_path_map.find( n );

            if ( find_it == m_path_map.end() )
            {
                m_path_map[n] = p;
                m_folder_count++;
            }
            else
            {
                if ( find_it->second.filename().string().find( "������" ) != std::string::npos && is_parent_sub_folder( find_it->second, p ) )
                {
                    m_path_map[n] = p;
                }
                else
                {
                    std::cout << "�ļ�������ͬ��" << p.string() << " <<==>> " << find_it->second.string() << std::endl;
                }
            }
        }
    }

    std::cout
        << " ��ɡ�" << std::endl
        << "����" << m_file_count << " ���ļ��� " << m_folder_count << " ���ļ��С�" << std::endl;

    for ( std::map<std::string, path>::iterator it = m_path_map.begin(); it != m_path_map.end(); ++it )
    {
        std::cout << it->first << " : " << it->second << std::endl;
    }
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


// 
bool MyBook::create_sub_directories( const std::string& folder_name )
{
    std::map<std::string, path>::iterator it = m_path_map.find( folder_name );

    if ( it != m_path_map.end() )
    {
        return true;
    }

    std::cout << "�������ļ��У�" << folder_name << std::endl;

    path parent;
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
        std::cout << "ʧ�ܣ����ļ��в����ڣ�" << parent.string() << std::endl;
        return false;
    }

    path new_parent = parent;
    path name = parent.filename();
    //std::cout << name.string() << std::endl;

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

        path new_name = path( name.string() + sub_dirs.substr(0, i + 1) );
        path new_path = new_parent / new_name;
        std::map<std::string, path>::iterator find_it = m_path_map.find( new_name.string() );

        if ( find_it != m_path_map.end() )
        {
            std::cout << "�ɹ��������ļ����Ѵ��ڣ� " << new_name.string() << std::endl;
            return true;
        }

        if ( false == boost::filesystem::create_directory( new_path ) )
        {
            std::cout << "ʧ�ܣ�ϵͳ����" << new_path.string() << std::endl;
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
        std::cout << "���鲻���ڣ�" << book_path.string() << std::endl;
        return;
    }

    if ( ! boost::regex_match( folder_name, m_regex ) )
    {
        std::cout << "�ļ�����������ͼ��ݷ��෨��"<< folder_name << std::endl;
        return;
    }

    std::map<std::string, path>::iterator it = m_path_map.find( folder_name );

    if ( it == m_path_map.end() )
    {
        if ( false == create_sub_directories( folder_name ) )
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

    std::cout << "��ӳɹ���" << new_book_path.string() << std::endl;
}
