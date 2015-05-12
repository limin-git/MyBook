#include "StdAfx.h"
#include "MakeSDK.h"


MakeSDK::MakeSDK( ParameterPtr parameter )
{
    m_base = absolute(parameter->m_base);
    m_proj = absolute(parameter->m_proj);
    m_base.make_preferred();
    m_proj.make_preferred();
    m_is_remove = parameter->m_is_remove;

    // base
    m_baase_code                            = m_base / "code";
    m_base_build                            = m_base / "build";
    m_base_sdk                              = m_base / "sdk";

    // base\build
    m_base_build_win32_n                    = m_base_build / "win32_n";
    m_base_build_win32_nd                   = m_base_build / "win32_nd";
    m_base_build_win32_nr                   = m_base_build / "win32_nr";

    // base\code
    m_base_code_cots                        = m_baase_code / "cots";
    m_base_code_tools                       = m_baase_code / "tools";
    m_base_code_transactive                 = m_baase_code / "transactive";

    // base\sdk
    m_base_sdk_include                      = m_base_sdk / "include";
    m_base_sdk_lib                          = m_base_sdk / "lib";
    m_base_sdk_idl                          = m_base_sdk / "idl";
    m_base_sdk_omniORB                      = m_base_sdk / "omniORB";

    // base\sdk\include
    m_base_sdk_include_cots                 = m_base_sdk_include / "cots";
    m_base_sdk_include_transactive          = m_base_sdk_include / "transactive";

    // base\sdk\idl
    m_base_sdk_idl_transactive              = m_base_sdk_idl / "transactive";

    // project
    m_project_build                         = m_proj / "build";
    m_project_code                          = m_proj / "code";
    m_project_sdk                           = m_proj / "sdk";

    // project\build
    m_project_build_win32_n                 = m_project_build / "win32_n";
    m_project_build_win32_nd                = m_project_build / "win32_nd";
    m_project_build_win32_nr                = m_project_build / "win32_nr";

    // project\code
    m_project_code_cots                     = m_project_code / "cots";
    m_project_code_tools                    = m_project_code / "tools";
    m_project_code_transactive              = m_project_code / "transactive";
}


void MakeSDK::make()
{
    return ( m_is_remove ? remove_sdk() : make_sdk() );
}


void MakeSDK::make_sdk()
{
    create_or_remove_hard_link_for_base_build_win32_n();
    create_or_remove_directory_for_base_sdk();
    create_or_remove_directory_symbol_link();
    create_or_remove_hard_link_for_base_sdk_lib();
    std::cout << std::flush;
}


void MakeSDK::remove_sdk()
{
    create_or_remove_hard_link_for_base_sdk_lib( true );
    create_or_remove_directory_symbol_link( true );
    create_or_remove_directory_for_base_sdk( true );
    create_or_remove_hard_link_for_base_build_win32_n( true );
    std::cout << std::flush;
}


void MakeSDK::create_or_remove_hard_link_for_base_sdk_lib( bool is_remove )
{
    std::cout << ( is_remove ? "REMOVE" : "CREATE" ) << " HARD LINK FOR " << m_base_sdk_lib.string() << "\n";

    create_or_remove_hard_link( m_base_code_cots / "ACE/6_0_4/ACE_wrappers/lib/ACEd.lib", m_base_sdk_lib / "ACEd.lib", is_remove );
    create_or_remove_hard_link( m_base_code_cots / "ACE/6_0_4/ACE_wrappers/lib/ACEMFCd.lib", m_base_sdk_lib / "ACEMFCd.lib", is_remove );
    create_or_remove_hard_link( m_base_code_cots / "boost/boost_1_39_0/stage/lib/libboost_date_time-vc90-mt-gd-1_39.lib", m_base_sdk_lib / "libboost_date_time-vc90-mt-gd-1_39.lib", is_remove );
    create_or_remove_hard_link( m_base_code_cots / "boost/boost_1_39_0/stage/lib/libboost_filesystem-vc90-mt-gd-1_39.lib", m_base_sdk_lib / "libboost_filesystem-vc90-mt-gd-1_39.lib", is_remove );
    create_or_remove_hard_link( m_base_code_cots / "boost/boost_1_39_0/stage/lib/libboost_regex-vc90-mt-gd-1_39.lib", m_base_sdk_lib / "libboost_regex-vc90-mt-gd-1_39.lib", is_remove );
    create_or_remove_hard_link( m_base_code_cots / "boost/boost_1_39_0/stage/lib/libboost_system-vc90-mt-gd-1_39.lib", m_base_sdk_lib / "libboost_system-vc90-mt-gd-1_39.lib", is_remove );
    create_or_remove_hard_link( m_base_code_cots / "mcl/1.1.0/lib/winnt/TA_mcl_d.lib", m_base_sdk_lib / "TA_mcl_d.lib", is_remove );
    create_or_remove_hard_link( m_base_code_cots / "mysql/5.5.15/winnt/lib/libmysql.lib", m_base_sdk_lib / "libmysql.lib", is_remove );
    create_or_remove_hard_link( m_base_code_cots / "oracle/10.2.0/winnt/lib/oci.lib", m_base_sdk_lib / "oci.lib", is_remove );
    create_or_remove_hard_link( m_base_code_cots / "WinStackTrace/build/Debug_VS2008/stracelib.lib", m_base_sdk_lib / "stracelib.lib", is_remove );

    std::cout << "DONE" << "\n";
}


