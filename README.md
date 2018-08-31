# ArpProof

linux/C++

Send ARP request

## Build
`Ubuntu 18.04.1` and `gcc version 7.3.0`

```
make
```

## Run
Run as admin, because it use socket.

### param
---
-i [net interface]

Network interface

---

-s [ipv4 address]/[mac address]

ARP sender

---

-t [ipv4 address]/[mac address]

ARP target

---

example

```shell
sudo ./arp_request -i ens33 -s 127.0.1.1/00:0c:29:a6:3a:92 -t 127.0.0.1/ff:ff:ff:ff:ff:ff
```
