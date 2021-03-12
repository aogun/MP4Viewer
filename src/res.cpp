//
// Created by aogun on 2021/3/5.
//

#include "common.h"
#include "res.h"
#include "mm_log.h"
#include "stb_image.h"

extern bool load_texture_from_memory(const uint8_t * image_data, void ** out_srv,
                              int width, int height);
extern bool load_texture_from_memory_win(const uint8_t * image_data, void ** out_srv,
                                         int width, int height, mp4_raw_type_t type);

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

static const unsigned int play_compressed_size = 2083;
static const unsigned int play_compressed_data[2084/4] =
        {
                0x0000bc57, 0x00000000, 0x0d080000, 0x00000400, 0x4e50892f, 0x1a0a0d47, 0x0000000a, 0x4448490d, 0x20078252, 0x21038340, 0x05820608, 0x7169aa23,
                0x260682de, 0x474b6206, 0x83ff0044, 0xa0dd0f01, 0x0093a7bd, 0x49c20700, 0x78544144, 0x6b9aed9c, 0x15d55570, 0xce75bf80, 0x290109b9, 0xc0ec50c8,
                0xc34ea81f, 0x2d404140, 0x8a46028f, 0xf224b483, 0x4a0a52f0, 0x8283c901, 0xec041550, 0x0f286e58, 0xc95a8641, 0x2101b908, 0x404d2856, 0x05b1d010,
                0xd88cbc21, 0x03044b14, 0x87f1d09d, 0x907169d8, 0xee3c860a, 0x6e3fabdd, 0x37b858b0, 0x4373dc9c, 0xb337cd3a, 0xeb33dc7e, 0x9ef6bdac, 0xeefb38f5,
                0xd0b42d0d, 0xfc2d0b42, 0x946e22ff, 0xe934067b, 0x432cf38b, 0xf6ff5a98, 0xadc864a6, 0x245f235c, 0xc026bbc4, 0xf97ca372, 0x95b52c9f, 0x07436da7,
                0x0ec1d0db, 0x985d54f3, 0x6866dd53, 0x4c1ca246, 0xe022ae34, 0x1e7a19ae, 0x0fcaa8e8, 0x78473e04, 0x80001819, 0x9867cdb2, 0xbf94115d, 0x84cfc027,
                0x6c022ae1, 0xc0db4743, 0xbfb275d9, 0x507c9d72, 0x77df0779, 0xc36db30c, 0xc4795b25, 0x277d1f08, 0x5d413347, 0xc4adec2d, 0xb23da395, 0x0a3e0154,
                0x4bded7ca, 0x2d9da69d, 0x63023587, 0xb11d1feb, 0xd727590c, 0x7e47854b, 0x11770c26, 0x6ff911a0, 0x9f22bcce, 0x9c650456, 0x56c32613, 0x60cad9ce,
                0xcaf44e11, 0x7aefe7d4, 0x72bd9342, 0x5ab81136, 0x2444c700, 0x641be51a, 0x77c75db7, 0x392f309d, 0xcc31db06, 0x651546ff, 0x80eba6fd, 0x144d1d78,
                0xdee052ae, 0x28149169, 0x7c1a457b, 0x1d14dfaf, 0x81045962, 0x050280ee, 0x8ef30d76, 0x2d5e6e6d, 0xc4116dd3, 0xd701118e, 0x1c937c72, 0xd7d0cdbe,
                0x86dae431, 0x8590db3a, 0x53018a8f, 0x6fc36c74, 0x22ae1d37, 0x234fd060, 0x616ef011, 0x2d7011c3, 0x2f68a703, 0xfe9440a0, 0xa8f68ff5, 0xaafa5392,
                0x2763739c, 0x01abb811, 0x94b7111a, 0x4b0f926e, 0x07c832bb, 0xc9728ed9, 0xe0f94651, 0x3ac9d0e4, 0xf2ba8087, 0x96126bc1, 0x1011d702, 0x8250b073,
                0xb774b5c9, 0x436c3b0c, 0x62f0db7b, 0x6925255a, 0x8deed13f, 0xae371ab4, 0xa4f87942, 0xdbec0a48, 0xfc0a7d7c, 0x8abb54bc, 0x592114b2, 0x07aba594,
                0x6cb107c9, 0xfd51383d, 0x9d01e42a, 0x15846a80, 0xacb0d7b7, 0xdd6a58b8, 0x9e2f14f8, 0xcc2f8045, 0xd1328e1b, 0x95b38c31, 0xd51de7f7, 0x6d328115,
                0xded6cd1d, 0xf21c522d, 0x0984ea91, 0x3b6486da, 0x397fa2cc, 0xd49c8d94, 0x93d32db4, 0x0700210f, 0x11ad5d7a, 0xca38e28e, 0x94d53533, 0xc3ac382f,
                0xd92bd9a3, 0xdedf1724, 0x9876d92a, 0x38ce1b34, 0x73092f4a, 0xcd04e474, 0xb6d0c8cf, 0x38d2a6ee, 0x8e822b3c, 0xc02cd722, 0x07d3b955, 0xac3b9f7b,
                0xda038145, 0xb7dd9a31, 0x9b50e48a, 0xc81d7d10, 0x4f909475, 0xd38522c9, 0x31d04f0f, 0x1ee3a6ee, 0x06e052cf, 0xfc56bf93, 0xf1667362, 0x668acd61,
                0xae58b8bb, 0x012adaec, 0x6cfb885f, 0xa1b6fbc3, 0xf5e615b3, 0xeddd7e31, 0x77176bf7, 0xbc79bad3, 0x0bd8814b, 0xb074a380, 0x0f4bf90d, 0x0fcb5bea,
                0x6368d768, 0xe52edf6c, 0xa064f777, 0x72b6d903, 0xa94656d9, 0xd7efd272, 0x32ec44ac, 0xf1fc195d, 0xf81748b8, 0xbbebf6fd, 0x75128ac0, 0x855085f3,
                0x3fdbb8b0, 0xbc7e3558, 0x4d29a384, 0x8d5dad18, 0x1e515ac3, 0xc8747faa, 0x6ddfec82, 0xfc6ed397, 0x9b67858e, 0x92ba9f21, 0xf3c33ed6, 0xb2937c9f,
                0xdef407d5, 0xdd6de368, 0x6d171f29, 0x61b69193, 0xde70cfbc, 0x0cb50c31, 0xab1d1fc7, 0x935fbf01, 0xc6f54ddc, 0x1a05d978, 0x281c7d23, 0x7f44a97d,
                0xb41fc2f3, 0xb771b44d, 0x536290ec, 0x04a5aa4b, 0x6595a56d, 0xe2129458, 0xac3d8f4f, 0x73bda3fd, 0x115534b3, 0x638e258c, 0x47c6dd98, 0xfa7dbaf9,
                0xb7b1b460, 0xff9f29bc, 0xdb64a776, 0x0c701e61, 0xf4c3b57f, 0xa7779476, 0xb4fe6866, 0x53665a38, 0xe486c116, 0xc7f096db, 0x28b47782, 0xdccb463f,
                0xb60e5bb8, 0x0d8fa64b, 0xb612b6b9, 0xa94b2c95, 0x475d3c78, 0x047a9bbb, 0x3c03bf2f, 0xfbf15b11, 0xb0dc9cc2, 0xf3472757, 0x72c5d7e3, 0x092efd65,
                0x51c03058, 0x174a3b8e, 0xc663b29f, 0xd27c2d28, 0x5b0929af, 0xc0a50480, 0x1f0ee48d, 0x57babe14, 0x522a2b5f, 0xc13f9a3b, 0x5bfc72dd, 0x28efe997,
                0x95436cf3, 0x575c9865, 0x2dc4cac5, 0xe0536240, 0xd4293246, 0x7cb1618e, 0x2848940a, 0xe5594def, 0x056cd218, 0xcdd130cb, 0xca371c9c, 0xd1c2b6b1,
                0x545e2352, 0x8dcb69b9, 0x5abbaad9, 0x4c935880, 0x9b2802c8, 0x3fb18fcb, 0x4d204a96, 0xe664aa9d, 0xfd7161a8, 0x866ecda3, 0x4d011770, 0x801700b7,
                0x25231985, 0xc1694b18, 0x2d5bed70, 0x7b865814, 0xd6151fea, 0x9bbe665c, 0x0a639a81, 0xfcc38714, 0x3e5d3d2c, 0xe4f0a68b, 0xd3694d40, 0x2caaf986,
                0xce024944, 0xf25acc09, 0x3b74ec8e, 0x808a7358, 0x8ce429bf, 0x8dec91f8, 0x03dcf4a5, 0xf908bf7a, 0x23015218, 0x354af042, 0x951e5773, 0xb4198d4b,
                0x08404439, 0x24bae5e5, 0x42311d9e, 0xb4a68cbe, 0x5edeb07c, 0x28bcf363, 0x52822899, 0xbf59162e, 0x1ac72fdc, 0xb8b7323d, 0xc5fc2a08, 0x3c1cc8b6,
                0xe962ca4a, 0xfd5fb92d, 0xe83e790a, 0x61057284, 0x2c67dd65, 0xfe2a745b, 0x556f0fff, 0x2c29540a, 0x7b1d573e, 0x9864bc3b, 0xe4c27176, 0xccfd5ca1,
                0xb22c643d, 0x6ef12d9f, 0x522b7c3b, 0x61b0ede0, 0xf231a0c6, 0x00853471, 0x9836dd6a, 0x2acb5ae9, 0x51702da0, 0x47d3fe60, 0xd3c41028, 0x0459e37e,
                0xeb761af8, 0xfb0b2882, 0xdadd898c, 0xbda21c5e, 0xf280528d, 0x3d94003d, 0x954f3928, 0x6eb9b9ca, 0xdc49bd9c, 0x464680d5, 0xb5c50880, 0x3becccb5,
                0xd347ce5e, 0x53503d59, 0x84be616c, 0x12416858, 0xacc2a9f0, 0xc3b2a539, 0xf16e8f8d, 0x4e4406ae, 0x57f6bf80, 0x8459c299, 0x65a31e9c, 0x79bb2c5f,
                0x18a87483, 0x03ba51f2, 0x8a608546, 0x1bc7dcc5, 0xe0dcdad9, 0x072c1165, 0x19ffaeee, 0xeb756512, 0xa73b7224, 0x77b4554b, 0xf6d1f636, 0x20acc935,
                0xc591264c, 0xd640d0fb, 0x3de4f7cc, 0x83c57e37, 0x18504577, 0x50afca2d, 0x77965192, 0xd1f6247d, 0x34fbfad8, 0x6d6b043d, 0x8fea1743, 0xc97ddac6,
                0xb41ef1b2, 0xf6783736, 0x522dd01d, 0xc86b1fd5, 0xdabe86c6, 0x44f22c59, 0x087a1c89, 0x4d643187, 0xad77b3fd, 0x459e0dcd, 0x50044340, 0x4c7eceeb,
                0xd2855857, 0x08a2e516, 0xac65b381, 0x58712009, 0x6acf0f8e, 0xbb5e2c40, 0x21897b47, 0xa7757e88, 0xc9484f70, 0xd6e27c99, 0xe848905f, 0xd34c6c02,
                0xb22ead56, 0xa42cc240, 0x3ca1b5be, 0xbc84e131, 0xe252ceee, 0x140a5848, 0x5c21d6f7, 0x2925cff8, 0xadd58211, 0xb1e8f798, 0xca4975e6, 0x957822b3,
                0xbdb47d1d, 0x99557218, 0xc73fe445, 0x1cc84693, 0x5ae9a277, 0x4d3c1b9b, 0x353ddd81, 0x05ac435d, 0xa9d408ba, 0xc35382f2, 0x4a561fa2, 0x263e1c4d,
                0xe0524f14, 0xeced548d, 0x1941ac58, 0x11c2828b, 0x4fd2b22c, 0xb2abff35, 0x00404571, 0x60f606b5, 0x214b2aa6, 0x9e50dad2, 0xf59a7119, 0x12daef5e,
                0xf1e3ab85, 0xeb91a97d, 0x0330c0f2, 0x0d652c97, 0x3d3bd5c2, 0xb31642db, 0xdff8aa1f, 0x2eef2eba, 0xca136002, 0x57ff605c, 0x27ca35c7, 0x761ccc2a,
                0xee797646, 0x6ae0135d, 0xfc393a01, 0xa20d42e2, 0x0c50d316, 0xe412614a, 0x5e6ca80e, 0x1685adad, 0x85a1685a, 0x7f8e6e16, 0x8883a203, 0x479b953b,
                0x00000044, 0x4e454900, 0x6042ae44, 0x41fa0582, 0x0015dec4,
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
        case ID_RES_IMAGE_PLAY: {
            compressed_ptr = (uint8_t *)play_compressed_data;
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

bool load_texture_from_memory_ex(const uint8_t * image_data, void ** out_texture,
                                 int width, int height, mp4_raw_type_t type) {
    return load_texture_from_memory_win(image_data, out_texture, width, height, type);
}


