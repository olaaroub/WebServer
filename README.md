Webserv Architecture
This diagram provides a high-level overview of the server's architecture, from initial configuration to handling a client request.

graph TD
    %% Styling
    classDef setup fill:#0d47a1,stroke:#fff,stroke-width:2px,color:#fff;
    classDef core fill:#1565c0,stroke:#fff,stroke-width:2px,color:#fff;
    classDef request fill:#1e88e5,stroke:#fff,stroke-width:2px,color:#fff;
    classDef action fill:#42a5f5,stroke:#333,stroke-width:2px,color:#333;
    classDef response fill:#90caf9,stroke:#333,stroke-width:2px,color:#333;
    classDef cgi fill:#6a1b9a,stroke:#fff,stroke-width:2px,color:#fff;

    %% Main Flow
    subgraph "1. Server Setup"
        A[Config File (.conf)] --> B(Parse Configuration);
        B --> C(Setup Listening Sockets);
    end

    subgraph "2. Core Event Loop"
        C --> D{epoll: Wait for Events};
        D -- "New Connection" --> E(Accept Client);
        E --> D;
        D -- "Client has Data" --> F(Read Request);
    end

    subgraph "3. Request Processing"
        F --> G(Parse Request);
        G --> H{Route Request};
    end

    subgraph "4. Action Handling"
       H -- "Static File Request" --> I[Serve Static File (GET/DELETE)];
       H -- "CGI Script Request" --> J[Execute CGI Script (GET/POST)];
       H -- "Invalid Request" --> K[Return Error Page (e.g., 404)];
    end

    subgraph "5. Response"
        I --> L(Build HTTP Response);
        J --> L;
        K --> L;
        L --> M(Send Response to Client);
    end

    %% Sub-flow for CGI
    subgraph "CGI Execution Details"
        direction LR
        cgi1["fork() & pipe()"] --> cgi2["Redirect I/O (dup2)"] --> cgi3["execve()"];
    end

    J -.-> cgi1;

    %% Assign Classes
    class A,B,C setup;
    class D,E core;
    class F,G,H request;
    class I,J,K action;
    class L,M response;
    class cgi1,cgi2,cgi3 cgi;




