all :
	g++ -o arp_request main.cpp arp.cpp physical_address.cpp

clean :
	rm arp_request