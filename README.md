# My First C2 - Basic File Transfer Using BITS

This repository contains a simple Command and Control (C2) system designed for basic file transfer using the **Background Intelligent Transfer Service (BITS)** on Windows. BITS allows for reliable, asynchronous, and efficient file transfers, which is particularly useful in environments where stable connections are not guaranteed.

---

## Features

- **File Transfer**: Transfer files between the C2 server and the client using BITS.
- **Asynchronous Upload/Download**: BITS allows file transfers to run in the background, enabling efficient file handling even with intermittent connections.
- **Minimal Setup**: Simple C2 setup with only a few components to start transferring files.

---

## Architecture

The C2 system consists of two main parts:

### 1. **C2 Server**
The server side that handles incoming requests and communicates with the clients.

### 2. **C2 Client**
The client side that interacts with the server, receives commands, and transfers files to/from the server using BITS.
