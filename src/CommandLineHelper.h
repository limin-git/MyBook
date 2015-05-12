#pragma once
#include "Parameter.h"


struct CommandLineHelper
{
public:

    CommandLineHelper( int argc, char* argv[] );

    ParameterPtr get_parameter() { return m_parameter; }

private:

    bool is_remove( const char* str );
    void display_usage();

private:

    ParameterPtr m_parameter;
};
