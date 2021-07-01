#ifndef SIMAGE_H
#define SIMAGE_H

#include <stdio.h>
#include <string.h>

#include "sTypes.h"
#include "sLogging.h"
#include "sDebug.h"

typedef struct {
    u32 width;
    u32 height;
    u8 bpp;
    u8 *pixels;
    u32 size;

} PNG_Image;

typedef enum {
    PNG_UNKNOWN = 0,
    PNG_TYPE_IHDR,
    PNG_TYPE_PLTE,
    PNG_TYPE_IDAT,
    PNG_TYPE_IEND
} PNG_PacketType;

typedef struct {
    u32 length;
    PNG_PacketType type;
    u32 type_u32;
    u8 *data;
    u32 crc;
} PNG_Packet;

typedef struct {
    u32 width;
    u32 height;
    u8 bit_depth;
    u8 color_type;
    u8 compression;
    u8 filter;
    u8 interlaced;
} PNG_IHDR;

typedef struct PNG_DataChunk PNG_DataChunk;

struct PNG_DataChunk {
    u8 *data;
    u32 size;
    PNG_DataChunk *next;
};

typedef struct {
    PNG_DataChunk *first;
    PNG_DataChunk *last;

    void *contents;
    u32 contents_size;

    u32 bits_left;
    u32 bit_buffer;

} PNG_DataStream;

typedef struct {
    u8 cm;
    u8 cinfo;
    u8 fcheck;
    bool fdict;
    u8 flevel;

} PNG_IDAT;

void StreamFlushBits(PNG_DataStream *stream) {
    stream->bits_left = 0;
    stream->bit_buffer = 0;
}

#define StreamRead(stream, type) *(type *)StreamReadSize(stream, sizeof(type))

void *StreamReadSize(PNG_DataStream *stream, u32 size) {
    void *result = 0;

    for(;;) {
        if(size <= stream->contents_size) {
            result = stream->contents;
            stream->contents = (u8 *)stream->contents + size;
            stream->contents_size -= size;
            break;
        } else if((stream->contents_size == 0) && stream->first) {
            stream->first = stream->first->next;
            stream->contents_size = stream->first->size;
            stream->contents = stream->first->data;
        } else {
            break;
        }
    }
    if(!result) {
        sError("File underflow");
        stream->contents_size = 0;
    }

    return result;
}

u32 StreamReadBits(PNG_DataStream *stream, u8 count) {
    u32 result = 0;
    u8 bits_remaining = count;

    while(stream->bits_left < count && stream->contents_size) {
        u32 byte = StreamRead(stream, u8);
        stream->bit_buffer |= byte << stream->bits_left;
        stream->bits_left += 8;
    }

    if(stream->bits_left >= count) {
        stream->bits_left -= count;
        result |= stream->bit_buffer & ((1 << count) - 1);
        stream->bit_buffer >>= count;
    }
    /*
    while(bits_remaining > 0) {
        u8 bits_to_read = 0;
        if(8 - stream->bits_left == 0) {
            StreamFlushBits(stream);
            continue;
        }
        if(8 - stream->bits_left < bits_remaining) {
            bits_to_read = 8 - stream->bits_left;
        } else {
            bits_to_read = bits_remaining;
        }

        const u8 byte = *(u8 *)stream->contents >> (stream->bits_left);
        const u8 mask = (1 << bits_to_read) - 1;

        stream->bits_left += bits_to_read;
        bits_remaining -= bits_to_read;
        result = result << bits_remaining;
    }
    */
    return result;
}

void StreamAppendChunk(PNG_DataStream *stream, PNG_DataChunk *chunk) {
    if(!stream->first) {
        stream->first = chunk;
        stream->contents = chunk->data;
        stream->contents_size = chunk->size;
    }
    if(stream->last) {
        stream->last->next = chunk;
    }
    stream->last = chunk;
}

// Goes through the input array, and writes the amount of occurences of the lengths at index length.
// If there are three 4 size codes in the input, output[4] will be set to 3
void HummanGetLengthCounts(const u32 input_size,
                           const u32 *input,
                           const u32 output_size,
                           u32 *output) {
    for(u32 i = 0; i < input_size; ++i) {
        output[input[i]]++;
    }
}

// Returns the maximum value of the array given
u32 HuffmanGetMaxLength(const u32 array_size, const u32 *array) {
    u32 result = 0;
    for(u32 i = 0; i < array_size; ++i) {
        if(result < array[i])
            result = array[i];
    }
    return result;
}

// Generates the first values to use for each code length following the Huffman algorithm
void HuffmanCreateFirstLengthValues(const u32 *length_counts,
                                    u32 *first_lengths_values,
                                    const u32 size) {
    u32 code = 0;
    for(u32 i = 1; i < size; ++i) {
        code = (code + (length_counts[i - 1])) << 1;
        if(length_counts[i] > 0)
            first_lengths_values[i] = code;
    }
}

void HuffmanPrint(const u32 size, const u32 *huffman) {
    for(u32 i = 0; i < size; i++) {
        sTrace(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(huffman[i]));
    }
}

