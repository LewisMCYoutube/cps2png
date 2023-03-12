#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <spng.h>

int prt_to_png(uint8_t *data, char *out_filename) {
	uint32_t sig = *((uint32_t *)(data));
	if (memcmp(&sig, "PRT\0", 4) != 0) {
		fprintf(stderr, "Tried to parse PRT data without the PRT signature\n");
		return -1;
	}
	uint16_t version = *((uint16_t *)(data + 4));
	if (version != 101 && version != 102 && version != 0) {
		fprintf(stderr, "Unknown PRT version %u\n", (unsigned int) version);
		return -1;
	}
	uint16_t bpp = *((uint16_t *)(data + 6));
	if (bpp != 8 && bpp != 24) {
		fprintf(stderr, "Unsupported BPP value %u\n", (unsigned int) bpp);
		return -1;
	}
	uint16_t palette_off = *((uint16_t *)(data + 8));
	uint16_t data_off = *((uint16_t *)(data + 10));
	uint16_t img_w = *((uint16_t *)(data + 12));
	uint16_t img_h = *((uint16_t *)(data + 14));
	int has_alpha = *((uint16_t *)(data + 16)) != 0;
	uint32_t offx, offy;
	if (version == 102) {
		offx = *((uint32_t *)(data + 20));
		offy = *((uint32_t *)(data + 24));
		if (*((uint32_t *)(data + 28)) != 0) {
			img_w = *((uint32_t *)(data + 28));
		}
		if (*((uint32_t *)(data + 32)) != 0) {
			img_h = *((uint32_t *)(data + 32));
		}
	} else {
		offx = 0;
		offy = 0;
	}
	unsigned long int stride = (img_w * (bpp / 8) + 3) & ~3;
	spng_ctx *ctx = NULL;
	struct spng_ihdr ihdr = {0};
	ctx = spng_ctx_new(SPNG_CTX_ENCODER);
	FILE *out_file = fopen(out_filename, "wb");
	if (out_file == NULL) {
		perror("Error opening output file");
		return -1;
	}
	spng_set_png_file(ctx, out_file);
	ihdr.width = img_w;
	ihdr.height = img_h;
	if (bpp == 8) {
		if (has_alpha) {
			ihdr.color_type = SPNG_COLOR_TYPE_TRUECOLOR_ALPHA;
			ihdr.bit_depth = 8;
			spng_set_ihdr(ctx, &ihdr);
			uint8_t *pixels;
			pixels = malloc((img_w * img_h) * 4);
			unsigned int stride_count = 0;
			unsigned long int j = (img_h * stride) - stride;
			uint8_t *alpha_map = data + data_off + (img_h * stride) - 1;
			unsigned long int n = 0;
			for (unsigned int t = 0; n < (img_w * img_h) * 4; n += 4, t++) {
				if (t == (img_w)) {
					if (stride_count == img_h - 1) { break; }
					stride_count += 1;
					j -= stride;
					t = 0;
				}
				pixels[n] = *((uint8_t *)(data + palette_off + (*(data + data_off + j + t) * 4) + 2));
				pixels[n + 1] = *((uint8_t *)(data + palette_off + (*(data + data_off + j + t) * 4) + 1));
				pixels[n + 2] = *((uint8_t *)(data + palette_off + (*(data + data_off + j + t) * 4)));
				pixels[n + 3] = *(++alpha_map);
			}
			spng_encode_image(ctx, pixels, (img_w * img_h) * 4, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE);
			free(pixels);
		} else {
			ihdr.color_type = SPNG_COLOR_TYPE_INDEXED;
			ihdr.bit_depth = 8;
			spng_set_ihdr(ctx, &ihdr);
			struct spng_plte plte = { 0 };
			uint32_t img_palette_length = data_off - palette_off;
			plte.n_entries = img_palette_length / 4;
			unsigned int i = 0;
			for (unsigned int n = 0; i < img_palette_length; n++, i = i + 4) {
				plte.entries[n].blue = *((uint8_t *)(data + palette_off + i));
				plte.entries[n].green = *((uint8_t *)(data + palette_off + i + 1));
				plte.entries[n].red = *((uint8_t *)(data + palette_off + i + 2));
			}
			uint8_t *pixels;
			pixels = malloc(img_w * img_h);
			for (unsigned int i = 0, n = img_h; i < img_h; i++, n--) {
				unsigned long row_offset = stride * i;
				memcpy(pixels + ((n * img_w) - img_w), data + data_off + row_offset, img_w);
			}
			spng_set_plte(ctx, &plte);
			spng_encode_image(ctx, pixels, img_w * img_h, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE);
			free(pixels);
		}
	} else if (bpp == 24) {
		if (has_alpha) {
			ihdr.color_type = SPNG_COLOR_TYPE_TRUECOLOR_ALPHA;
			ihdr.bit_depth = 8;
			spng_set_ihdr(ctx, &ihdr);
			uint8_t *alpha_map = data + data_off + (img_h * stride);
			uint32_t *pixels;
			pixels = malloc((img_w * img_h) * 4);
			uint32_t j = 0;
			for (uint32_t yi = data_off + stride * img_h - stride; yi >= data_off && j < img_w * img_h; yi -= stride) {
				for (uint32_t i = 0; i < img_w * 3; i += 3, j++) {
					uint8_t r = *((uint8_t *)(data + yi + i + 2));
					uint8_t g = *((uint8_t *)(data + yi + i + 1));
					uint8_t b = *((uint8_t *)(data + yi + i));
					pixels[j] = (uint32_t) (has_alpha ? (uint32_t) *(alpha_map++) << 24: 0xFF000000) | (uint32_t) b << 16 | (uint32_t) g << 8 | (uint32_t) r;
				}
			}
			spng_encode_image(ctx, pixels, (img_w * img_h) * 4, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE);
			free(pixels);
		} else {
			ihdr.color_type = SPNG_COLOR_TYPE_TRUECOLOR;
			ihdr.bit_depth = 8;
			spng_set_ihdr(ctx, &ihdr);
			uint8_t *pixels;
			pixels = malloc((img_w * img_h) * 3);
			uint32_t j = 0;
			for (uint32_t yi = data_off + stride * img_h - stride; yi >= data_off && j / 3 < img_w * img_h; yi -= stride) {
				for (uint32_t i = 0; i < img_w * 3; i += 3, j += 3) {
					pixels[j] = *((uint8_t *)(data + yi + i + 2));
					pixels[j + 1] = *((uint8_t *)(data + yi + i + 1));
					pixels[j + 2] = *((uint8_t *)(data + yi + i));
				}
			}
			spng_encode_image(ctx, pixels, (img_w * img_h) * 3, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE);
			free(pixels);
		}
	}
	spng_ctx_free(ctx);
	fclose(out_file);
	return 0;
}

