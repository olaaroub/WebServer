# 1337 Webserv — HTTP/1.1 Server (C++98)

> A lightweight, event‑driven HTTP/1.1 server built from scratch for the 42 curriculum. Implements non‑blocking I/O, virtual hosts, CGI, uploads, and custom error pages — all in C++98.

<br/>

## ✨ Highlights

* **C++98 only** (no external networking libs)
* **Non‑blocking sockets + epoll** main loop
* **Multiple servers / virtual hosts** (per‑port, per‑server config)
* **HTTP methods**: `GET`, `POST`, `DELETE`
* **Static files** with `index` and **autoindex**
* **Uploads** (configurable `upload_store`)
* **CGI** via `fork/execve` (e.g., Python, PHP)
* **Custom error pages** (`error_page`)
* **Optional sessions** (bonus): simple cookie‑based session store

<br/>

## 🗺️ Architecture at a Glance (Mermaid)

> Kept intentionally **basic** for README — an overview of the request path. (This is a simplified and clearer version of your `2nd.mmd` diagram.)

```mermaid
flowchart LR
  S[Start] --> P[Parse config]
  P --> L[Create listening sockets]
  L --> E[Event loop (epoll)]

  E -->|new connection| A[Accept client]
  A --> R[Parse HTTP/1.1 request]
  R --> D{Route}

  D -->|Static| ST[Serve file or autoindex]
  D -->|Upload (POST)| UP[Save file]
  D -->|CGI| CG[Launch CGI (fork/execve)]
  CG --> RB[Read CGI output]

  ST --> B[Build response]
  UP --> B
  RB --> B

  R -->|invalid| ERR[Generate error response]
  B --> SND[Send + keep-alive/close]
  ERR --> SND
  SND --> E

  %% Optional bonus: cookie/session hop (kept minimal)
  CG -. may set .-> SC[(Session cookie)]
```

<br/>

## ⚙️ Build & Run

```bash
# build
make

# run with your config
./webserv path/to/your.conf

# example
./webserv configs/l3robi.conf
```

* The server spawns one listener **per configured port**.
* Use `Ctrl+C` to stop.

<br/>

## 📑 Minimal Configuration Example

```conf
# l3robi.conf (example)

server {
    listen              8080;
    server_name         localhost;

    root                ./www;
    index               index.html;
    autoindex           on;

    client_max_body_size  10M;   # supports K/M suffix

    error_page 404 /errors/404.html;
    error_page 500 /errors/500.html;

    location /uploads {
        allowed_methods   POST;
        upload_store      ./www/uploads;
    }

    location /cgi-bin {
        allowed_methods   GET POST;
        cgi_pass          .py /usr/bin/python3;
        cgi_pass          .php /usr/bin/php-cgi;
    }

    location /redirect-me {
        return 301 /new-place;   # simple redirect
    }
}
```

**Supported (typical) directives**

* `listen <port>`
* `server_name <name>`
* `root <path>`
* `index <file>`
* `autoindex on|off`
* `allowed_methods GET|POST|DELETE`
* `client_max_body_size <bytes|K|M>`
* `upload_store <path>` (in a `location`)
* `cgi_pass <.ext> <interpreter>` (in a `location`)
* `error_page <code> <uri>`
* `return 301|302 <uri>` (redirect)

> Notes: exact directive names may vary with your parser; adjust to match your project.

<br/>

## 🔁 Request Lifecycle (condensed)

1. **Startup:** parse config → open non‑blocking listeners → register in **epoll**.
2. **Event loop:** `epoll_wait()` → accept new clients → read request bytes.
3. **HTTP parse:** request line, headers, optional body (state machine, chunked/body size checks).
4. **Route:** by method + location block.

   * **Static:** build path from `root`/`index`, apply autoindex.
   * **Upload:** validate size & permissions, save to `upload_store`.
   * **CGI:** prepare envp/argv, `pipe()` + `fork()` + `execve()`, watchdog timeout.
5. **Respond:** status line + headers (+ optional `Set-Cookie`) + body; handle keep‑alive.
6. **Errors:** map to `error_page` when configured.

<br/>

## 🧪 Quick Testing

```bash
# Static
curl -i http://localhost:8080/

# Autoindex (if enabled)
curl -i http://localhost:8080/some/dir/

# Upload (POST)
curl -i -X POST -F file=@hello.txt http://localhost:8080/uploads

# CGI (Python example)
curl -i 'http://localhost:8080/cgi-bin/hello.py?name=webserv'

# Delete
curl -i -X DELETE http://localhost:8080/uploads/hello.txt
```

<br/>

## 📂 Project Layout (example)

```
webserv/
├─ src/
│  ├─ server/              # ServerManager, epoll loop
│  ├─ http/                # Request, Response, headers utils
│  ├─ cgi/                 # CgiExecutor, env builder
│  ├─ config/              # FileReader, ConfigParser
│  └─ utils/
├─ www/                    # Public files (root)
├─ configs/                # Sample .conf files
├─ Makefile
└─ README.md
```

<br/>

## 👥 Authors

* **olaaroub** (Oussama Laaroubi)
* **ohammou**
* **iahamdan**

> 1337 (UM6P) — 42 Network. Project validated ✅

<br/>

## 📜 License

This is an educational 42 project. If you intend to use or redistribute, please add a LICENSE file and update this section accordingly.
