import imaplib
import base64
import os
import email
import gmplot
import getpass
import requests
import webbrowser
import binascii
import codecs

def email_get_attachment(email_message):
	for part in email_message.walk():
		if part.get_content_maintype() == 'multipart':
			continue
		if part.get('Content-Disposition') is None:
			continue
		fileName = part.get_filename()
		
		if bool(fileName):
			dirName = os.path.dirname(__file__)
			filePath = os.path.join(dirName, 'rb_newest')
			if os.path.isfile(filePath):
				os.remove(filePath)
				
			fp = open(filePath, 'wb')
			fp.write(part.get_payload(decode=True))
			fp.close()
				
def get_newest_email(mail):
	mail.select('Inbox')
	result, data = mail.uid('search', None, "ALL") # search and return uids instead
	latest_email_uid = data[0].split()[-1]
	result, data = mail.uid('fetch', latest_email_uid, '(RFC822)')

	raw_email = data[0][1]
	raw_email_string = raw_email.decode('utf-8')
	email_message = email.message_from_string(raw_email_string)
	#print(email_message)
	return email_message

# gets data from file
# creates google maps plot and prints other data
def process_rb_message(filePath):
	with open(filePath, 'r') as f:
		rb_data = f.readline().strip().split(',')
		pressure = rb_data[1]
		humidity = rb_data[2]
		hTemp = rb_data[3]
		pTemp = rb_data[4]
		altitude = rb_data[5]
		time = rb_data[6]
		latitude = rb_data[7]
		longitude = rb_data[9]
		
		if latitude != '?' and longitude != '?':
			print('GPS Data:')
			print('GPS Timestamp: ' + time)
			print('GPS Altitude: ' + altitude)
			latitude_deg = int(latitude) / 3600000
			longitude_deg = -1*int(longitude) / 3600000 
			gmap3 = gmplot.GoogleMapPlotter(latitude_deg, longitude_deg, 15, 'INSERT_API_KEY_HERE') # make sure to fill in your API key
			gmap3.scatter( [latitude_deg], [longitude_deg], '#FF0000', 
						  size = 100, marker = False )
			filePath = os.path.join(dirName, 'rb_map.html')
			gmap3.draw(filePath)
			print('GPS Latitude: ' + str(latitude_deg))
			print('GPS Longitude: ' + str(longitude_deg))
			print('Created a Google Maps plot called rb_map.html')
			webbrowser.get('firefox').open_new_tab(filePath) # on windows, firefox must be in your enviornmental variables.
		else:
			print('GPS Data: Unknown location, no fix')
		
		print('\nPressure Sensor Data: ')
		if pressure == '?':
			print('Unknown, data not acquired properly')
		else:
			print('Pressure: ' + pressure)
			print('Temperature: ' + pTemp)
			
		print('\nHumidity Sensor Data: ')
		if humidity == '?':
			print('Unknown, data not acquired properly')
		else:
			print('Humidity: ' + humidity)
			print('Temperature: ' + hTemp)
			
def rb_send_message(message):
	url = "https://rockblock.rock7.com/rockblock/MT"
	querystring = {"imei":"INSERT_ROCKBLOCK_IMEI_HERE","username":"INSERT_ROCKBLOCK_USERNAME_HERE","password":"INSERT_ROCKBLOCK_PASSWORD_HERE","data":message} # Fill in your RockBLOCK login info and imei
	response = requests.request("POST", url, params=querystring)
	print(response.text)
			
			
# start of program

os.system('cls' if os.name == 'nt' else 'clear')
email_user = input('Email: ') # email login info
email_pass = getpass.getpass()

mail = imaplib.IMAP4_SSL("imap.gmail.com",993)
mail.login(email_user, email_pass)
os.system('cls' if os.name == 'nt' else 'clear')
print('Successfully logged in!')

while True:

	print('g: get and process most recent data')
	print('s: send a message to the RockBLOCK')
	print('q: quit the program')
	command = input('What would you like to do: ')
	if command == 'g':
		os.system('cls' if os.name == 'nt' else 'clear')
		print('Acquiring Email:')
		email_message = get_newest_email(mail)
		email_get_attachment(email_message)

		dirName = os.path.dirname(__file__)
		filePath = os.path.join(dirName, 'rb_newest')
		process_rb_message(filePath)
		
	elif command == 's':
		print('f: cut ftu')
		print('p: talk to other payload')
		command = input('Which message would you like to send: ')
		os.system('cls' if os.name == 'nt' else 'clear')
		if(command == 'f'):
			print('Cutting FTU')
			rb_send_message(binascii.hexlify(b'FTUPLZ'))
		if(command == 'p'):
			print('Format your message: ')
			id = input('Payload ID: ');
			message = input('Input the 6 characters that comprise your message: ')
			total = id + message + '\n'
			rb_send_message(binascii.hexlify(total.encode()))
			
	elif command == 'q':
		break
	else:
		print('Unknown command: ' + command)
		continue
	
