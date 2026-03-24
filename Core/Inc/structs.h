#ifndef INC_STRUCTS_H_
#define INC_STRUCTS_H_

#include <stdint.h>

/* ============================
 * Telemetry packet definition
 * ============================ */

#define TELEMETRY_MAGIC  0xBEEF

typedef struct
{
    uint16_t magic;            // 2 bytes
    uint32_t time_us;          // 4 bytes

    uint16_t pt_raw[6];        // 6 * 2 = 12 bytes (raw ADC counts)
    float    pt_psi[6];        // 6 * 4 = 24 bytes (converted pressure)

    float loadcell_weight;     // 4 bytes

    uint32_t checksum;         // 4 bytes
} __attribute__((packed)) telemetry_packet_t;

#endif /* INC_STRUCTS_H_ */
