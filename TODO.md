# TODOs:

## Woot 
 - [ ] compiles


### TARGET SPECIFICATION
 - [x] Struct representation
 - [ ] Can pass hostnames, IP addresses, networks, etc.
 - [ ] File input (-iL <filename>)
    - [ ] specify hypen (-) as filename to read from stdin
 - [ ] Exclude hosts (--exclude <host1\[,host2]\[,host3],...>)
 - [ ] Exclude file (--excludefile <exclude_file>)
 - [ ] SUPER BONUS: Choose random targets (-iR <num hosts>)

### PORT SPECIFICATION AND SCAN ORDER
 - [x] Struct representation
 - [ ] Only scan specified ports (-p <port ranges>) (Ex: -p22; -p1-65535; -p U:53,111,137,T:21-25,80,139,8080,S:9)
 - [ ] Exclude the specified ports from scanning (--exclude-ports <port ranges>)
 - [ ] Scan ports consecutively - don't randomize (-r)
 - [ ] BONUS: Scan <number> most common ports (--top-ports <number>)
 - [ ] BONUS: Fast mode - Scan fewer ports than the default scan (-F)

### HOST DISCOVERY
 - [x] Struct representation
 - [ ] Ping scan (-sn)
 - [ ] List scan (-sL)
 - [ ] Skip host discovery (-Pn)
 - [ ] Host discovery methods (With port list)
 	- [ ] TCP SYN host discovery (-PS <portlist>)
 	- [ ] TCP ACK host discovery (-PA <portlist>)
 	- [ ] UDP host discovery (-PU <portlist>)
 	- [ ] SUPER BONUS: SCTP host discovery (-PY)
 - [ ] Host discovery (ICMP without portlists)
 	- [ ] ICMP Echo host discovery (-PE)
 	- [ ] ICMP Timestamp host discovery (-PP)
 	- [ ] ICMP Netmask request host discovery (-PM)
 - [ ] Traceroute (--trace)
 - [ ] BONUS (I think this would require us implementing a getaddrinfo func): Custom DNS Servers (--dns-servers)

### MISC PARSING
 - [x] Struct representation
 - [ ] choose # of threads (--thread <num>)

### WORKER MODE
 - [x] Struct representation (not needed)
 - [ ] Run in "worker mode", and await a job. (--worker <port>)

###  TIMING AND PERFORMANCE
 - [x] Struct representation
 - [ ] Timeing templates (-T0..-T5) (This will be taken care of after we figure out timing)
 - [ ] Specify minimum initial wait time for probe response (--min-rtt-timeout <time>)
 - [ ] Specify maximum initial wait time for probe response (--max-rtt-timeout <time>)
 - [ ] Specify host timeout. Give up on host after this time if scan is not complete. (--host-timeout <time>)
 - [ ] Specify lowest amount of packets sent per second (--min-rate <packet amount>)
 - [ ] Specify highest amount of packets sent per second (--max-rate <packet amount>)
 - [ ] Specify max number of probe retransmissions to a single port (--max-retries <time>)
 - [ ] Specify scan delay inbetween sending any probe from a host. (--scan-delay <time>)
 - [ ] Specify max scan delay. The scan delay can grow as Nmap detects packet loss, so set a max. (--max-scan-delay <time>)

### SCANS
 - [x] Struct representation
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
 - [x] Struct representation
 - [ ] OS detection (-O)

### SERVICE/VERSION DETECTION
 - [x] Struct representation
 - [ ] Probe open ports to determine service/version info (-sV)

### FIREWALL/IDS EVASION AND SPOOFING
 - [x] Struct representation
 - [ ] Fragment packets (optionally w/given MTU) (-f; --mtu <val>:)
 - [ ] Cloak a scan with decoys (-D <decoy1,decoy2\[,ME],...>)
 - [ ] Spoof source address (-S <IP_Address>)
 - [ ] (BONUS hard to work on all hosts) Use specified interface (-e <iface>)
 - [ ] Use given port number (-g/--source-port <portnum>)
 - [ ] Set IP time-to-live field (--ttl <val>)
 - [ ] Append a custom payload to sent packets (--data <hex string>)
 - [ ] Append a custom ASCII string to sent packets (--data-string <string>)
 - [ ] Append random data to sent packets (--data-length <num>)
 - [ ] Send packets with a bogus TCP/UDP/(BONUS: SCTP) checksum (--badsum)

### OUTPUT
 - [x] Struct representation
 - [ ] Verbose output (-v, increase verbosity -vv)
 - [ ] Normal output (-oN) (default)
 - [ ] XML output (-oX)
 - [ ] Only show open (or possibly open) ports (--open)
 - [ ] Append to rather than clobber specified output files (--append-output)
 - [ ] BONUS: Resume an aborted scan (--resume <filename>)
 - [ ] BONUS: Print host interfaces and routes (for debugging) (--iflist)
 - [ ] help screen
	- [ ] time limit
