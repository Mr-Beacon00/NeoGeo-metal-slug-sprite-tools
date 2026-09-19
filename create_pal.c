#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t read_uint8(FILE *file) {
    uint8_t value;
    if (fread(&value, sizeof(uint8_t), 1, file) != 1) {
        perror("failed to read byte");
        exit(EXIT_FAILURE);
    }
    return value;
}

// original bits: abcdefghijklmnop -> swaped bits: aefghbijklcmnopd
uint16_t swap_bits(uint16_t value) {
    uint16_t swapped = 0;
    swapped |= ((value >> 15) & 1) << 15; // a
    swapped |= ((value >> 11) & 1) << 14; // e
    swapped |= ((value >> 10) & 1) << 13; // f
    swapped |= ((value >> 9) & 1) << 12;  // g
    swapped |= ((value >> 8) & 1) << 11;  // h
    swapped |= ((value >> 14) & 1) << 10; // b
    swapped |= ((value >> 7) & 1) << 9;   // i
    swapped |= ((value >> 6) & 1) << 8;   // j
    swapped |= ((value >> 5) & 1) << 7;   // k
    swapped |= ((value >> 4) & 1) << 6;   // l
    swapped |= ((value >> 13) & 1) << 5;  // c
    swapped |= ((value >> 3) & 1) << 4;   // m
    swapped |= ((value >> 2) & 1) << 3;   // n
    swapped |= ((value >> 1) & 1) << 2;   // o
    swapped |= ((value >> 0) & 1) << 1;   // p
    swapped |= ((value >> 12) & 1) << 0;  // d
    return swapped;
}

// read color pool file to buffer
void read_pal_from_memory(uint16_t addr, uint8_t *pal_data, size_t pal_size, uint8_t *buffer) {
    if (addr + 1 >= pal_size) {
        fprintf(stderr, "pal address out of range: 0x%04X (pal size: %zu)\n", addr, pal_size);
        exit(EXIT_FAILURE);
    }
    buffer[0] = pal_data[addr];
    buffer[1] = pal_data[addr + 1];
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input index file> <input color pool file> <output palette>\n", argv[0]);
        return 1;
    }

    char *input_index_file = argv[1];
    char *input_pal_file = argv[2];
    char *output_file = argv[3];

    // 1. read color pool and swap bits
    FILE *pal_file = fopen(input_pal_file, "rb");
    if (!pal_file) {
        perror("failed to open color pool file");
        return EXIT_FAILURE;
    }
    fseek(pal_file, 0, SEEK_END);
    long pal_size = ftell(pal_file);
    fseek(pal_file, 0, SEEK_SET);

    uint8_t *pal_data = (uint8_t *)malloc(pal_size);
    if (!pal_data) {
        perror("failed to allocate color pool buffer");
        fclose(pal_file);
        return EXIT_FAILURE;
    }
    if (fread(pal_data, 1, pal_size, pal_file) != (size_t)pal_size) {
        perror("failed to read color pool file");
        fclose(pal_file);
        free(pal_data);
        return EXIT_FAILURE;
    }
    fclose(pal_file);

    // bits swap
    for (long i = 0; i + 1 < pal_size; i += 2) {
        uint16_t value = pal_data[i] | (pal_data[i + 1] << 8);
        uint16_t swapped = swap_bits(value);
        pal_data[i] = swapped & 0xFF;
        pal_data[i + 1] = (swapped >> 8) & 0xFF;
    }

    // 2. color index
    FILE *file = fopen(input_index_file, "rb");
    if (!file) {
        perror("failed to open index");
        free(pal_data);
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    long num_entries = file_size / 4;

    uint8_t *buffer = (uint8_t *)malloc(num_entries * 2);
    if (!buffer) {
        perror("failed to allocate buffer");
        fclose(file);
        free(pal_data);
        return EXIT_FAILURE;
    }

    uint8_t *current_buffer_pos = buffer;

    for (long i = 0; i < file_size; i += 4) {
        uint8_t index1 = read_uint8(file);
        uint8_t index2 = read_uint8(file);
        fseek(file, 1, SEEK_CUR);
        uint8_t index3 = read_uint8(file);
        uint16_t result = index1 * 0x40 + index2 * 0x800 + index3 * 0x02;

        read_pal_from_memory(result, pal_data, pal_size, current_buffer_pos);
        current_buffer_pos += 2;
    }

    fclose(file);

    // 3. output
    FILE *output_name = fopen(output_file, "wb");
    if (!output_name) {
        perror("output failed");
        free(buffer);
        free(pal_data);
        return EXIT_FAILURE;
    }

    if (fwrite(buffer, 1, num_entries * 2, output_name) != num_entries * 2) {
        perror("fwrite failed");
    }

    fclose(output_name);
    free(buffer);
    free(pal_data);

    return EXIT_SUCCESS;
}
