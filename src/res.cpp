//
// Created by aogun on 2021/3/5.
//

#include "res.h"
#include "mm_log.h"
#include "stb_image.h"

extern bool load_texture_from_memory(const uint8_t * image_data, void ** out_srv,
                              int width, int height);

static const unsigned int zoom_in_compressed_size = 1565;
static const unsigned int zoom_in_compressed_data[1568/4] =
{
0x0000bc57, 0x00000000, 0x07060000, 0x00000400, 0x4e50892f, 0x1a0a0d47, 0x0000000a, 0x4448490d, 0x20078252, 0x21038328, 0x05820608, 0xb8fe8c23,
0x2606826d, 0x474b6206, 0x83ff0044, 0xa0d70d01, 0x0093a7bd, 0x49bc0500, 0x58544144, 0x6dd7d585, 0x18555450, 0xb9fff007, 0x65ddeef7, 0x97961517,
0x5055931d, 0x794d4454, 0x26ca05f7, 0x4a69c445, 0xc99b329a, 0xd33a698f, 0x4d8a6334, 0x3125f136, 0x89a69a75, 0x53358f94, 0xa6c01f94, 0x12706121,
0x437008df, 0xe4414454, 0x5dc5451d, 0xdeeeee11, 0x10fa73b7, 0xdc0a251a, 0xe3e7ad05, 0xf33cf73d, 0x79ef67db, 0xff8173ee, 0x48c99078, 0x99f9f59a,
0x1c841cf9, 0x30e7c88b, 0xd55939e0, 0x6cc001c3, 0x7008fbe2, 0xc5e8afc3, 0x59715a14, 0x53d37a6e, 0x9fe56605, 0xa688b14d, 0xf1279e02, 0xf8c3b389,
0x9621d125, 0x846629b0, 0x0003314e, 0xfa32033c, 0xb7d46406, 0xad75fc7b, 0x68a32977, 0xbdb65490, 0x35cd3722, 0x42a4c0a9, 0xa24a2997, 0x36473c85,
0x0673486e, 0x208b88ad, 0x6a72791c, 0x3554329d, 0x352561dd, 0xd29d3e5d, 0xff1b1ea2, 0x925b1fde, 0xae0709d5, 0x1ecaa2fd, 0x68b284c4, 0xadc2f047,
0xcd5ae76b, 0xfb5c7e26, 0xe1c75565, 0xfb6f66f2, 0x5541a92d, 0x8fd2ace4, 0x06133c32, 0x99742a4c, 0x6cf4839c, 0x25f0bcea, 0x8b47a56f, 0xb1ed14a3,
0x745018c1, 0x77394dba, 0x577d6eed, 0xbc3e9730, 0xc0205c92, 0x185a01a8, 0x0f8e08e3, 0x84000704, 0x62329b00, 0xf04718c4, 0x3b20e722, 0x30cd7c34,
0x6cff2b30, 0x8d902f0a, 0xb5ced65b, 0x8e1b8205, 0xae73377c, 0x94d8f095, 0x149d5f99, 0x9a22d030, 0x3996720a, 0xccf2c683, 0x835c3d6d, 0xc66b87ad,
0x86c79390, 0xd5906754, 0x040417c2, 0x155bdccc, 0x7e24f3c7, 0x0988fcf9, 0xe22ba3ef, 0x58210822, 0x66b7bd9c, 0xc810601e, 0xe24e7c0b, 0x5a3f3b1c,
0x9b94312b, 0xb18c4823, 0x1ba78473, 0x20c08300, 0xf9e1b713, 0x8996a159, 0x4b1dfda5, 0x662d43a2, 0x0c01c851, 0x51651902, 0xcd26d391, 0x6669c887,
0x00270650, 0x28124608, 0xb34e8594, 0x7b5c469a, 0x691861d2, 0xb4d6f0ed, 0xd885ae47, 0xb53aa844, 0x682b4603, 0xf278d364, 0x31818033, 0x450560c0,
0x25fbb881, 0xc8647a75, 0xa4662da3, 0xdca83c8a, 0x85236b48, 0x0a205247, 0x7a301bbc, 0x7dd2398a, 0xc8dfbefb, 0xdf4531d6, 0x873c197d, 0xd020c02e,
0xea4bd7e7, 0x2693dddb, 0xf8f0efac, 0x062d32bd, 0xb429020c, 0x3756bae2, 0x3fc4e9d5, 0x38653a8d, 0x1aa6eadf, 0x10600958, 0xde9b9658, 0xaa1a28c4,
0xc4e8fa9a, 0x57b380f2, 0xd94a50ef, 0xcb5d8ac5, 0x0f0601ae, 0x2a480009, 0xd3525edb, 0xd6353279, 0x380cea3e, 0x53e1c3d6, 0xe9ad1c74, 0x7d35fbf2,
0x866b83fb, 0xdc8a8587, 0xeba69734, 0x35950fe7, 0xc084350f, 0xb0a03182, 0x294bd9bc, 0xd77ca1fb, 0x8180baf2, 0xdf6a07d9, 0x70484427, 0x9e3f144b,
0x390808e9, 0xedcab034, 0x752f0677, 0x86db7307, 0x3e6f8daf, 0x5fdb807f, 0x093447bb, 0x8d4aae9f, 0x87a739b7, 0x26e5bea3, 0x59b9b42e, 0x11284c42,
0x66fdf1f8, 0xebe3a29f, 0x57bd5763, 0x6a2ffbba, 0xe4eeb2b2, 0x80a5cb07, 0x6b07d980, 0xb0a22df3, 0x059485e3, 0x088b20b6, 0xd4d0d555, 0x6f7bc371,
0xb89d7540, 0x15661bdc, 0xcc2a2105, 0xa68fa63a, 0xd0ee5733, 0xfb358e6f, 0xdfaa0dee, 0x5f780ff7, 0x45e4f8af, 0x1befbfef, 0x18c4bcfb, 0x5cf07f59,
0xda62d581, 0x6f1c919c, 0x64024d13, 0x7389c545, 0xab2a928d, 0xa5774b7b, 0xafcccded, 0xf3320a4e, 0xc5871c05, 0x6e763189, 0x25a87469, 0xf619aa6c,
0xe0001160, 0x77d0541e, 0x7685c65f, 0xcdaebfb7, 0x17652943, 0x7dbe9afd, 0x086733f8, 0x2391c6f8, 0xfa2bafbc, 0xfcc59b3b, 0x02d2466a, 0x50f0209b,
0x6aa70d34, 0xac9f251b, 0x33ba597c, 0xfbf0cf6d, 0x9eafecd7, 0x6396322b, 0x0a1c8516, 0x0d1aa944, 0x40b30003, 0xdd2108fa, 0x512b597e, 0xd07e2908,
0xa3149e4a, 0x9de55702, 0x6b366c10, 0xd629d4d1, 0xce972b75, 0x0f0209b7, 0xd271d34d, 0xfaf5e8d5, 0x774b6f94, 0x2d1b6ea6, 0x44f12047, 0x7eafbae0,
0xf84a7335, 0xb629f6cf, 0xf309658c, 0x81c73cac, 0xae538a52, 0xd780d22b, 0x4774b47f, 0x64203bca, 0xc79b8752, 0xf257b91e, 0x094e565c, 0x3ed22dff,
0xe2796335, 0x431c21fc, 0x34b37d55, 0xa58ef920, 0x9e6ca60c, 0x98f01c6c, 0x7deac937, 0xa04c69ae, 0x90b08e95, 0xb34594b8, 0x3010842d, 0x6a5ed430,
0xa5fa7af1, 0xdde492d3, 0x26479794, 0x263735fc, 0xd3fcfae0, 0xa7a044a1, 0xb1ec3866, 0x16317109, 0x380c0010, 0xdbd5a977, 0xa3afe9e7, 0x55afbbe6,
0x9ed6ccee, 0xe011ee06, 0x3b559fcb, 0xa2aae260, 0x2c6722a3, 0xf19a758a, 0x57750d97, 0xdcb7fcda, 0xa0d24b9e, 0x5e654d77, 0x70b4fcb6, 0xdeae00ff,
0x99a66677, 0x33b1efc3, 0x660b111d, 0xe0fc3d47, 0x90ee6868, 0x5f76dc6f, 0xabbd0693, 0x9a5ef32b, 0x31f529bc, 0xf3200041, 0x008c4d40, 0x758c2f54,
0x3a71ce86, 0x5f2ee21e, 0xbb5b52eb, 0x3ac9b76f, 0x777c7a97, 0xc6d3ff72, 0x34154301, 0x6e28ae13, 0x39f08cf6, 0x53c3c74e, 0xa3ded4dd, 0xd5b9ef5e,
0xe57e13c3, 0x4301c2b3, 0x74a3466d, 0x1119ace9, 0x9d2d710f, 0xdb737abd, 0xccbadbcd, 0xc3e5f8af, 0x8567bee4, 0x3c808603, 0xdeb85dc7, 0x3a6babd9,
0x2f674b45, 0x71e96c6d, 0xa2babacb, 0x9be4e3fc, 0x0c0712cf, 0xca14c5fd, 0xbd1d2d9b, 0xed6d5d3f, 0xae781049, 0xedec234e, 0x3bd3bbe3, 0x7fee359f,
0x5a027f11, 0xfa4b69d1, 0x00f18216, 0x49000000, 0xae444e45, 0x05826042, 0xe9da04fa, 0x000000b5,
};

