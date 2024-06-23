#!/bin/bash

SERVER_URL="http://localhost:8081"

function test_get() {
    echo "Testing GET request for existing file..."
    curl -i $SERVER_URL/index.html
    echo

    echo "Testing GET request for non-existent file..."
    curl -i $SERVER_URL/nonexistent.html
    echo
}

function test_post() {
    echo "Testing file upload via POST..."
    echo "This is a test file" > testfile.txt
    curl -i -X POST -F "file=@testfile.txt" $SERVER_URL/upload
    echo
    rm testfile.txt
}

function test_delete() {
    echo "Testing file delete via DELETE..."
    curl -i -X DELETE $SERVER_URL/upload/testfile.txt
    echo
}

function test_cgi() {
    echo "Testing CGI script execution..."
    curl -i $SERVER_URL/cgi-bin/test.php
    echo
}

function test_responses() {
    echo "Testing various responses..."
    curl -i $SERVER_URL
    echo

    curl -i $SERVER_URL/nonexistentpage
    echo
}

function stress_test() {
    echo "Testing server with high number of requests..."
    ab -n 1000 -c 100 $SERVER_URL/index.html
    echo
}

function run_all_tests() {
    test_get
    test_post
    test_delete
    test_cgi
    test_responses
    stress_test
}

# Executar todos os testes
run_all_tests
