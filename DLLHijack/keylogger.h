#pragma once
#include <windows.h>
#include <stdio.h>
#include <winhttp.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <time.h>
#pragma comment(lib, "winhttp.lib")
#define SEND_FREQUENCY 5000 // send a log file every 5 seconds (this is too fast)


/*
* sendHTTPRequest() will send the log file as an http request 
*/

//Global vars
HANDLE ghMutex; // release mutex after ever return