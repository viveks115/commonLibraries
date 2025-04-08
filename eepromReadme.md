eeprom_write_byte(0x000A, 0x5A);          // Write 0x5A to address 0x000A
uint8_t val;
eeprom_read_byte(0x000A, &val);           // Read from address 0x000A

uint8_t data[4] = {1, 2, 3, 4};
eeprom_write_page(0x0010, data, 4);       // Write 4 bytes to 0x0010

uint8_t buffer[4];
eeprom_read_block(0x0010, buffer, 4);     // Read 4 bytes from 0x0010



uint16_t voltage = 230;

// Store to EEPROM at address 0x0000
eeprom_write_uint16_array(0x0000, &voltage, 1);


uint16_t voltage_read = 0;

eeprom_read_uint16_array(0x0000, &voltage_read, 1);


uint16_t voltage = 230;
uint16_t current = 50;
uint16_t pf = 95;

// Write them at specific locations
eeprom_write_uint16_array(0x0001, &voltage, 1);  // Store voltage at address 1
eeprom_write_uint16_array(0x0005, &current, 1);  // Store current at address 5
eeprom_write_uint16_array(0x000A, &pf, 1);       // Store pf at address 10


uint16_t voltage_read = 0;
uint16_t current_read = 0;
uint16_t pf_read = 0;

eeprom_read_uint16_array(0x0001, &voltage_read, 1);
eeprom_read_uint16_array(0x0005, &current_read, 1);
eeprom_read_uint16_array(0x000A, &pf_read, 1);
