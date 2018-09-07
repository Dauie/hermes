# TODOs:

## Woot 
 - [ ] compiles


### TARGET SPECIFICATION
 - [ ] Can pass hostnames, IP addresses, networks, etc.
 - [ ] File input (-iL <filename>)
 - [ ] Exclude hosts (--exclude <host1\[,host2]\[,host3],...>)
 - [ ] Exclude file (--excludefile <exclude_file>)
 - [ ] SUPER BONUS: Choose random targets (-iR <num hosts>)

### PORT SPECIFICATION AND SCAN ORDER
 - [ ] Only scan specified ports (-p <port ranges>) (Ex: -p22; -p1-65535; -p U:53,111,137,T:21-25,80,139,8080,S:9)
 - [ ] Exclude the specified ports from scanning (--exclude-ports <port ranges>)
 - [ ] Scan ports consecutively - don't randomize (-r)
 - [ ] BONUS: Scan <number> most common ports (--top-ports <number>)
 - [ ] BONUS: Fast mode - Scan fewer ports than the default scan (-F)

### HOST DISCOVERY
 - [ ] Ping scan (-sn)
 - [ ] List scan (-sL)
 - [ ] Skip host discovery (-Pn)
 - [ ] Host discovery methods (With port list)
 	- [ ] TCP SYN host discovery (-PS <portlist>)
 	- [ ] TCP ACK host discovery (-PA <portlist>)
 	- [ ] UDP host discovery (-PU <portlist>)
 	- [ ] SUPER BONUS: SCTP host discovery (-PY)
 - [ ] Host discovery (ICMP)
 	- [ ] ICMP Echo host discovery (-PE)
 	- [ ] ICMP Timestamp host discovery (-PP)
 	- [ ] ICMP Netmask request host discovery (-PM)
 - [ ] Traceroute (--trace)
 - [ ] BONUS: Custom DNS Servers (--dns-servers)

### MISC PARSING
 - [ ] choose # of threads (--thread <num>)

### WORKER MODE
 - [ ] Run in "worker mode", and await a job. (--worker <port>)

###  TIMING AND PERFORMANCE
 - [ ] Timeing templates (-T0..-T5)
 - [ ] Specify minimum initial wait time for probe response (--min-rtt-timeout <time>)
 - [ ] Specify maximum initial wait time for probe response (--max-rtt-timeout <time>)
 - [ ] Specify host timeout. Give up on host after this time if scan is not complete. (--host-timeout <time>)
 - [ ] Specify lowest amount of packets sent per second (--min-rate <packet amount>)
 - [ ] Specify highest amount of packets sent per second (--max-rate <packet amount>)
 - [ ] Specify max number of probe retransmissions to a single port (--max-retries)
 - [ ] Specify scan delay inbetween sending any probe from a host. (--scan-delay)
 - [ ] Specify max scan delay. The scan delay can grow as Nmap detects packet loss, so set a max. (--max-scan-delay)

### SCANS
 - [ ] scans
	- [ ] SYN (-sS)
	- [ ] NULL (-sT)
	- [ ] ACK (-sA)
	- [ ] FIN (-sF)
	- [ ] XMAS (-sX)
	- [ ] UDP (-sU)
	- [ ] if not specified use all six
	- [ ] can run any combination of scans

### OS DETECTION
 - [ ] OS detection (-O)

### SERVICE/VERSION DETECTION
 - [ ] Probe open ports to determine service/version info (-sV)
 - [ ] Intensity (--version-intensity <level{0-9}>) (need default)

### FIREWALL/IDS EVASION AND SPOOFING
 - [ ] Fragment packets (optionally w/given MTU) (-f; --mtu <val>:)
 - [ ] Cloak a scan with decoys (-D <decoy1,decoy2\[,ME],...>)
 - [ ] Spoof source address (-S <IP_Address>)
 - [ ] Use specified interface (-e <iface>)
 - [ ] Use given port number (-g/--source-port <portnum>)
 - [ ] Set IP time-to-live field (--ttl <val>)
 - [ ] Append a custom payload to sent packets (--data <hex string>)
 - [ ] Append a custom ASCII string to sent packets (--data-string <string>)
 - [ ] Append random data to sent packets (--data-length <num>)
 - [ ] Send packets with a bogus TCP/UDP/(BONUS: SCTP) checksum (--badsum)

### OUTPUT
 - [ ] Verbose output (-v, increase verbosity -vv)
 - [ ] Normal output (-oN) (default)
 - [ ] XML output (-oX)
 - [ ] Only show open (or possibly open) ports (--open)
 - [ ] Append to rather than clobber specified output files (--append-output)
 - [ ] BONUS: Resume an aborted scan (--resume <filename>)
 - [ ] BONUS: Print host interfaces and routes (for debugging) (--iflist)
 - [ ] help screen
	- [ ] time limit
