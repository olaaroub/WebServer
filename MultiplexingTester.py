import asyncio

HOST = '127.0.0.1'
PORT = 8080

async def slow_client(name, delay, body):
    reader, writer = await asyncio.open_connection(HOST, PORT)
    print(f"[{name}] Connected")

    # send request headers slowly
    request = (
        "POST / HTTP/1.1\r\n"
        f"Host: localhost\r\n"
        f"Content-Length: {len(body)}\r\n"
        "\r\n"
    )

    # simulate sending header slowly
    for part in request:
        writer.write(part.encode())
        await writer.drain()
        await asyncio.sleep(delay)

    # send body slowly
    for b in body:
        writer.write(b.encode())
        await writer.drain()
        await asyncio.sleep(delay)

    print(f"[{name}] Waiting for response...")
    response = await reader.read(4096)
    print("Got response ")
    # print(f"[{name}] Got response:\n{response.decode(errors='ignore')}")
    print(f"[{name}] Closing socket...")
    writer.close()
    await writer.wait_closed()


async def fast_client(name, body):
    reader, writer = await asyncio.open_connection(HOST, PORT)
    print(f"[{name}] Connected")

    request = (
        "POST / HTTP/1.1\r\n"
        f"Host: localhost\r\n"
        f"Content-Length: {len(body)}\r\n"
        "\r\n"
        f"{body}"
    )
    writer.write(request.encode())
    await writer.drain()

    print(f"[{name}] Waiting for response...")
    response = await reader.read(4096)
    # print(f"[{name}] Got response:\n{response.decode(errors='ignore')}")
    print(f"[{name}] Closing socket...")
    writer.close()
    await writer.wait_closed()


async def main():
    tasks = [
        slow_client("slow2", 0.2, "hello slow2")
    ]
    await asyncio.gather(*tasks)

if __name__ == "__main__":
    asyncio.run(main())

# slow_client("slow2", 0.2, "hello slow2"),
        # fast_client("fast1", "hello fast1"),
        # fast_client("fast2", "hello fast2"),