<strong>Native Console Application for Secure Web Project Configuration</strong>

This project provides a native console application designed to securely input your web project's configuration file, including sensitive data such as environment variables, database credentials, and API keys, without leaving traces on the disk. The application ensures that all data is stored only in memory, reducing the risk of sensitive information being exposed during the configuration process.

<strong>How Does It Works?</strong>
    1. Pipe Creation: When the console application is launched, it generates a unique named pipe (e.g., \\.\pipe\SECURITY_GUILHERME_ENV on Windows or /tmp/SECURITY_GUILHERME_ENV on Linux). The name of the pipe is based on a GUID or a predefined format, ensuring that each session has a distinct pipe name.
    2. User Input: The user is prompted to enter a JSON (data) configuration, which typically includes sensitive data such as database passwords, API keys, and other environment variables.
    3. Data Transmission: The entered JSON (data) is securely transmitted through the named pipe to the waiting web application. The console application handles all data securely, ensuring it remains in memory during transmission and is never written to disk.
    4. Reading from the Pipe in your web application: On the web application side, the application is set up to listen to the named pipe during startup. It reads the incoming data from the pipe and parses the JSON, loading the configuration directly into the application’s environment.
    5. Memory Cleanup: After the data is transmitted, the console application securely zeros out the memory used to store the JSON data and closes the pipe, ensuring no residual data is left in memory. The pipe is also deleted, ensuring there are no leftover artifacts that could be exploited.

<strong>Usage</strong>
* Linux Version
Navigate to the Linux branch to find the source code and instructions for compiling and running the application on a Linux-based system.

* Windows Version
Switch to the Windows branch for the version that is fully compatible with Windows environments. Compilation and usage instructions are provided