void HuffmanCompute(const u32 size, const u32 *lengths, u32 *huffman_table) {
    u32 max_length = HuffmanGetMaxLength(size, lengths);

    // We will store here the amount of occurences of length i. ie : length_counts[9] == 8 > there are 8 codes with a length of 9
    u32 *length_counts = sMalloc(sizeof(u32) * max_length + 1);
    length_counts[0] = 0;
    HummanGetLengthCounts(size, lengths, max_length, length_counts);
    // We will store here the next value to assign to a length i. if we want to query the next value for length 4 -> length_values[4].
    u32 *length_values = sMalloc(sizeof(u32) * max_length + 1);

    length_values[0] = 0;

    HuffmanCreateFirstLengthValues(length_counts, length_values, max_length + 1);

    for(u32 i = 0; i < size; ++i) {
        if(lengths[i] > 0) {
            huffman_table[i] = length_values[lengths[i]]++;
        }
    }
}

void PNGPrintHeader(const u8 *header) {
    sTrace(
        "PNG : Header \n      %0.2X\n      %0.2X %0.2X %0.2X\n      %0.2X %0.2X\n      %0.2X\n      %0.2X",
        header[0],
        header[1],
        header[2],
        header[3],
        header[4],
        header[5],
        header[6],
        header[7]);
}

void PNGReadPacket(FILE *file, PNG_Packet *packet) {
    // Length
    fread(&packet->length, sizeof(u32), 1, file);
    packet->length = swap_u32(packet->length);
    //sTrace("PNG : Length : %d", packet->length);

    // Type
    fread(&packet->type_u32, sizeof(char), 4, file);
    char str[5] = {'\0'};
    memcpy(str, &packet->type_u32, 4);
    packet->type_u32 = swap_u32(packet->type_u32);

    switch(packet->type_u32) {
    case('IHDR'): packet->type = PNG_TYPE_IHDR; break;
    case('IDAT'): packet->type = PNG_TYPE_IDAT; break;
    case('PLTE'): packet->type = PNG_TYPE_PLTE; break;
    case('IEND'): packet->type = PNG_TYPE_IEND; break;
    default: packet->type = PNG_UNKNOWN;
    }

    if(packet->type != PNG_UNKNOWN) {
        sTrace("PNG : Type %s", str);
    } else {
        sWarn("PNG : Type %s (unhandled)", str);
    }

    // TODO maybe we don't need to allocate and copy here ?
    packet->data = sMalloc(packet->length * sizeof(u8));
    fread(packet->data, sizeof(u8), packet->length, file);
    /*
    sTrace("PNG : Packet data");
    for(u32 i = 0; i < packet->length; ++i) {
        srTrace("%0.2X", packet->data[i]);
    }
    srTrace("\n");
    */

    // CRC
    fread(&packet->crc, sizeof(u32), 1, file);
    //sTrace("PNG : CRC : %d", packet->crc);
}

