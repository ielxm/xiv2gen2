#include <stdio.h>
#include <stdlib.h>
#include <liboath/oath.h>
#include <curl/curl.h> // to make request
#include <time.h>

// 20*8/5+'\0'=33
#define TOTP_SECRET_SIZE 33

// print error msg to stderr and exit with EXIT_FAILURE exit code
static void die(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}
// send totp code to xivlauncher
static void stotp(const char *totp)
{
    CURL *curl = curl_easy_init();
    if (curl == NULL) die("Failed to initialize libcurl\n");

    char url[256];
    snprintf(
        url,
        sizeof(url),
        "http://127.0.0.1:4646/ffxivlauncher/%s",
        totp
    );

    FILE *devnull = fopen("/dev/null", "w");
    if (devnull == NULL) die("Failed to open /dev/null");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, devnull);

    CURLcode rc = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    if (rc != CURLE_OK)
    {
        char msg[256];
        snprintf(
            msg,
            sizeof(msg),
            "Request failed: %s",
            curl_easy_strerror(rc)
        );
        die(msg);
    }
}
int main(int argc, char *argv[])
{
    if (argc != 2) die("Usage: ./xiv2gen2 <stored_secret>");

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) die("Failed to open secret file on provided filepath");
    
    char secret[TOTP_SECRET_SIZE];
    size_t len = fread(secret, 1, sizeof(secret) - 1, f);
    if (ferror(f)) die ("Failed to read secret file");
    
    secret[len] = '\0';
    
    fclose(f);

    char *secret_raw = NULL;
    size_t secret_raw_len = 0;

    int rc = oath_base32_decode(
        secret,
        len,
        &secret_raw,
        &secret_raw_len
    );
    if (rc != OATH_OK) die("Failed to decode provided secret");

    char otp[7];
    rc = oath_totp_generate(
        secret_raw,
        secret_raw_len,
        time(NULL),
        30,
        0,
        6,
        otp
    );
    if (rc != OATH_OK) die("Failed to generate TOTP code");

    printf("%s\n", otp);

    stotp(otp);

    return EXIT_SUCCESS;
}
