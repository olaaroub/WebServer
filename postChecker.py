import socket

def send_file_chunked(file_path, host='127.0.0.1', port=8080, chunk_size=8):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))
    print(f"[+] Connected to server {host}:{port}")

    request_line = "POST /upload HTTP/1.1\r\n"
    headers = (
        f"Host: {host}\r\n"
        f"Transfer-Encoding: chunked\r\n"
        f"Content-Type: application/octet-stream\r\n"
        f"Connection: close\r\n"
        "\r\n"
    )
    client_socket.sendall(request_line.encode())
    client_socket.sendall(headers.encode())

    with open(file_path, 'rb') as f:
        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break
            size_line = f"{len(chunk):X}\r\n".encode()
            client_socket.sendall(size_line)
            client_socket.sendall(chunk)
            client_socket.sendall(b"\r\n")
            print(f"Sent chunk of size: {len(chunk)} bytes")

    # send last chunk: size 0
    client_socket.sendall(b"0\r\n\r\n")
    print("here\n")
    # receive response
    response = client_socket.recv(4096)
    print("Server response:")
    print(response.decode(errors='ignore'))

    client_socket.close()

if __name__ == "__main__":
    file_path = "nots.txt"
    send_file_chunked(file_path)
