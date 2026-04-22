$ErrorActionPreference = "Stop"

$INCLUDE_DIR = "include"

if (-not (Test-Path -Path $INCLUDE_DIR)) {
    New-Item -ItemType Directory -Force -Path $INCLUDE_DIR | Out-Null
    Write-Host "Created include directory."
}

$HTTPLIB_URL = "https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h"
$HTTPLIB_FILE = "$INCLUDE_DIR\httplib.h"

if (-not (Test-Path -Path $HTTPLIB_FILE)) {
    Write-Host "Downloading httplib.h..."
    Invoke-WebRequest -Uri $HTTPLIB_URL -OutFile $HTTPLIB_FILE
} else {
    Write-Host "httplib.h already exists."
}

$JSON_URL = "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
$JSON_DIR = "$INCLUDE_DIR\nlohmann"
$JSON_FILE = "$JSON_DIR\json.hpp"

if (-not (Test-Path -Path $JSON_DIR)) {
    New-Item -ItemType Directory -Force -Path $JSON_DIR | Out-Null
}

if (-not (Test-Path -Path $JSON_FILE)) {
    Write-Host "Downloading json.hpp..."
    Invoke-WebRequest -Uri $JSON_URL -OutFile $JSON_FILE
} else {
    Write-Host "json.hpp already exists."
}

Write-Host "Compiling main.cpp..."
# Added -D_WIN32_WINNT=0x0A00 to fix cpp-httplib Windows version requirement
g++ main.cpp -o server.exe -I include -lws2_32 -D_WIN32_WINNT=0x0A00 -O3

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation successful. Running server.exe..."
    .\server.exe
} else {
    Write-Host "Compilation failed."
}
