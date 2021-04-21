function ping_function(){	
	ip netns exec $1 ping -c 3 10.0.10.63
	ip netns exec $1 ping -c 3 10.0.10.64
	ip netns exec $1 ping -c 3 10.0.20.63
	ip netns exec $1 ping -c 3 10.0.20.64
	ip netns exec $1 ping -c 3 10.0.30.63
	ip netns exec $1 ping -c 3 10.0.30.64
	ip netns exec $1 ping -c 3 10.0.40.63
	ip netns exec $1 ping -c 3 10.0.40.64
	ip netns exec $1 ping -c 3 10.0.50.63
	ip netns exec $1 ping -c 3 10.0.50.64
	ip netns exec $1 ping -c 3 10.0.60.63
	ip netns exec $1 ping -c 3 10.0.60.64
}	#Function to ping all the interfaces from the namespace

#Creating the namespaces
ip netns add H1
ip netns add H2
ip netns add H3
ip netns add H4
ip netns add R1
ip netns add R2
ip netns add R3

#Creating the veth interfaces
ip link add v1 type veth peer name v2
ip link add v3 type veth peer name v4
ip link add v5 type veth peer name v6
ip link add v7 type veth peer name v8
ip link add v9 type veth peer name v10
ip link add v11 type veth peer name v12

#Assigning the veth interfaces to corresponding namespaces
ip link set v1 netns H1
ip link set v2 netns R1
ip link set v3 netns H2
ip link set v4 netns R1
ip link set v5 netns R1
ip link set v6 netns R2
ip link set v7 netns R2
ip link set v8 netns R3
ip link set v9 netns R3
ip link set v10 netns H3
ip link set v11 netns R3
ip link set v12 netns H4

#Adding IP Address to all the interfaces
ip netns exec H1 ip addr add 10.0.10.63/24 dev v1
ip netns exec R1 ip addr add 10.0.10.64/24 dev v2
ip netns exec H2 ip addr add 10.0.20.63/24 dev v3
ip netns exec R1 ip addr add 10.0.20.64/24 dev v4
ip netns exec R1 ip addr add 10.0.30.63/24 dev v5
ip netns exec R2 ip addr add 10.0.30.64/24 dev v6
ip netns exec R2 ip addr add 10.0.40.63/24 dev v7
ip netns exec R3 ip addr add 10.0.40.64/24 dev v8
ip netns exec R3 ip addr add 10.0.50.63/24 dev v9
ip netns exec H3 ip addr add 10.0.50.64/24 dev v10
ip netns exec R3 ip addr add 10.0.60.63/24 dev v11
ip netns exec H4 ip addr add 10.0.60.64/24 dev v12

#Making all the interfaces up
ip netns exec H1 ip link set v1 up
ip netns exec R1 ip link set v2 up
ip netns exec H2 ip link set v3 up
ip netns exec R1 ip link set v4 up
ip netns exec R1 ip link set v5 up
ip netns exec R2 ip link set v6 up
ip netns exec R2 ip link set v7 up
ip netns exec R3 ip link set v8 up
ip netns exec R3 ip link set v9 up
ip netns exec H3 ip link set v10 up
ip netns exec R3 ip link set v11 up
ip netns exec H4 ip link set v12 up

#Enabling loopback lo
ip netns exec H1 ip link set lo up
ip netns exec H2 ip link set lo up
ip netns exec H3 ip link set lo up
ip netns exec H4 ip link set lo up
ip netns exec R1 ip link set lo up
ip netns exec R2 ip link set lo up
ip netns exec R3 ip link set lo up

#Enabling IP Forwarding at namespaces R1, R2 and R3
ip netns exec R1 sysctl -w net.ipv4.ip_forward=1
ip netns exec R2 sysctl -w net.ipv4.ip_forward=1
ip netns exec R3 sysctl -w net.ipv4.ip_forward=1

#Adding the required routes to the correspondind route tables of each namespace
ip netns exec R2 ip route add 10.0.50.0/24 via 10.0.40.64 dev v7
ip netns exec R2 ip route add 10.0.60.0/24 via 10.0.40.64 dev v7
ip netns exec R2 ip route add 10.0.20.0/24 via 10.0.30.63 dev v6
ip netns exec R2 ip route add 10.0.10.0/24 via 10.0.30.63 dev v6

ip netns exec R1 ip route add 10.0.40.0/24 via 10.0.30.64 dev v5
ip netns exec R1 ip route add 10.0.50.0/24 via 10.0.30.64 dev v5
ip netns exec R1 ip route add 10.0.60.0/24 via 10.0.30.64 dev v5

ip netns exec R3 ip route add 10.0.10.0/24 via 10.0.40.63 dev v8
ip netns exec R3 ip route add 10.0.20.0/24 via 10.0.40.63 dev v8
ip netns exec R3 ip route add 10.0.30.0/24 via 10.0.40.63 dev v8

ip netns exec H1 ip route add 10.0.20.0/24 via 10.0.10.64 dev v1
ip netns exec H1 ip route add 10.0.30.0/24 via 10.0.10.64 dev v1
ip netns exec H1 ip route add 10.0.40.0/24 via 10.0.10.64 dev v1
ip netns exec H1 ip route add 10.0.50.0/24 via 10.0.10.64 dev v1
ip netns exec H1 ip route add 10.0.60.0/24 via 10.0.10.64 dev v1

ip netns exec H2 ip route add 10.0.10.0/24 via 10.0.20.64 dev v3
ip netns exec H2 ip route add 10.0.30.0/24 via 10.0.20.64 dev v3
ip netns exec H2 ip route add 10.0.40.0/24 via 10.0.20.64 dev v3
ip netns exec H2 ip route add 10.0.50.0/24 via 10.0.20.64 dev v3
ip netns exec H2 ip route add 10.0.60.0/24 via 10.0.20.64 dev v3

ip netns exec H3 ip route add 10.0.10.0/24 via 10.0.50.63 dev v10
ip netns exec H3 ip route add 10.0.20.0/24 via 10.0.50.63 dev v10
ip netns exec H3 ip route add 10.0.30.0/24 via 10.0.50.63 dev v10
ip netns exec H3 ip route add 10.0.40.0/24 via 10.0.50.63 dev v10
ip netns exec H3 ip route add 10.0.60.0/24 via 10.0.50.63 dev v10

ip netns exec H4 ip route add 10.0.10.0/24 via 10.0.60.63 dev v12
ip netns exec H4 ip route add 10.0.20.0/24 via 10.0.60.63 dev v12
ip netns exec H4 ip route add 10.0.30.0/24 via 10.0.60.63 dev v12
ip netns exec H4 ip route add 10.0.40.0/24 via 10.0.60.63 dev v12
ip netns exec H4 ip route add 10.0.50.0/24 via 10.0.60.63 dev v12

#Getting the hops from H1 to H4, H3 to H4 and H4 to H2
echo
echo "Hops from H1 to H4"
echo
ip netns exec H1 traceroute 10.0.60.64
echo
echo "Hops from H3 to H4"
echo
ip netns exec H3 traceroute 10.0.60.64
echo
echo "Hops from H4 to H2"
echo
ip netns exec H4 traceroute 10.0.20.63

#Pinging all the interfaces from each namespace
ns="H1"
ping_function $ns
ns="H2"
ping_function $ns
ns="H3"
ping_function $ns
ns="H4"
ping_function $ns
ns="R1"
ping_function $ns
ns="R2"
ping_function $ns
ns="R3"
ping_function $ns
