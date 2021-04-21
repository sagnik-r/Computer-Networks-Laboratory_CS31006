#Add 3 namespaces H1, H2, H3 and R
ip netns add H1
ip netns add H2
ip netns add H3
ip netns add R

#Create the bridge at R
ip netns exec R brctl addbr br

#Bring up the bridge at
ip netns exec R brctl stp br off
ip netns exec R ip link set dev br up

#Create the veth interfaces veth1, veth2, veth3, veth4, veth5 and veth6
ip link add veth1 type veth peer name veth2
ip link add veth5 type veth peer name veth4
ip link add veth6 type veth peer name veth3

#Assign veth1, veth5 and veth6 to H1, H3 and H2 and the remaining to R
ip link set veth1 netns H1
ip link set veth5 netns H3
ip link set veth6 netns H2
ip link set veth2 netns R
ip link set veth3 netns R
ip link set veth4 netns R

#Assign IP Address to veth1, veth2, veth3, veth4, veth5 and veth6
ip netns exec H1 ip addr add 10.0.10.63/24 dev veth1
ip netns exec H2 ip addr add 10.0.20.63/24 dev veth6
ip netns exec H3 ip addr add 10.0.30.63/24 dev veth5
ip netns exec R ip addr add 10.0.10.1/24 dev veth2
ip netns exec R ip addr add 10.0.20.1/24 dev veth3
ip netns exec R ip addr add 10.0.30.1/24 dev veth4

#Assign veth2, veth4 and veth3 to bridge br at R
ip netns exec R brctl addif br veth2
ip netns exec R brctl addif br veth4
ip netns exec R brctl addif br veth3

#Bring up veth1, veth2, veth3, veth4, veth5 and veth6
ip -n H1 link set veth1 up
ip -n H2 link set veth6 up
ip -n H3 link set veth5 up
ip -n R link set veth2 up
ip -n R link set veth3 up
ip -n R link set veth4 up

#Bring up the lo interfaces
ip -n H1 link set lo up
ip -n H2 link set lo up
ip -n H3 link set lo up
ip -n R link set lo up

#Add the veth interfaces to bridge br by setting br as master
ip netns exec R ip link set veth2 master br
ip netns exec R ip link set veth3 master br
ip netns exec R ip link set veth4 master br

#Adding default gateway in all network namespaces
ip netns exec H1 ip route add default via 10.0.10.63
ip netns exec H2 ip route add default via 10.0.20.63
ip netns exec H3 ip route add default via 10.0.30.63

#Enable IP Forwarding with sysctl
sysctl -q net.ipv4.ip_forward=1
