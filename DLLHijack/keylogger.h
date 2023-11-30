#pragma once
#include <windows.h>
#include <stdio.h>
#include <winhttp.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <time.h>

//Global vars
HANDLE ghMutex; // release mutex after ever return