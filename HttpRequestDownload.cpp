#include <iostream>
#include "curl.h"
#include <cstring>

// Struct to store data for progress callback
struct ProgressData {
    double last_display; // Timestamp of last progress display
};

// Callback function to write received data to a file
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// Callback function to display download progress
int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t, curl_off_t) {
    ProgressData *progress_data = (ProgressData *)clientp;
    double now = time(NULL);

    // Display progress every 1 second
    if (now - progress_data->last_display >= 1.0) {
        double percent = (dltotal > 0) ? (dlnow * 100.0 / dltotal) : 0;
        std::cout << "Downloaded " << percent << "% (" << dlnow << " bytes of " << dltotal << " bytes)." << std::endl;
        progress_data->last_display = now;
    }

    return 0;
}

int main() {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Create a curl handle
    CURL *curl = curl_easy_init();
    if (curl) {
        // Set the URL to download
        curl_easy_setopt(curl, CURLOPT_URL, "https://files.testfile.org/ZIPC/250MB-Corrupt-Testfile.Org.zip");

        // Extract filename from URL
        std::string url = "https://files.testfile.org/ZIPC/250MB-Corrupt-Testfile.Org.zip";
        std::string filename = url.substr(url.find_last_of("/") + 1);

        // Set up the file to save the downloaded content
        FILE *fp = fopen(filename.c_str(), "wb");
        if (fp == nullptr) {
            std::cerr << "Failed to open file for writing." << std::endl;
            return 1;
        }

        // Set the callback function to write received data to the file
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        // Set the callback function to display download progress
        ProgressData progress_data;
        progress_data.last_display = 0.0;
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progress_data);

        // Perform the HTTP request
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
