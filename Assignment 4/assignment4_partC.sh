#Creating all the namespaces
ip netns add N1
ip netns add N2
ip netns add N3
ip netns add N4
ip netns add N5
ip netns add N6

#Creating the veth interfaces
ip link add v1 type veth peer name v2
ip link add v3 type veth peer name v4
ip link add v5 type veth peer name v6
ip link add v7 type veth peer name v8
ip link add v9 type veth peer name v10
ip link add v11 type veth peer name v12

#Assigning the veth interfaces to the corresponding namespaces
ip link set v1 netns N1
ip link set v2 netns N2
ip link set v3 netns N2
ip link set v4 netns N3
ip link set v5 netns N3
ip link set v6 netns N4
ip link set v7 netns N4
ip link set v8 netns N5
ip link set v9 netns N5
ip link set v10 netns N6
ip link set v11 netns N6
ip link set v12 netns N1

#Assigning IP Addresses to each veth interface
ip netns exec N1 ip addr add 10.0.10.63/24 dev v1
ip netns exec N2 ip addr add 10.0.10.64/24 dev v2
ip netns exec N2 ip addr add 10.0.20.63/24 dev v3
ip netns exec N3 ip addr add 10.0.20.64/24 dev v4
ip netns exec N3 ip addr add 10.0.30.63/24 dev v5
ip netns exec N4 ip addr add 10.0.30.64/24 dev v6
ip netns exec N4 ip addr add 10.0.40.63/24 dev v7
ip netns exec N5 ip addr add 10.0.40.64/24 dev v8
ip netns exec N5 ip addr add 10.0.50.63/24 dev v9
ip netns exec N6 ip addr add 10.0.50.64/24 dev v10
ip netns exec N6 ip addr add 10.0.60.63/24 dev v11
ip netns exec N1 ip addr add 10.0.60.64/24 dev v12

#Enabling the veth interfaces
ip netns exec N1 ip link set v1 up
ip netns exec N2 ip link set v2 up
ip netns exec N2 ip link set v3 up
ip netns exec N3 ip link set v4 up
ip netns exec N3 ip link set v5 up
ip netns exec N4 ip link set v6 up
ip netns exec N4 ip link set v7 up
ip netns exec N5 ip link set v8 up
ip netns exec N5 ip link set v9 up
ip netns exec N6 ip link set v10 up
ip netns exec N6 ip link set v11 up
ip netns exec N1 ip link set v12 up

#Enabling loopback lo
ip netns exec N1 ip link set lo up
ip netns exec N2 ip link set lo up
ip netns exec N3 ip link set lo up
ip netns exec N4 ip link set lo up
ip netns exec N5 ip link set lo up
ip netns exec N6 ip link set lo up

#Enabling IP Forwarding in all the namespaces
ip netns exec N1 sysctl -w net.ipv4.ip_forward=1
ip netns exec N2 sysctl -w net.ipv4.ip_forward=1
ip netns exec N3 sysctl -w net.ipv4.ip_forward=1
ip netns exec N4 sysctl -w net.ipv4.ip_forward=1
ip netns exec N5 sysctl -w net.ipv4.ip_forward=1
ip netns exec N6 sysctl -w net.ipv4.ip_forward=1

#Adding the required routes to the route tables of the namespacs
ip netns exec N1 ip route add 10.0.20.0/24 via 10.0.10.64 dev v1
ip netns exec N1 ip route add 10.0.30.0/24 via 10.0.10.64 dev v1
ip netns exec N1 ip route add 10.0.40.0/24 via 10.0.10.64 dev v1
ip netns exec N1 ip route add 10.0.50.0/24 via 10.0.10.64 dev v1

ip netns exec N2 ip route add 10.0.30.0/24 via 10.0.20.64 dev v3
ip netns exec N2 ip route add 10.0.40.0/24 via 10.0.20.64 dev v3
ip netns exec N2 ip route add 10.0.50.0/24 via 10.0.20.64 dev v3
ip netns exec N2 ip route add 10.0.60.0/24 via 10.0.20.64 dev v3

ip netns exec N3 ip route add 10.0.10.0/24 via 10.0.30.64 dev v5
ip netns exec N3 ip route add 10.0.40.0/24 via 10.0.30.64 dev v5
ip netns exec N3 ip route add 10.0.50.0/24 via 10.0.30.64 dev v5
ip netns exec N3 ip route add 10.0.60.0/24 via 10.0.30.64 dev v5

ip netns exec N4 ip route add 10.0.10.0/24 via 10.0.40.64 dev v7
ip netns exec N4 ip route add 10.0.20.0/24 via 10.0.40.64 dev v7
ip netns exec N4 ip route add 10.0.50.0/24 via 10.0.40.64 dev v7
ip netns exec N4 ip route add 10.0.60.0/24 via 10.0.40.64 dev v7

ip netns exec N5 ip route add 10.0.10.0/24 via 10.0.50.64 dev v9
ip netns exec N5 ip route add 10.0.20.0/24 via 10.0.50.64 dev v9
ip netns exec N5 ip route add 10.0.30.0/24 via 10.0.50.64 dev v9
ip netns exec N5 ip route add 10.0.60.0/24 via 10.0.50.64 dev v9

ip netns exec N6 ip route add 10.0.10.0/24 via 10.0.60.64 dev v11
ip netns exec N6 ip route add 10.0.20.0/24 via 10.0.60.64 dev v11
ip netns exec N6 ip route add 10.0.30.0/24 via 10.0.60.64 dev v11
ip netns exec N6 ip route add 10.0.40.0/24 via 10.0.60.64 dev v11

#Displaying the hops from N1 to N5, N3 to N5 and N3 to N1
echo
echo "Hops from N1 to N5 (v8 - 10.0.40.64)"
echo
sleep 2
ip netns exec N1 traceroute -T 10.0.40.64
echo
echo "Hops from N1 to N5 (v9 - 10.0.50.63)"
echo
sleep 2
ip netns exec N1 traceroute -T 10.0.50.63
echo
echo "Hops from N3 to N5 (v8 - 10.0.40.64)"
echo
sleep 2
ip netns exec N3 traceroute -T 10.0.40.64
echo
echo "Hops from N3 to N5 (v9 - 10.0.50.63)"
echo
sleep 2
ip netns exec N3 traceroute -T 10.0.50.63
echo
echo "Hops from N3 to N1 (v12 - 10.0.60.64)"
echo
sleep 2
ip netns exec N3 traceroute -T 10.0.60.64
echo
echo "Hops from N3 to N1 (v1 - 10.0.10.63)"
echo
sleep 2
ip netns exec N3 traceroute -T 10.0.10.63