//-----------------------------------------------------------------------------
// [SECTION] Decompression code
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array and encoded as base85.
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
// Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------

static unsigned int stb_decompress_length(const unsigned char *input)
{
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char *stb__barrier_out_e, *stb__barrier_out_b;
static const unsigned char *stb__barrier_in_b;
static unsigned char *stb__dout;
static void stb__match(const unsigned char *data, unsigned int length)
{
    // INVERSE of memmove... write each byte before copying the next...
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_out_b) { stb__dout = stb__barrier_out_e+1; return; }
    while (length--) *stb__dout++ = *data++;
}

static void stb__lit(const unsigned char *data, unsigned int length)
{
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_in_b) { stb__dout = stb__barrier_out_e+1; return; }
    memcpy(stb__dout, data, length);
    stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static const unsigned char *stb_decompress_token(const unsigned char *i)
{
    if (*i >= 0x20) { // use fewer if's for cases that expand small
        if (*i >= 0x80)       stb__match(stb__dout-i[1]-1, i[0] - 0x80 + 1), i += 2;
        else if (*i >= 0x40)  stb__match(stb__dout-(stb__in2(0) - 0x4000 + 1), i[2]+1), i += 3;
        else /* *i >= 0x20 */ stb__lit(i+1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
    } else { // more ifs for cases that expand large, since overhead is amortized
        if (*i >= 0x18)       stb__match(stb__dout-(stb__in3(0) - 0x180000 + 1), i[3]+1), i += 4;
        else if (*i >= 0x10)  stb__match(stb__dout-(stb__in3(0) - 0x100000 + 1), stb__in2(3)+1), i += 5;
        else if (*i >= 0x08)  stb__lit(i+2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
        else if (*i == 0x07)  stb__lit(i+3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
        else if (*i == 0x06)  stb__match(stb__dout-(stb__in3(1)+1), i[4]+1), i += 5;
        else if (*i == 0x04)  stb__match(stb__dout-(stb__in3(1)+1), stb__in2(4)+1), i += 6;
    }
    return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen)
{
    const unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
    unsigned long blocklen = buflen % 5552;

    unsigned long i;
    while (buflen) {
        for (i=0; i + 7 < blocklen; i += 8) {
            s1 += buffer[0], s2 += s1;
            s1 += buffer[1], s2 += s1;
            s1 += buffer[2], s2 += s1;
            s1 += buffer[3], s2 += s1;
            s1 += buffer[4], s2 += s1;
            s1 += buffer[5], s2 += s1;
            s1 += buffer[6], s2 += s1;
            s1 += buffer[7], s2 += s1;

            buffer += 8;
        }

        for (; i < blocklen; ++i)
            s1 += *buffer++, s2 += s1;

        s1 %= ADLER_MOD, s2 %= ADLER_MOD;
        buflen -= blocklen;
        blocklen = 5552;
    }
    return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

static unsigned int stb_decompress(unsigned char *output, const unsigned char *i, unsigned int /*length*/)
{
    if (stb__in4(0) != 0x57bC0000) return 0;
    if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
    const unsigned int olen = stb_decompress_length(i);
    stb__barrier_in_b = i;
    stb__barrier_out_e = output + olen;
    stb__barrier_out_b = output;
    i += 16;

    stb__dout = output;
    for (;;) {
        const unsigned char *old_i = i;
        i = stb_decompress_token(i);
        if (i == old_i) {
            if (*i == 0x05 && i[1] == 0xfa) {
                if (stb__dout != output + olen) return 0;
                if (stb_adler32(1, output, olen) != (unsigned int) stb__in4(2))
                    return 0;
                return olen;
            } else {
                return 0;
            }
        }
        if (stb__dout > output + olen)
            return 0;
    }
}

std::shared_ptr<raw_image> get_raw_image(raw_image_id_t id) {
    std::shared_ptr<raw_image> res;
    uint8_t * compressed_ptr = nullptr;
    uint32_t compressed_size = 0;
    switch (id) {
        case ID_RES_IMAGE_ZOOM_IN: {
            compressed_ptr = (uint8_t *)zoom_in_compressed_data;
            break;
        }
        default:
            MM_LOG_ERROR("invalid res id %d", (int)id);
            return res;
    }
    auto size = stb_decompress_length(compressed_ptr);
    auto decompressed_data = new uint8_t[size];
    stb_decompress(decompressed_data, compressed_ptr, compressed_size);

    int width, height, channels;
    auto data = stbi_load_from_memory(decompressed_data, size, &width, &height, &channels, 4);
    delete[] decompressed_data;

    if (data == nullptr) {
        MM_LOG_ERROR("load image from res id %d failed", (int)id);
        return res;
    }
    res = std::make_shared<raw_image>(data, width, height);

    return res;
}

raw_image::raw_image(uint8_t *data, int width, int height) {
    m_data = data;
    m_width = width;
    m_height = height;
}

raw_image::~raw_image() {
    delete[] m_data;
}

void * get_inner_texture(raw_image_id_t id) {
    auto res = get_raw_image(id);
    if (!res) return nullptr;

    void * texture = nullptr;

    if (!load_texture_from_memory(res->data(), &texture, res->width(), res->height())) {
        MM_LOG_ERROR("load_texture_from_memory faild for id %d", id);
        return nullptr;
    }
    return texture;
}
