#!/bin/bash

CONFIG_DIR="/home/diogo/webserv/config"
WEB_SERVER_BIN="./webserv"
SERVER_PID=""
SERVER_URL="http://localhost:8081"

# Define colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
RESET='\033[0m'

function start_server() {
    local config_file=$1
    $WEB_SERVER_BIN $config_file &
    SERVER_PID=$!
    sleep 2  # Aguarda o servidor iniciar
}

function stop_server() {
    if [ -n "$SERVER_PID" ]; then
        kill $SERVER_PID
        wait $SERVER_PID 2>/dev/null
        SERVER_PID=""
    fi
}

function test_response_status() {
    echo -e "${BLUE}Testing HTTP response status codes...${RESET}"
    curl -i "$SERVER_URL/status_codes"  # Supondo que você tenha uma rota para listar os códigos de status
    echo
}

function test_multiple_ports() {
    echo -e "${BLUE}Testing multiple ports...${RESET}"
    start_server "$CONFIG_DIR/webserv_ports.conf"
    curl -i "http://localhost:8081/index.html"
    curl -i "http://localhost:8082/index.html"
    stop_server
}

function test_multiple_hostnames() {
    echo -e "${BLUE}Testing multiple hostnames...${RESET}"
    start_server "$CONFIG_DIR/webserv_hostnames.conf"
    curl --resolve example.com:8081:127.0.0.1 http://example.com/
    curl --resolve test.com:8081:127.0.0.1 http://test.com/
    stop_server
}

function test_default_error_page() {
    echo -e "${BLUE}Testing default error page (404)...${RESET}"
    start_server "$CONFIG_DIR/webserv_error_page.conf"
    curl -i "$SERVER_URL/nonexistent.html"
    stop_server
}

function test_client_body_limit() {
    echo -e "${BLUE}Testing client body limit...${RESET}"
    start_server "$CONFIG_DIR/webserv_client_body_limit.conf"
    curl -i -X POST -H "Content-Type: plain/text" --data "BODY IS HERE" "$SERVER_URL/upload"
    stop_server
}

function test_routes_to_directories() {
    echo -e "${BLUE}Testing routes to different directories...${RESET}"
    start_server "$CONFIG_DIR/webserv_routes.conf"
    curl -i "$SERVER_URL/dir1/"
    curl -i "$SERVER_URL/dir2/"
    stop_server
}

function test_default_file_for_directory() {
    echo -e "${BLUE}Testing default file for directory...${RESET}"
    start_server "$CONFIG_DIR/webserv_default.conf"
    curl -i "$SERVER_URL/directory/"
    stop_server
}

function test_accepted_methods() {
    echo -e "${BLUE}Testing accepted methods for route...${RESET}"
    start_server "$CONFIG_DIR/webserv_methods.conf"
    curl -i -X DELETE "$SERVER_URL/protected"
    stop_server
}

function show_menu() {
    echo -e "${GREEN}Select a test to run:${RESET}"
    echo "1) Test response status codes"
    echo "2) Test multiple ports"
    echo "3) Test multiple hostnames"
    echo "4) Test default error page"
    echo "5) Test client body limit"
    echo "6) Test routes to directories"
    echo "7) Test default file for directory"
    echo "8) Test accepted methods for route"
    echo "9) Run all tests"
    echo "0) Exit"
    echo
}

while true; do
    show_menu
    read -p "Enter the number of the test to run: " choice
    case $choice in
        1) test_response_status ;;
        2) test_multiple_ports ;;
        3) test_multiple_hostnames ;;
        4) test_default_error_page ;;
        5) test_client_body_limit ;;
        6) test_routes_to_directories ;;
        7) test_default_file_for_directory ;;
        8) test_accepted_methods ;;
        9)
            test_response_status
            test_multiple_ports
            test_multiple_hostnames
            test_default_error_page
            test_client_body_limit
            test_routes_to_directories
            test_default_file_for_directory
            test_accepted_methods
            ;;
        0) exit 0 ;;
        *) echo -e "${RED}Invalid choice. Please enter a number between 0 and 9.${RESET}" ;;
    esac
    echo
done