void unpack_lnd(uint8_t *buf, uint8_t *out, uint32_t size, uint32_t unpacked_size) {
	uint32_t src = 0;
	uint32_t dst = 0;
	while (dst < unpacked_size && src < size) {
		uint32_t ctl = *((uint8_t *)(buf + src));
		src++;
		if (ctl & 0x80) {
			if (ctl & 0x40) {
				uint32_t count = (ctl & 0x1F) + 2;
				if (ctl & 0x20) {
					count += (uint32_t) *((uint8_t *)(buf + src)) << 5;
					src++;
				}
				if (unpacked_size - dst < count) {
					count = unpacked_size - dst;
				}
				uint8_t v = *((uint8_t *)(buf + src));
				src++;
				memset(out + dst, v, count);
				dst += count;
			} else {
				int count = ((ctl >> 2) & 0xF) + 2;
				int offset = ((ctl & 3) << 8) + *((uint8_t *)(buf + src)) + 1;
				src++;
				if (unpacked_size - dst < count) count = unpacked_size - dst;
				if (src + count > size) {
					fprintf(stderr, "Compressed data implies an out-of-bounds read - output may be incorrect\n");
					for (unsigned int i = 0; count > i; i++) {
						if (src + i > size) {
							dst += i;
							break;
						} else {
							memcpy(out + dst + i, out + dst - offset + i, 1);
						}
					}
					break;
				} else {
					memcpy(out + dst, out + dst - offset, count);
					dst += count;
				}
			}
		} else if (ctl & 0x40) {
			int length = (ctl & 0x3F) + 2 < unpacked_size - dst? (ctl & 0x3F) + 2: unpacked_size - dst;
			unsigned int count = *((uint8_t *)(buf + src));
			src++;
			memcpy(out + dst, buf + src, length);
			src += length;
			dst += length;
			count = count * length < unpacked_size - dst? count * length: unpacked_size - dst;
			uint32_t s = dst - length;
			while(count) {
				unsigned int preceding = dst - s < count? dst - s: count;
				memcpy(out + dst, out + s, preceding);
				dst += preceding;
				count -= preceding;
			}
		} else {
			uint32_t count = (ctl & 0x1F) + 1;
			if(ctl & 0x20) {
				count += (uint32_t)(*((uint8_t *)(buf + src)) << 5);
				src++;
			}
			if(unpacked_size - dst < count) count = unpacked_size - dst;
			memcpy(out + dst, buf + src, count);
			src += count;
			dst += count;
		}
	}
	if (unpacked_size > dst) {
		memset(out + dst, *((uint8_t *)(out + dst - 1)), unpacked_size - dst);
	}
}

