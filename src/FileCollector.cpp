#include "StdAfx.h"
#include "FileCollector.h"
#include "Utility.h"


FileCollector::FileCollector( const path& p, const path& current_path, const std::vector<path>& additional_directories )
    : m_path( p ),
      m_current_path( current_path ),
      m_additional_directories( additional_directories )
{
    std::cout << m_path.string() << std::endl;
    simple_collect();
    //recursive_collect();
}


const std::vector<path>& FileCollector::collect_from_file( const path& p )
{
    struct CollectFromFileThread
    {
        CollectFromFileThread( const path& p, std::map< path, std::vector<path> >& m, boost::mutex& mutex, boost::condition_variable& con )
            : m_path( p ), m_map( m ), m_mutex( mutex ), m_condition( con )
        {
        }

        void operator()()
        {
            std::string s = Utility::get_string_from_file( m_path, 200 );
            //std::string s = Utility::get_string_from_file( m_path );
            std::vector<path> paths;
            static const boost::regex e
            (
                "(?x)"
                "^ [ \t]* \\#include [ \t]+ [\"<] ([^\"<>]+) [\">]"
            );
            boost::sregex_iterator it( s.begin(), s.end(), e );
            boost::sregex_iterator end;

            for ( ; it != end; ++it )
            {
                paths.push_back( it->str(1) );
            }

            boost::unique_lock<boost::mutex> lock( m_mutex );
            m_map[m_path] = paths;
            m_condition.notify_all();
        }

        const path& m_path;
        boost::condition_variable& m_condition;
        boost::mutex& m_mutex;
        std::map< path, std::vector<path> >& m_map;
    };

    static std::map< path, std::vector<path> > s_file_includes_map;
    static boost::mutex s_mutex;
    static boost::condition_variable s_condition;

    boost::unique_lock<boost::mutex> lock( s_mutex );
    std::map< path, std::vector<path> >::iterator it = s_file_includes_map.find( p );

    if ( it == s_file_includes_map.end() )
    {
        CollectFromFileThread data( p, s_file_includes_map, s_mutex, s_condition );
        boost::thread t( data );
    }

    while ( it == s_file_includes_map.end() )
    {
        s_condition.wait( lock );
        it = s_file_includes_map.find( p );
    }

    return it->second;
}


path FileCollector::search_path( const path& include, const path& parent )
{
    if ( include.is_complete() )
    {
        return include;
    }

    {
        path p = parent / include;

        if ( boost::filesystem::exists( p ) && ! is_directory( p ) )
        {
            return boost::filesystem::system_complete(p);
        }
    }

    {
        path p = m_current_path / include;

        if ( boost::filesystem::exists( p ) && ! is_directory( p ) )
        {
            return boost::filesystem::system_complete(p);
        }
    }

    for ( size_t i = 0; i < m_additional_directories.size(); ++i )
    {
        path p;

        if ( m_additional_directories[i].is_absolute() )
        {
            p = m_additional_directories[i] / include;
        }
        else
        {
            p = m_current_path / m_additional_directories[i] / include;
        }

        if ( boost::filesystem::exists( p ) && ! is_directory(p) )
        {
            return boost::filesystem::system_complete(p);
        }
    }

    return path();
}


void FileCollector::recursive_collect()
{
    m_queue.push( m_path );

    while ( false == m_queue.empty() )
    {
        path p = m_queue.front();
        //std::cout << p.string() << std::endl;
        m_queue.pop();
        m_includes.insert( p );
        path parent = p.parent_path();
        const std::vector<path>& includes = collect_from_file( p );

        for ( size_t i = 0; i < includes.size(); ++i )
        {
            path include = search_path( includes[i], parent );

            if ( include.empty() )
            {
                m_includes.insert( includes[i] );
            }
            else
            {
                if ( m_includes.find( include ) == m_includes.end() )
                {
                    m_includes.insert( include );
                    m_queue.push( include );
                }
            }
        }
    }
}


void FileCollector::simple_collect()
{
    path parent = m_path.parent_path();
    const std::vector<path>& includes = collect_from_file( m_path );

    for ( size_t i = 0; i < includes.size(); ++i )
    {
        path include = search_path( includes[i], parent );

        if ( false == include.empty() )
        {
            m_includes.insert( include );
        }
        else
        {
            m_includes.insert( includes[i] );
        }
    }
}


boost::shared_ptr<boost::thread>  FileCollector::create_FileCollectorThread( std::set<path>& includes, const path& p, const path& current_path, const std::vector<path>& additional )
{
    struct FileCollectorThread
    {
        FileCollectorThread( std::set<path>& includes, const path& p, const path& current_path, const std::vector<path>& additional_directories )
            : m_includes( includes ), m_path( p ), m_current_path( current_path ), m_additional_directories( additional_directories )
        {
        }

        void operator()()
        {
            FileCollector collector( m_path, m_current_path, m_additional_directories );
            m_includes = collector.m_includes;
        }

        const path& m_path;
        const path& m_current_path;
        std::set<path>& m_includes;
        const std::vector<path>& m_additional_directories;
    };

    FileCollectorThread data( includes, p, current_path, additional );
    return boost::shared_ptr<boost::thread>( new boost::thread( data ) );
}
