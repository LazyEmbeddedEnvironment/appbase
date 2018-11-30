//
// Created by jonb on 28.11.2018.
//
#pragma once

#include <appbase/application.hpp>
#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <yarp/os/all.h>
#include <memory>

namespace bpo = boost::program_options;

using bpo::options_description;
using bpo::variables_map;
using std::string;
using std::vector;
using yarp::os::Network;
using yarp::os::Port;
class DataProcessor : public yarp::os::PortReader {
    virtual bool read(yarp::os::ConnectionReader& connection) {
        _callback(connection);
        return true;
    }
public:
    typedef std::function<void(yarp::os::ConnectionReader&)> readerCallback;
    virtual void setCallback(readerCallback callback) {
        this->_callback = callback;
    };
private:
    readerCallback _callback;
};

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

    void plugin_initialize( const variables_map& options ) {
        std::cout << "initialize "<< name()  << " instance " << instance() <<"\n" ;
        yarp = std::shared_ptr<Network>(new Network);
        processor = std::shared_ptr<DataProcessor>(new DataProcessor);
        processor->setCallback([](yarp::os::ConnectionReader& conn){
            yarp::os::Bottle b;
            b.read(conn);
            // process data in b
            std::cout <<std::fixed<<yarp::os::Time::now() << std::endl;
            printf("Got %s\n", b.toString().c_str());
        });
        portIn = std::shared_ptr<Port>(new Port);
        portIn->open("/"+name()+"/"+options.at("port_name").as<string>());
        portIn->setReader(*this->processor);
    }
    void plugin_startup()  {
        std::cout << "starting "<< name() <<"\n";
    }
    void plugin_shutdown() {
        std::cout << "shutdown "<< name() <<"\n";
        portIn.reset(); // Attention do not change reset order !!!
        processor.reset();
        yarp.reset();
    }
private:
    std::shared_ptr<Network> yarp;
    std::shared_ptr<DataProcessor> processor;
    std::shared_ptr<Port> portIn;
};