static uint8_t* prepare_cps(uint8_t *buf, uint32_t size, uint32_t *unpacked_size_o) {
	uint16_t version;
	uint16_t compression;
	uint32_t unpacked_size;
	unsigned int data_offset;
	unsigned int unpacking_offset;
	if (memcmp(buf + 12, "bmp\0", 4) == 0) {
		version = *((uint16_t *)(buf + 4));
		compression = *((uint16_t *)(buf + 6));
		unpacked_size = *((uint32_t *)(buf + 8));
		*unpacked_size_o = unpacked_size;
		data_offset = 16;

		if (version != 100) {
			fprintf(stderr, "CPS file has unknown version\n");
			return 0;
		} else {
			unpacking_offset = data_offset;
			for (uint32_t j = 12; j < size - 4; j = j + 4) {
				*((uint32_t *)(buf + j)) -= 0x12345678 + size;
			}
		}
	} else if (memcmp(buf + 16, "bmp\0", 4) == 0) {
		uint32_t packed_size = *((uint32_t *)(buf + 4));
		if (size > packed_size) {
			size = packed_size;
		}
		uint32_t key_off = *((uint32_t *)(buf + size - 4)) - 0x7534682;
		if (key_off > size) {
			fprintf(stderr, "Key offset is out-of-bounds\n");
			return 0;
		}
		uint32_t key = *((uint32_t *)(buf + key_off)) + key_off + 0x3786425;
		version = *((uint16_t *)(buf + 8));
		compression = *((uint16_t *)(buf + 10));
		unpacked_size = *((uint32_t *)(buf + 12));
		*unpacked_size_o = unpacked_size;
		data_offset = 20;

		if (version == 102) {
			unpacking_offset = data_offset;
			if (key_off != 0) {
				for (uint32_t pos = 0x10; pos < size; pos += 4) {
					if (pos == size - 4) {
						*((uint32_t *)(buf + pos)) = 0;
						break;
					}
					uint32_t data = *((uint32_t *)(buf + pos));
					if (pos != key_off && key_off != 0) {
						data -= key + size;
					}
					*((uint32_t *)(buf + pos)) = data;
					key = 1103515245 * key + 39686;
				}
			}
		} else if (version == 104) {
			unpacking_offset = 32;
			uint16_t img_w = *((uint16_t *)(buf + 20));
			uint16_t img_h = *((uint16_t *)(buf + 22));
			uint16_t bpp = *((uint16_t *)(buf + 24));

			const uint32_t keys[8] = { 0x2623A189, 0x146FD8D7, 0x8E6F55FF, 0x1F497BCD, 0x1BB74F41, 0x0EB731D1, 0x5C031379, 0x64350881 };
			uint32_t j = 32;
			for (uint32_t i = 0; j < size - 4; j = j + 4, i++) {
				*((uint32_t *)(buf + j)) -= key + keys[i % 8] + size;
				key *= 0x41C64E6D;
				key += 0x9B06;
			}
		} else {
			fprintf(stderr, "CPS version unknown\n");
			return 0;
		}
	} else {
		fprintf(stderr, "'bmp' string is not at offset 12 or 16, so this CPS file is unsupported\n");
		return 0;
	}
	uint8_t *out;
	if (compression == 257) {
		out = malloc(unpacked_size);
		if (unpacking_offset != data_offset) {
			unpack_lnd(buf + unpacking_offset, out + (unpacking_offset - data_offset), size - unpacking_offset, unpacked_size);
			memcpy(out, buf, unpacking_offset - data_offset);
		} else {
			unpack_lnd(buf + unpacking_offset, out, size - unpacking_offset, unpacked_size);
		}
	} else if (compression == 256) {
		fprintf(stderr, "Unsupported type of compression\n");
		return 0;
	} else if (compression & 2) {
		fprintf(stderr, "LND16 compression is unsupported\n");
		return 0;
	} else if (compression == 0) {
		out = malloc(unpacked_size);
		memcpy(out, buf + data_offset, unpacked_size);
	} else {
		fprintf(stderr, "Unknown type of compression\n");
		return 0;
	}
	return out;
}
int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Usage: ./cps2png <file.cps> <file.png>\n");
		exit(EXIT_FAILURE);
	}
	FILE *cps_file;
	cps_file = fopen(argv[1], "rb");
	if (cps_file == NULL) {
		perror("Error opening CPS file");
		return -1;
	}

	long int size;
	fseek(cps_file, -1, SEEK_END);
	size = ftell(cps_file) + 1;
	rewind(cps_file);

	uint8_t *data;
	data = malloc(size);
	fread(data, 1, size, cps_file);
	fclose(cps_file);

	uint32_t unpacked_size;
	if (memcmp(data, "CPS\0", 4) == 0) {
		data = prepare_cps(data, size, &unpacked_size);
		if (data == 0) {
			exit(EXIT_FAILURE);
		}
		if (prt_to_png(data, argv[2]) == -1) {
			exit(EXIT_FAILURE);
		}
		free(data);
	} else if (memcmp(data, "PRT\0", 4) == 0) {
		if (prt_to_png(data, argv[2]) == -1) {
			exit(EXIT_FAILURE);
		}
		free(data);
	} else {
		fprintf(stderr, "Not a CPS or PRT file\n");
		free(data);
		exit(EXIT_FAILURE);
	}
}

// Copyright (c) 2023 project PHYCHO
// 
// Permission to use, copy, modify, and/or distribute this software
// for any purpose with or without fee is hereby granted.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
// OR PERFORMANCE OF THIS SOFTWARE.
//
// Based significantly on code from <https://github.com/malucard/eternal-legacy>,
// which was written by malucart and is available under the zlib license.
// See <https://github.com/LewisMCYoutube/cps2png> for full documentation.
