#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void rearrange_bytes(unsigned char *dest, const unsigned char *src) {
    dest[2] = src[0];   dest[6] = src[1];   dest[10] = src[2];  dest[14] = src[3];
    dest[18] = src[4];  dest[22] = src[5];  dest[26] = src[6];  dest[30] = src[7];
    dest[3] = src[8];   dest[7] = src[9];   dest[11] = src[10]; dest[15] = src[11];
    dest[19] = src[12]; dest[23] = src[13]; dest[27] = src[14]; dest[31] = src[15];
    dest[0] = src[16];  dest[4] = src[17];  dest[8] = src[18];  dest[12] = src[19];
    dest[16] = src[20]; dest[20] = src[21]; dest[24] = src[22]; dest[28] = src[23];
    dest[1] = src[24];  dest[5] = src[25];  dest[9] = src[26];  dest[13] = src[27];
    dest[17] = src[28]; dest[21] = src[29]; dest[25] = src[30]; dest[29] = src[31];
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input S rom> <output_file>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    FILE *infile = fopen(input_file, "rb");
    if (!infile) {
        perror("Error opening input file");
        return 1;
    }

    FILE *outfile = fopen(output_file, "wb");
    if (!outfile) {
        perror("Error opening output file");
        fclose(infile);
        return 1;
    }

    unsigned char buffer[32];
    unsigned char rearranged[32];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, 32, infile)) == 32) {
        rearrange_bytes(rearranged, buffer);
        fwrite(rearranged, 1, 32, outfile);
    }

    if (bytes_read > 0) {
        memset(buffer + bytes_read, 0, 32 - bytes_read);
        rearrange_bytes(rearranged, buffer);
        fwrite(rearranged, 1, 32, outfile);
    }

    fclose(infile);
    fclose(outfile);

    return 0;
}
