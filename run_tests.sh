#!/bin/bash

SERVER_URL="http://localhost:8081"
TEST_DIR="/home/diogo/webserv/www"

# Define colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
RESET='\033[0m'

function test_get_existing_file() {
    echo -e "${BLUE}Testing GET request for existing file...${RESET}"
    curl -i "$SERVER_URL/index.html"
    echo
}

function test_get_nonexistent_file() {
    echo -e "${BLUE}Testing GET request for non-existent file...${RESET}"
    curl -i "$SERVER_URL/nonexistent.html"
    echo
}

function test_post() {
    echo -e "${BLUE}Testing file upload via POST...${RESET}"
    echo "This is a test file" > testfile.txt
    curl -i -X POST -F "file=@testfile.txt" "$SERVER_URL/upload"
    echo
    rm testfile.txt
}

function test_delete() {
    echo -e "${BLUE}Testing file delete via DELETE...${RESET}"
    curl -i -X DELETE "$SERVER_URL/upload/testfile.txt"
    echo
}

function test_cgi() {
    echo -e "${BLUE}Testing CGI script execution...${RESET}"
    curl -i "$SERVER_URL/cgi-bin/test.php"
    echo
}

function test_responses() {
    echo -e "${BLUE}Testing various responses...${RESET}"
    curl -i "$SERVER_URL"
    echo
    curl -i "$SERVER_URL/nonexistentpage"
    echo
}

function test_stress() {
    echo -e "${RED}Testing server with high number of requests...${RESET}"
    echo -e "${YELLOW}Please ensure you have Siege installed${RESET}"
    read -p "Press Enter to continue..."
    siege -b -t30s "$SERVER_URL"
}

function show_menu() {
    echo -e "${GREEN}Select a test to run:${RESET}"
    echo "1) Test GET request for existing file"
    echo "2) Test GET request for non-existent file"
    echo "3) Test file upload via POST"
    echo "4) Test file delete via DELETE"
    echo "5) Test CGI script execution"
    echo "6) Test various responses"
    echo "7) Test server with high number of requests (Stress Test)"
    echo "8) Run all tests"
    echo "0) Exit"
    echo
}

while true; do
    show_menu
    read -p "Enter the number of the test to run: " choice
    case $choice in
        1) test_get_existing_file ;;
        2) test_get_nonexistent_file ;;
        3) test_post ;;
        4) test_delete ;;
        5) test_cgi ;;
        6) test_responses ;;
        7) test_stress ;;
        8) 
            test_get_existing_file
            test_get_nonexistent_file
            test_post
            test_delete
            test_cgi
            test_responses
            test_stress
            ;;
        0) exit 0 ;;
        *) echo -e "${RED}Invalid choice. Please enter a number between 0 and 8.${RESET}" ;;
    esac
    echo
done
