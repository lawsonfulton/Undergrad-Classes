import base64
import urllib2 as url
import random

webapp_url = "http://localhost:4555"

def decode(string):
	return bytearray(base64.b64decode(string))

def encode(bytes):
	return base64.b64encode(str(bytes))

#xor two byte arrays
def xor(a, b):
	result = bytearray(a)
	for i in xrange(len(a)):
		result[i] = a[i] ^ b[i]

	return result

def concat_bytes(a, b):
	#check_vals(a, "a")
	#check_vals(b, "b")
	return a + b

def check_vals(x, name):
	for val in x:
		if val < 0 or val > 255:
			print "%s: %d" % (name, val)

def get_cookie():
	response = url.urlopen(webapp_url)
	cookie = response.info()["Set-Cookie"]
	return cookie.split('"')[1]

def blocks(msg_bytes):
	i = 0
	while i < len(msg_bytes):
		yield msg_bytes[i:i+16]
		i += 16

def random_word():
	return random.randint(0,255)



















