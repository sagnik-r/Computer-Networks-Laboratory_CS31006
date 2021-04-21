'''
Name: Sagnik Roy
Roll No: 18CS10063
Networks Lab
Assignment2
'''

import sys
from xml.dom.minidom import parse
import xml.dom.minidom
import pycountry
import pandas as pd
import csv
import geoip2.database

file_name = sys.argv[1]	#input pdml(xml) file

file = xml.dom.minidom.parse(file_name) 
packets = file.getElementsByTagName("packet")

ip_list = []	#list to store all unique ip addresses
for packet in packets:
    protocol = packet.getElementsByTagName("proto")
    for protos in protocol:
        if ( protos.getAttribute("name") == "http" ):	#Only http protocol has the required files
            fields = protos.getElementsByTagName("field")
            ip = ""
            for field in fields:
                flag = 0
                field_name = field.getAttribute("name")
                if( field_name == "http.x_forwarded_for" ):	#Contains the source ip address
                    ip = field.getAttribute("show")		#Get the source ip
                if( field_name == "http.request.line" and field.getAttribute("value") == "5669613a20496e7465726e65742e6f72670d0a" ):	#Check if using internet.org proxy
                    flag = 1
                if( flag == 1 ):	#Using the iorg proxy
                    if( ip not in ip_list ):
                        ip_list.append(ip)	#Add to unique list of ips

print('Total {} users found'.format(len(ip_list)))	#Displaying total unique users
country_dict = {}	#Country: Frequency dictionary


for ip in ip_list:
	'''
	The path to the Geolite2-Country.mmdb is to be replaced in the next line to make it::::
	with geoip2.database.Reader("path to mmdb file") as reader:

	The mmdb file can be downloaded from:
	https://drive.google.com/file/d/1bWkHjjuBKIkqqRosl3bUtxvrVyvRPdOW/view?usp=sharing
	'''
	with geoip2.database.Reader("C:\\Users\\Sagnik Roy\\Desktop\\Assn2_NetworksLab\\GeoLite2-Country.mmdb") as reader:
		get_country = reader.country(ip)
		c_name = get_country.country.name
		# Adding the country to the frequency list
		if c_name in country_dict:
			country_dict[c_name] = country_dict[c_name] + 1
		else:
			country_dict[c_name] = 1

#Creating the data.csv file
out_file = open("data.csv", "w")
writer = csv.writer(out_file)
for key, value in country_dict.items():
    writer.writerow([key, value])
out_file.close()
df = pd.read_csv('data.csv')
df.to_csv('data.csv', index=False)	#Removing blank lines from csv