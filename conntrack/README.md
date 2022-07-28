# libnetfilter_conntrack

The `libnetfilter_conntrack` is a userspace library providing a programming interface (API) to the in-kernel connection tracking state table.

## Intro

This program uses `libnetfilter_conntrack` to get the mark in `conntrack` information.  
For experimentation we added a `iptables` rule to set the `conntrack` mark on DNS request packets.

```shell
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

## Result

And then we execute the program to retrieve the mark in the `conntrack`.

```shell
$ ping www.google.com -c 1
PING www.google.com (172.217.160.100) 56(84) bytes of data.
64 bytes from tsa03s06-in-f4.1e100.net (172.217.160.100): icmp_seq=1 ttl=128 time=6.35 ms

--- www.google.com ping statistics ---
1 packets transmitted, 1 received, 0% packet loss, time 0ms
rtt min/avg/max/mdev = 6.347/6.347/6.347/0.000 ms
$ sudo ./build/conntrack 
Result:  [UPDATE] udp      17 27 src=192.168.25.130 dst=1.1.1.1 sport=34883 dport=53 src=1.1.1.1 dst=192.168.25.130 sport=53 dport=34883 mark=100 use=1
Mark: 100
Result:  [UPDATE] udp      17 27 src=192.168.25.130 dst=1.1.1.1 sport=48764 dport=53 src=1.1.1.1 dst=192.168.25.130 sport=53 dport=48764 mark=100 use=1
Mark: 100
Result:  [UPDATE] udp      17 27 src=127.0.0.1 dst=127.0.1.1 sport=55853 dport=53 src=127.0.1.1 dst=127.0.0.1 sport=53 dport=55853 mark=100 use=1
Mark: 100
Result:  [UPDATE] udp      17 27 src=192.168.25.130 dst=1.1.1.1 sport=36450 dport=53 src=1.1.1.1 dst=192.168.25.130 sport=53 dport=36450 mark=100 use=1
Mark: 100
Result:  [UPDATE] udp      17 27 src=192.168.25.130 dst=8.8.8.8 sport=36450 dport=53 src=8.8.8.8 dst=192.168.25.130 sport=53 dport=36450 mark=100 use=1
Mark: 100
Result:  [UPDATE] udp      17 27 src=127.0.0.1 dst=127.0.1.1 sport=60674 dport=53 src=127.0.1.1 dst=127.0.0.1 sport=53 dport=60674 mark=100 use=1
Mark: 100
Result:  [UPDATE] udp      17 27 src=127.0.0.1 dst=127.0.0.1 sport=54313 dport=53 src=127.0.0.1 dst=127.0.0.1 sport=53 dport=54313 mark=100 use=1
Mark: 100
Result:  [UPDATE] udp      17 27 src=127.0.0.1 dst=127.0.0.1 sport=43177 dport=53 src=127.0.0.1 dst=127.0.0.1 sport=53 dport=43177 mark=100 use=1
Mark: 100
Result:  [UPDATE] udp      17 27 src=127.0.0.1 dst=127.0.1.1 sport=41079 dport=53 src=127.0.1.1 dst=127.0.0.1 sport=53 dport=41079 mark=100 use=1
Mark: 100
```
