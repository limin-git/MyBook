#pragma once
#include "Parameter.h"


using namespace boost::filesystem;


struct MakeSDK
{
public:

    MakeSDK( ParameterPtr parameter );

    void make();

private:

    void make_sdk();
    void remove_sdk();

private:

    void create_or_remove_hard_link_for_base_sdk_lib( bool is_remove = false );
    void create_or_remove_directory_symbol_link( bool is_remove = false );
    void create_or_remove_directory_for_base_sdk( bool is_remove = false );
    void create_or_remove_directory_symbol_link( const path& to, const path& from, bool is_remove = false );
    bool create_or_remove_directory( const path& dir, bool is_remove = false );
    void create_or_remove_hard_link_for_base_build_win32_n( bool is_remove = false );
    void create_or_remove_hard_link( const path& to, const path& new_hard_link, bool is_remove = false );

private:

    bool remove_existance( const path& p );
    void print_path( path p1 = path(), path p2 = path(), path p3 = path() );

private:

    path m_base;
    path m_proj;

    // base
    path m_baase_code;
    path m_base_build;
    path m_base_sdk;

    // base\build
    path m_base_build_win32_n;
    path m_base_build_win32_nd;
    path m_base_build_win32_nr;

    // base\code
    path m_base_code_cots;
    path m_base_code_tools;
    path m_base_code_transactive;

    // base\sdk
    path m_base_sdk_include;
    path m_base_sdk_lib;
    path m_base_sdk_idl;
    path m_base_sdk_omniORB;

    // base\sdk\include
    path m_base_sdk_include_cots;
    path m_base_sdk_include_transactive;

    // base\sdk\idl
    path m_base_sdk_idl_transactive;

    // project
    path m_project_build;
    path m_project_code;
    path m_project_sdk;

    // project\build
    path m_project_build_win32_n;
    path m_project_build_win32_nd;
    path m_project_build_win32_nr;

    // project\code
    path m_project_code_cots;
    path m_project_code_tools;
    path m_project_code_transactive;

    bool m_is_remove;
};
