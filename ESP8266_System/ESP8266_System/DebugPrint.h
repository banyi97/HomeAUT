#define DEBUG_ESP_PORT Serial
#ifdef DEBUG_ESP_PORT
#define PRINTF(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#define PRINTLN(...) DEBUG_ESP_PORT.println( __VA_ARGS__ )
#else
#define PRINTF(...)
#define PRINTLN(...)
#endif