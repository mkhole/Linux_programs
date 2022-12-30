  
# REF

https://www.netfilter.org/projects/libnetfilter_conntrack/index.html

# INTRO

The `libnetfilter_conntrack` is a userspace library providing a programming interface (API) to the in-kernel connection tracking state table.

# Example 1 - Retrieve Mark

The following code uses `libnetfilter_conntrack` to get the mark in `conntrack` information.
https://github.com/mkhole/linux-c-program/blob/master/conntrack/conntrack_mark.c
After compiling with `-lnetfilter_conntrack` LD flag, it needs root privilege to execute.

``` shell
gcc conntrack.c -lnetfilter_conntrack && sudo ./a.out
```

For demonstration, we added an `iptables` rule to set the `conntrack` mark on each DNS request packet.

``` shell
$ sudo iptables -t mangle -A POSTROUTING -p udp --dport 53 -j CONNMARK --set-mark 100
$ sudo iptables -t mangle -nvL
Chain PREROUTING (policy ACCEPT 0 packets, 0 bytes)
 pkts bytes target     prot opt in     out     source               destination

Chain INPUT (policy ACCEPT 0 packets, 0 bytes)
 pkts bytes target     prot opt in     out     source               destination

Chain FORWARD (policy ACCEPT 0 packets, 0 bytes)
 pkts bytes target     prot opt in     out     source               destination

Chain OUTPUT (policy ACCEPT 0 packets, 0 bytes)
 pkts bytes target     prot opt in     out     source               destination

Chain POSTROUTING (policy ACCEPT 0 packets, 0 bytes)
 pkts bytes target     prot opt in     out     source               destination
    0     0 CONNMARK   udp  --  *      *       0.0.0.0/0            0.0.0.0/0            udp dpt:53 CONNMARK set 0x64
```

Then we execute the program to retrieve the mark value in the `conntrack`.

``` shell
$ ping www.google.com -c 1
PING www.google.com (172.217.160.100) 56(84) bytes of data.
64 bytes from tsa03s06-in-f4.1e100.net (172.217.160.100): icmp_seq=1 ttl=128 time=6.35 ms

$ sudo ./build/conntrack_mark
Result:     [NEW] udp      17 30 src=192.168.25.200 dst=1.1.1.1 sport=44385 dport=53 [UNREPLIED] src=1.1.1.1 dst=192.168.25.200 sport=53 dport=44385 mark=100
MSG Type: 1
L3 Proto: IPv4
Mark: 100
Result:  [UPDATE] udp      17 30 src=192.168.25.200 dst=1.1.1.1 sport=51872 dport=53 src=1.1.1.1 dst=192.168.25.200 sport=53 dport=51872 mark=100
MSG Type: 2
L3 Proto: IPv4
Mark: 100
Result:  [UPDATE] udp      17 30 src=192.168.25.200 dst=8.8.8.8 sport=51872 dport=53 src=8.8.8.8 dst=192.168.25.200 sport=53 dport=51872 mark=100
MSG Type: 2
L3 Proto: IPv4
Mark: 100
Result:  [UPDATE] udp      17 30 src=192.168.25.200 dst=192.168.25.2 sport=51872 dport=53 src=192.168.25.2 dst=192.168.25.200 sport=53 dport=51872 mark=100
MSG Type: 2
L3 Proto: IPv4
Mark: 100
Result:  [UPDATE] udp      17 30 src=127.0.0.1 dst=127.0.1.1 sport=32854 dport=53 src=127.0.1.1 dst=127.0.0.1 sport=53 dport=32854 mark=100
MSG Type: 2
L3 Proto: IPv4
Mark: 100
Result:  [UPDATE] udp      17 30 src=192.168.25.200 dst=1.1.1.1 sport=44385 dport=53 src=1.1.1.1 dst=192.168.25.200 sport=53 dport=44385 mark=100
MSG Type: 2
L3 Proto: IPv4
Mark: 100
Result:     [NEW] udp      17 30 src=127.0.0.1 dst=127.0.1.1 sport=49699 dport=53 [UNREPLIED] src=127.0.1.1 dst=127.0.0.1 sport=53 dport=49699 mark=100
MSG Type: 1
L3 Proto: IPv4
Mark: 100
Result:     [NEW] udp      17 30 src=192.168.25.200 dst=1.1.1.1 sport=42009 dport=53 [UNREPLIED] src=1.1.1.1 dst=192.168.25.200 sport=53 dport=42009 mark=100
MSG Type: 1
L3 Proto: IPv4
Mark: 100
Result:  [UPDATE] udp      17 30 src=192.168.25.200 dst=1.1.1.1 sport=42009 dport=53 src=1.1.1.1 dst=192.168.25.200 sport=53 dport=42009 mark=100
MSG Type: 2
L3 Proto: IPv4
Mark: 100
Result:  [UPDATE] udp      17 30 src=127.0.0.1 dst=127.0.1.1 sport=49699 dport=53 src=127.0.1.1 dst=127.0.0.1 sport=53 dport=49699 mark=100
MSG Type: 2
L3 Proto: IPv4
Mark: 100
```

# Example 2 - Retrieve NAT Information

``` shell
$ sudo ./build/conntrack_nat
Result:     [NEW] icmp     1 30 src=192.168.7.34 dst=8.8.8.8 type=8 code=0 id=11610 [UNREPLIED] src=8.8.8.8 dst=192.168.25.200 type=0 code=0 id=11610
MSG Type: 1
Timeout: 30
IPS_SRC_NAT is set
IPS_SRC_NAT_DONE is set
IPS_DST_NAT_DONE is set
Result:  [UPDATE] icmp     1 30 src=192.168.7.34 dst=8.8.8.8 type=8 code=0 id=11610 src=8.8.8.8 dst=192.168.25.200 type=0 code=0 id=11610
MSG Type: 2
Timeout: 30
IPS_SRC_NAT is set
IPS_SRC_NAT_DONE is set
IPS_DST_NAT_DONE is set
Result: [DESTROY] icmp     1 src=192.168.7.34 dst=8.8.8.8 type=8 code=0 id=11610 src=8.8.8.8 dst=192.168.25.200 type=0 code=0 id=11610
MSG Type: 4
Timeout: 0
IPS_SRC_NAT is set
IPS_SRC_NAT_DONE is set
IPS_DST_NAT_DONE is set
```
