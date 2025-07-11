import socket
import time

# This script connects to a server, sends a simple GET request, and then
# shuts down the writing half of the socket to trigger EPOLLRDHUP on the server.

# --- Configure this ---
SERVER_HOST = '127.0.0.1'
SERVER_PORT = 8080 # Change to your server's port
# ----------------------

print(f"Connecting to {SERVER_HOST}:{SERVER_PORT}...")
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    client_socket.connect((SERVER_HOST, SERVER_PORT))
    print("✅ Connected.")

    request = b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    client_socket.sendall(request)
    print("✅ Sent a simple GET request.")

    # THIS IS THE KEY PART:
    # We tell the OS we are done writing, but we can still read.
    # This sends the signal that triggers EPOLLRDHUP on the server.
    print("⏳ Shutting down the writing half of the socket (SHUT_WR)...")
    client_socket.shutdown(socket.SHUT_WR)
    print("✅ Write stream closed.")

    # Your server should now detect the EPOLLRDHUP and might close the connection.
    # We can try to read the response it sends before disconnecting.
    print("⏳ Waiting for server's response...")
    response = client_socket.recv(4096)

    if response:
        print("\n--- Server Response ---")
        print(response.decode('utf-8', errors='ignore'))
        print("-----------------------")
    else:
        print("✅ Server closed the connection as expected after RDHUP.")

except Exception as e:
    print(f"❌ An error occurred: {e}")
finally:
    print("Closing socket.")
    client_socket.close()
