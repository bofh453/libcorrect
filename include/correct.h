#ifndef CORRECT_H
#define CORRECT_H
#include <stdint.h>
#ifndef _MSC_VER
#include <unistd.h>
#ifdef __MINGW32__
#define ssize_t int
#endif
#else
#define ssize_t int
#endif

// Convolutional Codes

// Convolutional polynomials are 16 bits wide
typedef uint16_t correct_convolutional_polynomial_t;

static const correct_convolutional_polynomial_t correct_conv_r12_6_polynomial[] = {073, 061};
static const correct_convolutional_polynomial_t correct_conv_r12_7_polynomial[] = {0161, 0127};
static const correct_convolutional_polynomial_t correct_conv_r12_8_polynomial[] = {0225, 0373};
static const correct_convolutional_polynomial_t correct_conv_r12_9_polynomial[] = {0767, 0545};
static const correct_convolutional_polynomial_t correct_conv_r13_6_polynomial[] = {053, 075, 047};
static const correct_convolutional_polynomial_t correct_conv_r13_7_polynomial[] = {0137, 0153,
                                                                                   0121};
static const correct_convolutional_polynomial_t correct_conv_r13_8_polynomial[] = {0333, 0257,
                                                                                   0351};
static const correct_convolutional_polynomial_t correct_conv_r13_9_polynomial[] = {0417, 0627,
                                                                                   0675};

typedef uint8_t correct_convolutional_soft_t;

struct correct_convolutional;
typedef struct correct_convolutional correct_convolutional;

/* correct_convolutional_create allocates and initializes an encoder/decoder for
 * a convolutional code with the given parameters. This function expects that
 * poly will contain inv_rate elements. E.g., to create a conv. code instance
 * with rate 1/2, order 7 and polynomials 0161, 0127, call
 * correct_convolutional_create(2, 7, []correct_convolutional_polynomial_t{0161, 0127});
 *
 * If this call is successful, it returns a non-NULL pointer.
 */
correct_convolutional *correct_convolutional_create(size_t inv_rate, size_t order,
                                                    const correct_convolutional_polynomial_t *poly);

/* correct_convolutional_destroy releases all resources associated
 * with conv. This pointer should not be used for further calls
 * after calling destroy.
 */
void correct_convolutional_destroy(correct_convolutional *conv);

/* correct_convolutional_encode_len returns the number of *bits*
 * in a msg_len of given size, in *bytes*. In order to convert
 * this returned length to bytes, save the result of the length
 * modulo 8. If it's nonzero, then the length in bytes is
 * length/8 + 1. If it is zero, then the length is just
 * length/8.
 */
size_t correct_convolutional_encode_len(correct_convolutional *conv, size_t msg_len);

/* correct_convolutional_encode uses the given conv instance to
 * encode a block of data and write it to encoded. The length of
 * encoded must be long enough to hold the resulting encoded length,
 * which can be calculated by calling correct_convolutional_encode_len.
 * However, this length should first be converted to bytes, as that
 * function returns the length in bits.
 *
 * This function returns the number of bits written to encoded. If
 * this is not an exact multiple of 8, then it occupies an additional
 * byte.
 */
size_t correct_convolutional_encode(correct_convolutional *conv, const uint8_t *msg, size_t msg_len,
                                    uint8_t *encoded);

/* correct_convolutional_decode uses the given conv instance to
 * decode a block encoded by correct_convolutional_encode. This
 * call can cope with some bits being corrupted. This function
 * cannot detect if there are too many bits corrupted, however,
 * and will still write a message even if it is not recovered
 * correctly. It is up to the user to perform checksums or CRC
 * in order to guarantee that the decoded message is intact.
 *
 * num_encoded_bits should contain the length of encoded in *bits*.
 * This value need not be an exact multiple of 8. However,
 * it must be a multiple of the inv_rate used to create
 * the conv instance.
 *
 * This function writes the result to msg, which must be large
 * enough to hold the decoded message. A good conservative size
 * for this buffer is the number of encoded bits multiplied by the
 * rate of the code, e.g. for a rate 1/2 code, divide by 2. This
 * value should then be converted to bytes to find the correct
 * length for msg.
 *
 * This function returns the number of bytes written to msg.
 */
size_t correct_convolutional_decode(correct_convolutional *conv, const uint8_t *encoded,
                                    size_t num_encoded_bits, uint8_t *msg);

/* correct_convolutional_decode_soft uses the given conv instance
 * to decode a block encoded by correct_convolutional_encode and
 * then modulated/demodulated to 8-bit symbols. This function expects
 * that 1 is mapped to 255 and 0 to 0. An erased symbol should be
 * set to 128. The decoded message may contain errors.
 *
 * num_encoded_bits should contain the length of encoded in *bits*.
 * This value need not be an exact multiple of 8. However,
 * it must be a multiple of the inv_rate used to create
 * the conv instance.
 *
 * This function writes the result to msg, which must be large
 * enough to hold the decoded message. A good conservative size
 * for this buffer is the number of encoded bits multiplied by the
 * rate of the code, e.g. for a rate 1/2 code, divide by 2. This
 * value should then be converted to bytes to find the correct
 * length for msg.
 *
 * This function returns the number of bytes written to msg.
 */
size_t correct_convolutional_decode_soft(correct_convolutional *conv,
                                         const correct_convolutional_soft_t *encoded,
                                         size_t num_encoded_bits, uint8_t *msg);

#endif