void MakeSDK::create_or_remove_directory_symbol_link( bool is_remove )
{
    std::cout << ( is_remove ? "REMOVE" : "CREATE" ) << " DIRECTORY SYMBOL LINK" << "\n";

    create_or_remove_directory_symbol_link( m_base_code_cots, m_base_sdk_include_cots, is_remove );
    create_or_remove_directory_symbol_link( m_base_code_transactive, m_base_sdk_include_transactive, is_remove );
    create_or_remove_directory_symbol_link( m_base_code_transactive, m_base_sdk_idl_transactive, is_remove );
    create_or_remove_directory_symbol_link( m_base_code_cots / "omniORB", m_base_sdk_omniORB, is_remove );
    create_or_remove_directory_symbol_link( m_base_sdk, m_project_sdk, is_remove );
    create_or_remove_directory_symbol_link( m_base_code_cots, m_project_code_cots, is_remove );

    std::cout << "DONE" << "\n";
}


void MakeSDK::create_or_remove_directory_for_base_sdk( bool is_remove )
{
    std::cout << ( is_remove ? "REMOVE" : "CREATE" ) << " DIRECTORY" << "\n";

    create_or_remove_directory( m_base_sdk_include, is_remove );
    create_or_remove_directory( m_base_sdk_lib, is_remove);
    create_or_remove_directory( m_base_sdk_idl, is_remove );

    std::cout << "DONE" << "\n";
}


void MakeSDK::create_or_remove_directory_symbol_link( const path& to, const path& from, bool is_remove )
{
    std::cout << "\t" << to.string() << " <<===>> " << from.string() << "\n";

    if ( false == remove_existance( from ) || true == is_remove  )
    {
        return;
    }

    boost::system::error_code ec;
    boost::filesystem::create_directory_symlink( to, from, ec );

    if ( ec )
    {
        std::cout << "\t" << ec.message() << "\n";
    }
}


bool MakeSDK::create_or_remove_directory( const path& dir, bool is_remove )
{
    std::cout << "\t" << dir.string() << "\n";

    if ( false == remove_existance( dir ) || true == is_remove  )
    {
        return false;
    }

    boost::system::error_code ec;
    bool result = boost::filesystem::create_directory( dir, ec );

    if ( ec )
    {
        std::cout << "\t" << ec.message() << "\n";
    }

    return result;
}


void MakeSDK::create_or_remove_hard_link_for_base_build_win32_n( bool is_remove )
{
    std::cout << ( is_remove ? "REMOVE" : "CREATE" ) << " HARD LINK FOR " << m_base_build_win32_nd.string() << "\n";

    const path dot_h( ".h" );
    const path dot_hh( ".hh" );
    const path dot_rc( ".rc" );
    recursive_directory_iterator end;

    std::vector<path> to_paths;

    for ( recursive_directory_iterator it( m_base_build_win32_n ); it != end; ++it )
    {
        const path ext = it->path().extension();

        if ( ext == dot_h || ext == dot_hh || ext == dot_rc )
        {
            to_paths.push_back( it->path() );
        }
    }

    static std::string baase_code           = m_baase_code.string();
    static std::string base_build_win32_n   = m_base_build_win32_n.string();

    for ( size_t i = 0; i < to_paths.size(); ++i )
    {
        std::string s = to_paths[i].string();
        boost::algorithm::replace_first( s, base_build_win32_n, baase_code );
        path new_hard_link = path(s);
        path& to = to_paths[i];
        std::cout << "\t" << to.string() << " <<===>> " << new_hard_link.string() << "\n";

        if ( false == remove_existance( new_hard_link ) || true == is_remove  )
        {
            continue;
        }

        boost::system::error_code ec;
        boost::filesystem::create_hard_link( to, new_hard_link, ec );

        if ( ec )
        {
            std::cout << "\t" << ec.message() << "\n";
        }
    }

    std::cout << "DONE" << "\n";
}


void MakeSDK::create_or_remove_hard_link( const path& to, const path& new_hard_link, bool is_remove )
{
    std::cout << "\t" << to.string() << " <<===>> " << new_hard_link.string() << "\n";

    if ( false == remove_existance( new_hard_link ) || true == is_remove  )
    {
        return;
    }

    boost::system::error_code ec;
    boost::filesystem::create_hard_link( to, new_hard_link, ec );

    if ( ec )
    {
        std::cout << "\t" << ec.message() << "\n";
    }
}


bool MakeSDK::remove_existance( const path& p )
{
    if ( exists( p ) )
    {
        boost::system::error_code ec;
        permissions(p, all_all);
        remove( p, ec);

        if ( ec )
        {
            std::cout << "\t" << ec.message() << "\n";
            return false;
        }
    }

    return true;
}


void MakeSDK::print_path( path p1, path p2, path p3 )
{
    if ( ! p1.empty() )
    {
        std::cout << p1.string() << "\n";
    }
    if ( ! p2.empty() )
    {
        std::cout << p2.string() << "\n";
    }
    if ( ! p3.empty() )
    {
        std::cout << p3.string() << "\n";
    }
}
