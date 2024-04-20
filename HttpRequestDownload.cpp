#include <iostream>
#include <string>
#include "curl.h"

std::string downloadURL = "https://testfileorg.netwet.net/500MB-CZIPtestfile.org.zip"; // Some random download (500MB)

// Callback function to write received data to a file
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int main() {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Create a curl (cURL) handle
    CURL *curl = curl_easy_init();
    if (curl) {
        // Set the URL to download
        curl_easy_setopt(curl, CURLOPT_URL, downloadURL);

        // Set up the file to save the downloaded content
        FILE *fp = fopen("download_file.txt", "wb");
        if (fp == nullptr){
            std::cerr << "Failed to open file for writing. (Locating Error)" << std::endl;
            return 1;
        }

        // Set the callback function to write received data to the file
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        // Perfom the HTTP request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "Failed to download file: " << curl_easy_strerror(res) << std::endl;
            fclose(fp);
            return 1;
        }

        // Cleanup
        fclose(fp);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize curl." << std::endl;
        return 1;
    }

    // Cleanup global state
    curl_global_cleanup();

    std::cout << "File downloaded successfully." << std::endl;
    return 0;
}