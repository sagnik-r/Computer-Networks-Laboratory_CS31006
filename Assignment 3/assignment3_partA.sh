#Create the veth interfaces veth0 and veth1
ip link add veth0 type veth peer name veth1	

#Create the network namespace sag0 and sag1
ip netns add sag0	
ip netns add sag1	

#Assign veth0 and veth1 to sag0 and sag1 namespace
ip link set veth0 netns sag0	
ip link set veth1 netns sag1

#Assign IP Address to veth0 and veth1
ip netns exec sag0 ip addr add 10.1.1.0/24 dev veth0	
ip netns exec sag1 ip addr add 10.1.2.0/24 dev veth1

#Bring up veth0 and veth1
ip -n sag0 link set veth0 up	
ip -n sag1 link set veth1 up

#Bring up lo interface
ip -n sag0 link set lo up	
ip -n sag1 link set lo up
 
#Update sag0 and sag1 route table by adding a route to 10.1.2.0/24 and 10.1.1.0/24
ip -n sag0 route add 10.1.2.0/24 dev veth0
ip -n sag1 route add 10.1.1.0/24 dev veth1
