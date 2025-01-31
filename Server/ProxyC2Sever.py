import socket
import subprocess

class ProxyC2Server:
    def __init__(self, server_ip, server_port):
        self.server_ip = server_ip
        self.server_port = server_port
        self.server_socket = None

    def connect_to_c2_server(self):
        """Establish connection with the C2 server."""
        try:
            # Create socket object
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print(f"Connecting to {self.server_ip}:{self.server_port}...")
            # Connect to the server
            self.server_socket.connect((self.server_ip, self.server_port))
            print("Connected to the C2 server.")
        except socket.error as err:
            print(f"Error connecting to the server: {err}")

    def close_connection(self):
        if self.server_socket:
            self.server_socket.close()
            print("Connection closed.")

    def listen_for_commands(self):
        try:
            while True:
                data = self.server_socket.recv(1024)

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
            self.server_socket.close()  # Ensure the socket is closed when done
            print("Connection closed.")

    def process_command(self, command):
        """Process the received command."""
        # This is where you handle the command, e.g., run local actions or send a response.
        # For now, let's just print it and return a simple response.
        print(f"Processing command: {command}")
        
        # Example: If the command is "exit", return a response
        if command == "exit":
            return "Proxy server shutting down."
        
        # You can add more logic here to handle different types of commands
        return f"Command '{command}' executed successfully."
    
    def send_response(self, response):
        """Send the response from the executed command back to the C2 server."""
        try:
            # Encode the response into bytes
            encoded_response = response.encode('utf-8')
            # Send the response to the C2 server
            self.server_socket.send(encoded_response)
            print(f"Sent response: {response}")
        except socket.error as err:
            print(f"Error sending response: {err}")


# Example usage
proxy = ProxyC2Server("127.0.0.1", 9999)  # Replace with actual server IP and port
proxy.connect_to_c2_server()
proxy.listen_for_commands()