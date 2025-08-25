import socket

host = "127.0.0.1"   # السيرفر لي غادي تستقبل عليه
port = 8080          # البورت ديال السيرفر

# نبنيو request POST مع Transfer-Encoding: chunked
request = (
    "POST /uploads HTTP/1.1\r\n"
    f"Host: {host}:{port}\r\n"
    "User-Agent: ChunkTester/1.0\r\n"
    "Transfer-Encoding: chunked\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    # Chunk 1: "Hello"
    "5;lang=en\r\n"
    "Hello\r\n"
    # Chunk 2: " World!"
    "7;desc=\"part2\"\r\n"
    " World!\r\n"
    # نهاية
    "0\r\n"
    "\r\n"
)

# نبعث request
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((host, port))
    s.sendall(request.encode())
    response = s.recv(4096)
    print("=== Server Response ===")
    print(response.decode(errors="ignore"))

