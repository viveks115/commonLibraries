#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

// === Required I2C functions (implement based on your microcontroller) ===

void i2c_init(void) {
}

void i2c_wait(void) {
    while ((SSPCON2 & 0x1F) || (SSPSTAT & 0x04)); // Wait for idle
}

void i2c_start(void) {
    i2c_wait();
    SEN = 1;
}

void i2c_stop(void) {
    i2c_wait();
    PEN = 1;
}

void i2c_restart(void) {
    i2c_wait();
    RSEN = 1;
}

bool i2c_write_byte(uint8_t data) {
    i2c_wait();
    SSPBUF = data;
    i2c_wait();
    return !ACKSTAT;  // 0 = ACK received
}

uint8_t i2c_read_byte(bool ack) {
    uint8_t received;
    i2c_wait();
    RCEN = 1;             // Enable receive
    i2c_wait();
    received = SSPBUF;
    i2c_wait();
    ACKDT = ack ? 0 : 1;  // 0 = ACK, 1 = NACK
    ACKEN = 1;            // Send acknowledge
    return received;
}
///////////////////////////////////////////////////CONTROLLER SPECIFIC CODE ENDS/////////////////////////////////////////////////////////////

// === EEPROM Configuration ===
#define EEPROM_I2C_ADDRESS  0x50       // 24Cxx base address
#define EEPROM_SIZE_KBIT    128        // Change this based on your EEPROM (e.g., 2, 4, 8, 16, 128)
#define EEPROM_PAGE_SIZE    64         // Adjust according to EEPROM datasheet
#define EEPROM_TOTAL_BYTES  (EEPROM_SIZE_KBIT * 1024 / 8)
#define USE_16BIT_ADDRESS   (EEPROM_SIZE_KBIT > 16)

// === Delay after write ===
void eeprom_write_delay() {
    for (volatile int i = 0; i < 50000; i++);  // Adjust if needed
}

// === Low-level Byte Write ===
bool eeprom_write_bytes(uint16_t mem_address, uint8_t *data, uint16_t len) {
    if (len > EEPROM_PAGE_SIZE) return false;

    i2c_start();

    if (USE_16BIT_ADDRESS) {
        if (!i2c_write_byte((EEPROM_I2C_ADDRESS << 1) | 0)) return false;
        if (!i2c_write_byte(mem_address >> 8)) return false;
        if (!i2c_write_byte(mem_address & 0xFF)) return false;
    } else {
        uint8_t addr = EEPROM_I2C_ADDRESS | ((mem_address >> 8) & 0x07);
        if (!i2c_write_byte((addr << 1) | 0)) return false;
        if (!i2c_write_byte(mem_address & 0xFF)) return false;
    }

    for (uint16_t i = 0; i < len; i++) {
        if (!i2c_write_byte(data[i])) return false;
    }

    i2c_stop();
    eeprom_write_delay();
    return true;
}

// === Low-level Byte Read ===
bool eeprom_read_bytes(uint16_t mem_address, uint8_t *buffer, uint16_t len) {
    i2c_start();

    if (USE_16BIT_ADDRESS) {
        if (!i2c_write_byte((EEPROM_I2C_ADDRESS << 1) | 0)) return false;
        if (!i2c_write_byte(mem_address >> 8)) return false;
        if (!i2c_write_byte(mem_address & 0xFF)) return false;
    } else {
        uint8_t addr = EEPROM_I2C_ADDRESS | ((mem_address >> 8) & 0x07);
        if (!i2c_write_byte((addr << 1) | 0)) return false;
        if (!i2c_write_byte(mem_address & 0xFF)) return false;
    }

    i2c_start();
    if (!i2c_write_byte((EEPROM_I2C_ADDRESS << 1) | 1)) return false;

    for (uint16_t i = 0; i < len; i++) {
        buffer[i] = i2c_read_byte(i < (len - 1));  // ACK all except last
    }

    i2c_stop();
    return true;
}

// === Store an array of uint16_t ===
bool eeprom_write_uint16_array(uint16_t mem_address, uint16_t *data, uint8_t count) {
    uint8_t buffer[count * 2];

    for (uint8_t i = 0; i < count; i++) {
        buffer[2 * i]     = data[i] >> 8;        // High byte
        buffer[2 * i + 1] = data[i] & 0xFF;      // Low byte
    }

    return eeprom_write_bytes(mem_address, buffer, count * 2);
}

// === Read an array of uint16_t ===
bool eeprom_read_uint16_array(uint16_t mem_address, uint16_t *data, uint8_t count) {
    uint8_t buffer[count * 2];
    if (!eeprom_read_bytes(mem_address, buffer, count * 2)) return false;

    for (uint8_t i = 0; i < count; i++) {
        data[i] = ((uint16_t)buffer[2 * i] << 8) | buffer[2 * i + 1];
    }

    return true;
}
