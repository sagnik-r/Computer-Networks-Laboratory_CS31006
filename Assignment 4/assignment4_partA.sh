function ping_function(){	
	ip netns exec $1 ping -c 3 10.0.10.63
	ip netns exec $1 ping -c 3 10.0.10.64
	ip netns exec $1 ping -c 3 10.0.20.63
	ip netns exec $1 ping -c 3 10.0.20.64
	ip netns exec $1 ping -c 3 10.0.30.63
	ip netns exec $1 ping -c 3 10.0.30.64
}	#Function to ping all the interfaces from the namespaces

#Creating the namespaces
ip netns add N1
ip netns add N2
ip netns add N3
ip netns add N4

#Creating the veth interfaces
ip link add v1 type veth peer name v2
ip link add v3 type veth peer name v4
ip link add v5 type veth peer name v6

#Linking the interfaces to the namespaces
ip link set v1 netns N1
ip link set v2 netns N2
ip link set v3 netns N2
ip link set v4 netns N3
ip link set v5 netns N3
ip link set v6 netns N4

#Assigning IP addresses to the interfaces
ip netns exec N1 ip addr add 10.0.10.63/24 dev v1
ip netns exec N2 ip addr add 10.0.10.64/24 dev v2
ip netns exec N2 ip addr add 10.0.20.63/24 dev v3
ip netns exec N3 ip addr add 10.0.20.64/24 dev v4
ip netns exec N3 ip addr add 10.0.30.63/24 dev v5
ip netns exec N4 ip addr add 10.0.30.64/24 dev v6

#Making all the interfaces up
ip netns exec N1 ip link set v1 up
ip netns exec N2 ip link set v2 up
ip netns exec N2 ip link set v3 up
ip netns exec N3 ip link set v4 up
ip netns exec N3 ip link set v5 up
ip netns exec N4 ip link set v6 up

#Enabling loopback lo
ip netns exec N1 ip link set lo up
ip netns exec N2 ip link set lo up
ip netns exec N3 ip link set lo up
ip netns exec N4 ip link set lo up

#Enable IP Forwarding at namespace N2 and N3
ip netns exec N2 sysctl -w net.ipv4.ip_forward=1
ip netns exec N3 sysctl -w net.ipv4.ip_forward=1

#Adding all the required routes to the route tables of each namespace
ip netns exec N1 ip route add 10.0.20.0/24 via 10.0.10.64 dev v1
ip netns exec N1 ip route add 10.0.30.0/24 via 10.0.10.64 dev v1

ip netns exec N2 ip route add 10.0.30.0/24 via 10.0.20.64 dev v3

ip netns exec N3 ip route add 10.0.10.0/24 via 10.0.20.63 dev v4

ip netns exec N4 ip route add 10.0.20.0/24 via 10.0.30.63 dev v6
ip netns exec N4 ip route add 10.0.10.0/24 via 10.0.30.63 dev v6

#Pinging all the interface from each namespace
ns="N1"
ping_function $ns
ns="N2"
ping_function $ns
ns="N3"
ping_function $ns
ns="N4"
ping_function $ns
