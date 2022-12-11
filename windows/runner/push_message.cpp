#include <cpr/cpr.h>
#include <cstdio>
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <iostream>
#include "push_message.h"

void pushmessage::initialize()
{
    // Get data from github 
    cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                               cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
                               cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    r.status_code;            // 200
    r.header["content-type"]; // application/json; charset=utf-8
    r.text;
    std::cout << r.text << std::endl;

    // connect to rabbitmq
    amqp_socket_t *socket;
    int status;
    amqp_bytes_t queuename;

    amqp_connection_state_t conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(conn);

    if (!socket)
    {
        std::cout << "creating TCP socket" << std::endl;
    }

    status = amqp_socket_open(socket, "localhost", 5672);
    if (status)
    {
        std::cout << "opening TCP socket" << std::endl;
    }

    amqp_login(conn, 
        "/", 
        0, 
        131072, 
        0, 
        AMQP_SASL_METHOD_PLAIN,
        "guest", 
        "guest",
    );

    amqp_channel_open(conn, 1);
    amqp_get_rpc_reply(conn);

    {

        amqp_queue_declare_ok_t *r = amqp_queue_declare(
            conn, 
            1, 
            amqp_cstring_bytes("ini.channel.saya"), 
            0, 1, 0, 0, 
            amqp_empty_table,
        );

        amqp_get_rpc_reply(conn);

        queuename = amqp_bytes_malloc_dup(r->queue);
        if (queuename.bytes == NULL)
        {
            fprintf(stderr, "Out of memory while copying queue name");
        }
    }

    std::cout << "Declare queue complete" << std::endl;
}