void sLoadImage(const char *path, PNG_Image *image) {
    // Check extension
    u32 length = strlen(path);
    u32 fmt_index = length - 3;
    char extension[4];
    memcpy(extension, path + fmt_index, 3);
    extension[3] = '\0';
    if(strcmp(extension, "png") != 0 && strcmp(extension, "PNG") != 0) {
        sError("Error : Unsupported image format %s", extension);
        return;
    }

    sTrace("PNG : Begin");
    FILE *file;
    fopen_s(&file, path, "rb");
    u8 header[8];
    fread(header, sizeof(u8), 8, file);

    PNG_DataStream stream = {0};

    // Parse
    for(;;) {
        sTrace("----");
        PNG_Packet packet = {};
        PNGReadPacket(file, &packet);

        if(packet.type == PNG_TYPE_IEND)
            break;

        switch(packet.type) {
        case PNG_TYPE_IHDR: {
            PNG_IHDR hdr = {};
            memcpy(&hdr, packet.data, sizeof(PNG_IHDR));

            if(hdr.interlaced == 1) {
                sError("Image is interlaced, this isn't supported yet");
                return;
            }

            if(hdr.color_type != 2) {
                sError("Image isn't RGB, this isn't supported yet");
                return;
            }
            if(hdr.filter != 0) {
                sError("Filters are unsupported yet");
                return;
            }

            image->width = swap_u32(hdr.width);
            image->height = swap_u32(hdr.height);

            switch(hdr.color_type) {
            //case(0): image->bpp = hdr.bit_depth; break;
            case(2): image->bpp = (hdr.bit_depth * 3) / 8; break;
            //case(3): image->bpp = hdr.bit_depth ; break;
            case(4): image->bpp = (hdr.bit_depth * 2) / 8; break; // Grayscale + Alpha
            case(6): image->bpp = (hdr.bit_depth * 4) / 8; break; // RGB + Alpha
            }

            image->size = image->width * image->height * image->bpp;
            image->pixels = sMalloc(image->size);
            sTrace("%dx%d (%dbpp)", image->width, image->height, image->bpp);
        } break;
        case PNG_TYPE_IDAT: {
            // https://www.ietf.org/rfc/rfc1950.txt

            PNG_DataChunk *chunk = sMalloc(sizeof(PNG_DataChunk));

            chunk->data = packet.data;
            chunk->size = packet.length;

            StreamAppendChunk(&stream, chunk);

        } break;
        default: {
            sTrace("PNG : Skipping packet");
            continue;
        }
        }
    }
    fclose(file);

    sTrace("PNG : Decode");

    // Copy
    // https://www.ietf.org/rfc/rfc1951.txt
    u8 *out_ptr = image->pixels;
    bool bfinal = 0;
    do {
        PNG_IDAT idat = {};
        idat.cm = StreamReadBits(&stream, 4);
        idat.cinfo = StreamReadBits(&stream, 4);
        idat.fcheck = StreamReadBits(&stream, 5);
        idat.fdict = StreamReadBits(&stream, 1);
        idat.flevel = StreamReadBits(&stream, 2);

        sTrace("CM : %d", idat.cm, idat.cinfo);

        if(idat.fdict) {
            sError("ADLER32 in this stream, this isn't handled.");
        }

        bfinal = StreamReadBits(&stream, 1);

        u8 btype = StreamReadBits(&stream, 2);
        StreamFlushBits(&stream);
        if(btype == 0) { // Uncompressed
            u32 len = StreamRead(&stream, u32);
            u32 nlen = StreamRead(&stream, u32);
        } else {
            if(btype == 2) { // Dynamic Huffman tree
                // TODO read the huffman tree
                u32 HLIT = StreamReadBits(&stream, 5);
                u32 HDIST = StreamReadBits(&stream, 5);
                u32 HCLEN = StreamReadBits(&stream, 4);

                HLIT += 257;
                HDIST += 1;
                HCLEN += 4;

                ASSERT(HCLEN < 19);
                u32 HCLENLengthTable[19] = {};
                const u32 HCLENSwizzle[] = {
                    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

                for(u32 i = 0; i < HCLEN; i++) {
                    u32 value = StreamReadBits(&stream, 3);
                    HCLENLengthTable[HCLENSwizzle[i]] = value;
                }
                sTrace("dkdk");
            } else if(btype == 1) {
            }

            const u32 FIXED_LENGTH_TABLE[] = {3,  4,  5,  6,   7,   8,   9,   10,  11, 13,
                                              15, 17, 19, 23,  27,  31,  35,  43,  51, 59,
                                              67, 83, 99, 115, 131, 163, 195, 227, 258};
            const u32 FIXED_DISTANCE_TABLE[] = {1,    2,    3,    4,     5,     7,    9,    13,
                                                17,   25,   33,   49,    65,    97,   129,  193,
                                                257,  385,  513,  769,   1025,  1537, 2049, 3073,
                                                4097, 6145, 8193, 12289, 16385, 24577};
            {
                u32 code = StreamReadBits(&stream, 9);
                u32 length = 0;
                if(code <= 255) {
                    length = code;
                } else if(code == 256) {
                    // End
                } else {
                    length = FIXED_LENGTH_TABLE[code - 257];

                    u32 extra_bits = 0;
                    // TODO optimize a modulo could work here
                    if(code >= 265 && code <= 268) {
                        extra_bits = 1;
                    } else if(code >= 269 && code <= 272) {
                        extra_bits = 2;
                    } else if(code >= 273 && code <= 276) {
                        extra_bits = 3;
                    } else if(code >= 277 && code <= 280) {
                        extra_bits = 4;
                    } else if(code >= 281 && code <= 284) {
                        extra_bits = 5;
                    } else if(code == 285) {
                        extra_bits = 0;
                    }
                    length += StreamReadBits(&stream, extra_bits);
                    // ! This could be backwards : "The extra bits should be interpreted as a machine integer stored with the -significant bit first, e.g., bits 1110 represent the value 14.
                }
            }
            {
                u32 code = StreamReadBits(&stream, 5);
                u32 extra_bits = 0;
                if(code >= 4) {
                    extra_bits = ((code - 4) % 2) + ((code - 4) / 2);
                }
                u32 distance = FIXED_DISTANCE_TABLE[code];
                distance += StreamReadBits(&stream, extra_bits); // This could be backwards
            }
            /*
            for(;;) {
                u32 code = Read(head);
                if(code == 256)
                    break; // Loop
                else if(code < 256) {
                    // Copy
                    *out_ptr = code;
                    out_ptr++;
                } else {
                    // Read size
                    u32 distance = Read();
                    u8 *read_ptr = out_ptr - distance;
                    for(u32 c = 0; c < code; c++) {
                        *out_ptr = *read_ptr;
                        out_ptr++;
                        read_ptr++;
                    }
                }
            }
            */
        }
    } while(bfinal == 0);

    // TODO : Consolidate data
    // Gather all sizes
    // Alloc a buffer big enough
    // Memcpy all of it
    // Free the chunks

    sTrace("PNG : End");
    return;
}

#endif