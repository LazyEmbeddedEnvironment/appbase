//
// Created by jonb on 28.11.2018.
//
#pragma once

#include <appbase/application.hpp>
#include <iostream>
#include <boost/exception/diagnostic_information.hpp>

namespace bpo = boost::program_options;

using bpo::options_description;
using bpo::variables_map;
using std::string;
using std::vector;

class yarp_plugin : public appbase::plugin<yarp_plugin>{
public:
    APPBASE_PLUGIN_REQUIRES();

    virtual void set_program_options( options_description& cli, options_description& cfg ) override
    {
        cfg.add_options()
//                ("readonly", "open the database in read only mode")
                ("listen-endpoint", bpo::value<string>()->default_value( "127.0.0.1:9876" ), "The local IP address and port to listen for incoming connections.")
                ("public-endpoint", bpo::value<string>()->default_value( "0.0.0.0:9876" ), "The public IP address and port that should be advertized to peers.")
                ("dbsize", bpo::value<uint64_t>()->default_value( 8*1024 ), "Minimum size MB of database shared memory file")
                ;
        cli.add_options()
                ("port_name", bpo::value<std::string>()->default_value("test"), "If you like to manipulate pin name in yarp nameserver" )
                ;
    }

    void plugin_initialize( const variables_map& options ) { std::cout << "initialize "<< name()  << " instance " << instance() <<"\n" ; }
    void plugin_startup()  { std::cout << "starting "<< name() <<"\n"; }
    void plugin_shutdown() { std::cout << "shutdown "<< name() <<"\n"; }
private:
};
