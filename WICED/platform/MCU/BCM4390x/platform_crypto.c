/*
 * Copyright 2018, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
#include <string.h>
#include "platform_crypto.h"
#include "RTOS/wwd_rtos_interface.h"
static unsigned int is_crypto_inited = WICED_FALSE;

void wiced_hwcrypto_init( void )
{
    if ( is_crypto_inited == WICED_FALSE )
    {
        platform_hwcrypto_init( );
        is_crypto_inited = WICED_TRUE;
    }
}

#if defined(MBEDTLS_AES_ALT)

void wiced_hw_aes_init( wiced_hw_aes_context *ctx )
{
    memset( ctx, 0, sizeof(wiced_hw_aes_context) );
    return;
}

void wiced_hw_aes_free( wiced_hw_aes_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }

    memset( ctx, 0, sizeof(wiced_hw_aes_context) );

    return;
}

/*
 * AES key schedule (same for encryption or decryption, as hardware handles schedule)
 *
 */

int wiced_hw_aes_setkey_enc( wiced_hw_aes_context *ctx, const unsigned char *key, unsigned int keybits )
{
    /* AES supported key lengths are 128bits, 192 bits and 256 bits. return Invalid key length error for
       key length other than the supported lengths */
    if ( keybits != AES128_KEYSIZE_BITS && keybits != AES192_KEYSIZE_BITS && keybits != AES256_KEYSIZE_BITS )
    {
        return -0x0020;
    }

    hw_aes_setkey_enc( ctx, (unsigned char *) key, keybits );

    return PLATFORM_SUCCESS;
}

/*
 * AES key schedule (same for encryption or decryption, as hardware handles schedule)
 *
 */

int wiced_hw_aes_setkey_dec( wiced_hw_aes_context *ctx, const unsigned char *key, unsigned int keybits )
{
    /* AES supported key lengths are 128bits, 192 bits and 256 bits. return Invalid key length error for
       key length other than the supported lengths */
    if ( keybits != AES128_KEYSIZE_BITS && keybits != AES192_KEYSIZE_BITS && keybits != AES256_KEYSIZE_BITS )
    {
        return -0x0020;
    }

    hw_aes_setkey_dec( ctx, (unsigned char *) key, keybits );

    return PLATFORM_SUCCESS;
}

/*
 * AES-ECB block encryption
 */

void wiced_hw_aes_encrypt( wiced_hw_aes_context *ctx, const unsigned char input[ 16 ], unsigned char output[ 16 ] )
{
    hw_aes_crypt_ecb( ctx, 1, input, output );
    return;
}

/*
 * AES-ECB block decryption
 */

void wiced_hw_aes_decrypt( wiced_hw_aes_context *ctx,
        const unsigned char input[16],
        unsigned char output[16] )
{
    hw_aes_crypt_ecb( ctx, 0, input, output );
    return;
}

/*
 * AES-ECB block encryption/decryption
 */

int wiced_hw_aes_crypt_ecb( wiced_hw_aes_context *ctx, int mode, const unsigned char input[ 16 ], unsigned char output[ 16 ] )
{
    unsigned char input_buf[16] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char output_buf[16] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    memcpy(input_buf,input,16);
    mode = ( mode == HW_AES_DECRYPT ) ? HW_AES_ENCRYPT : HW_AES_DECRYPT;

    hw_aes_crypt_ecb( ctx, mode, input_buf, output_buf );
    memcpy(output,output_buf,16);

    return PLATFORM_SUCCESS;
}

/*
 * AES-CBC buffer encryption/decryption
 */

int wiced_hw_aes_crypt_cbc( wiced_hw_aes_context *ctx, int mode, size_t length, unsigned char iv[ 16 ], const unsigned char *input, unsigned char *output )
{
    unsigned char input_buf[16] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char output_buf[16] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    /* AES input length should be multiple of 16, if input length is not
       multiple of 16, return Invalid data input length error */
    if ( length % AES_BLOCK_SIZE )
    {
        return ( -0x0022 );
    }

    mode = ( mode == HW_AES_DECRYPT ) ? HW_AES_ENCRYPT : HW_AES_DECRYPT;

    while ( length != 0 )
    {
        memcpy( input_buf, input, 16);
        hw_aes_crypt_cbc( ctx, mode, 16, iv, input_buf, output_buf );
        memcpy(output,output_buf,16);

        input += 16;
        output += 16;
        length -= 16;
    }

    return PLATFORM_SUCCESS;
}

/*
 * AES-CFB128 buffer encryption/decryption
 */

int wiced_hw_aes_crypt_cfb128( wiced_hw_aes_context *ctx, int mode, size_t length, size_t *iv_off, unsigned char iv[ 16 ], const unsigned char *input, unsigned char *output )
{
    mode = ( mode == HW_AES_DECRYPT ) ? HW_AES_ENCRYPT : HW_AES_DECRYPT;

    hw_aes_crypt_cfb( ctx, mode, length, (uint32_t*) iv_off, iv, input, output );

    return PLATFORM_SUCCESS;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */

int wiced_hw_aes_crypt_cfb8( wiced_hw_aes_context *ctx, int mode, size_t length, unsigned char iv[ 16 ], const unsigned char *input, unsigned char *output )
{
    /* TODO : USE COMPLETE HARDWARE API's instead s/w and h/w combinations */
    unsigned char c;
    unsigned char ov[ 17 ];

    while ( length-- )
    {
        memcpy( ov, iv, 16 );
        wiced_hw_aes_crypt_ecb( ctx, 1 /* MBEDTLS_AES_ENCRYPT */, iv, iv );

        if ( mode == 0 /* MBEDTLS_AES_DECRYPT */)
        ov[ 16 ] = *input;

        c = *output++ = (unsigned char) ( iv[ 0 ] ^ *input++ );

        if ( mode == 1 /* MBEDTLS_AES_ENCRYPT */)
        ov[ 16 ] = c;

        memcpy( iv, ov + 1, 16 );
    }

    return ( 0 );
}

/*
 * AES-CTR buffer encryption/decryption
 */
int wiced_hw_aes_crypt_ctr( wiced_hw_aes_context *ctx, size_t length, size_t *nc_off, unsigned char nonce_counter[ 16 ], unsigned char stream_block[ 16 ], const unsigned char *input, unsigned char *output )
{
    /* TODO : USE COMPLETE HARDWARE API's instead s/w and h/w combinations */
    int c, i;
    size_t n = *nc_off;

    while ( length-- )
    {
        if ( n == 0 )
        {
            wiced_hw_aes_crypt_ecb( ctx, 1 /* AES_ENCRYPT */, nonce_counter, stream_block );

            for ( i = 16; i > 0; i-- )
            if ( ++nonce_counter[ i - 1 ] != 0 )
            break;
        }
        c = *input++;
        *output++ = (unsigned char) ( c ^ stream_block[ n ] );

        n = ( n + 1 ) & 0x0F;
    }

    *nc_off = n;

    return ( 0 );

    return PLATFORM_SUCCESS;
}
#endif /* defined(MBEDTLS_AES_ALT) */

#if defined(MBEDTLS_DES_ALT)
/********************DES START*************************/
void wiced_hw_des_init( wiced_hw_des_context *ctx )
{
    memset( ctx, 0, sizeof(wiced_hw_des_context) );
    return;
}

void wiced_hw_des_free( wiced_hw_des_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }

    memset( ctx, 0, sizeof(wiced_hw_des_context) );

    return;
}
void wiced_hw_des3_init( wiced_hw_des_context *ctx )
{
//    memset( ctx, 0, sizeof(wiced_hw_des_context) );
//    return;

    UNUSED_PARAMETER( ctx );

    return;
}

void wiced_hw_des3_free( wiced_hw_des_context *ctx )
{

//    if ( ctx == NULL )
//    {
//        return;
//    }
//
//    memset( ctx, 0, sizeof(wiced_hw_des_context) );
//
//    return;

    UNUSED_PARAMETER( ctx );
    return;
}

int wiced_hw_des_setkey_enc( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE ] )
{
    hw_des_setkey( ctx, (unsigned char*) key );
    return PLATFORM_SUCCESS;
}

int wiced_hw_des_setkey_dec( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE ] )
{
    hw_des_setkey( ctx, (unsigned char*) key );
    return PLATFORM_SUCCESS;
}

int wiced_hw_des3_set2key_enc( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE * 2 ] )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( key );
    return PLATFORM_UNSUPPORTED;
}

int wiced_hw_des3_set2key_dec( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE * 2 ] )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( key );
    return PLATFORM_UNSUPPORTED;
}

int wiced_hw_des3_set3key_enc( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE * 3 ] )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( key );
    return PLATFORM_UNSUPPORTED;
}

int wiced_hw_des3_set3key_dec( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE * 3 ] )
{

    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( key );
    return PLATFORM_UNSUPPORTED;
}
int wiced_hw_des_crypt_ecb( wiced_hw_des_context *ctx, const unsigned char input[ 8 ], unsigned char output[ 8 ] )
{
    uint32_t mode = ctx->direction;
    unsigned char input_buf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char output_buf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    memcpy(input_buf,input,8);

    mode = ( mode == HW_AES_DECRYPT ) ? HW_AES_ENCRYPT : HW_AES_DECRYPT;

    hw_des_crypt_ecb( ctx, mode, input_buf, output_buf );

    memcpy( output,output_buf,8);

    return PLATFORM_SUCCESS;
}

int wiced_hw_des3_crypt_ecb( wiced_hw_des_context *ctx, const unsigned char input[ 8 ], unsigned char output[ 8 ] )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( input );
    UNUSED_PARAMETER( output );

    return PLATFORM_UNSUPPORTED;
}

int wiced_hw_des_crypt_cbc( wiced_hw_des_context *ctx, int mode, size_t length, unsigned char iv[ 8 ], const unsigned char *input, unsigned char *output )
{
    unsigned char input_buf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char output_buf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    if ( length % 8 )
    {
        return ( -0x0032 );
    }

    mode = ( mode == HW_DES_DECRYPT ) ? HW_DES_ENCRYPT : HW_DES_DECRYPT;

    while(length != 0 )
    {
        memcpy( input_buf, input, 8);
        hw_des_crypt_cbc( ctx, mode, 8, iv, input_buf, output_buf );
        memcpy(output,output_buf,8);

        input += 8;
        output += 8;
        length -= 8;
    }

    return PLATFORM_SUCCESS;
}

int wiced_hw_des3_crypt_cbc( wiced_hw_des_context *ctx, int mode, size_t length, unsigned char iv[ 8 ], const unsigned char *input, unsigned char *output )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( length );
    UNUSED_PARAMETER( mode );
    UNUSED_PARAMETER( iv );
    UNUSED_PARAMETER( input );
    UNUSED_PARAMETER( output );

    return PLATFORM_UNSUPPORTED;
}
#endif /* #if defined(MBEDTLS_DES_ALT) */
/********************DES END*************************/

/********************MD5 STARTS*************************/

#if defined(MBEDTLS_MD5_ALT)
void wiced_hw_md5_init( wiced_hw_md5_context *ctx )
{
    uint8_t *payloadbuffer;
    uint32_t offset = 0;

    memset( &ctx->hw_ctx, 0, sizeof(md5_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_md5_context) );

    payloadbuffer = ctx->payload_buffer;
    if( ( ( (unsigned long)payloadbuffer) & (HWCRYPTO_ALIGNMENT_BYTES - 1) ) != 0)
    {
        offset = HWCRYPTO_ALIGNMENT_BYTES - ( ((unsigned long)payloadbuffer) & (HWCRYPTO_ALIGNMENT_BYTES - 1));
    }

    ctx->hw_ctx.payload_buffer = payloadbuffer + offset;

    return;
}

void wiced_hw_md5_free( wiced_hw_md5_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }
    memset( &ctx->hw_ctx, 0, sizeof(md5_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_md5_context) );

    return;
}

void wiced_hw_md5_clone( wiced_hw_md5_context *dst, const wiced_hw_md5_context *src )
{
    uint8_t *payload_buffer = dst->hw_ctx.payload_buffer;

    memcpy(dst,src,sizeof(wiced_hw_md5_context));
    dst->hw_ctx.payload_buffer = payload_buffer;
}

void wiced_hw_md5_starts( wiced_hw_md5_context *ctx )
{
    uint8_t *payload_buffer = ctx->hw_ctx.payload_buffer;

    /* reinitializing the context structures in _starts also
       to handle the sequence of calls init->starts->update->finish
       and after this again starts->update->finish->free
       ( w/o invoking free for first sequence, second set is called)
       storing aligned payload_buffer and restoring it after memset */
    memset( &ctx->hw_ctx, 0, sizeof(md5_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_md5_context) );

    ctx->hw_ctx.payload_buffer = payload_buffer;
    ctx->flags = HASH_STARTS;

    return;
}

void wiced_hw_md5_update( wiced_hw_md5_context *ctx, const unsigned char *input, size_t ilen )
{
    uint32_t fill = 0;
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    fill = 64 - ctx->last;
    ctx->total += ilen;

    if ( ctx->flags == HASH_STARTS && ctx->total >= 64)
    {
        /* first 64-bytes available, we can call starts crypto
           API */
        if ( ctx->last > 0)
        {
            /* first copy the previously stored bytes  to
               aligned buf and then from input buffer*/
            memcpy(buf, ctx->left_over_bytes, ctx->last);
        }
        memcpy(buf + ctx->last, input, fill);

        platform_hwcrypto_md5_starts( &ctx->hw_ctx, buf, 64 );

        input += fill;
        ilen  -= fill;

        ctx->last = 0;
        ctx->flags = HASH_UPDATE;
    }

    if ( ctx->last > 0 && ilen >= fill )
    {
        /* first copy the previously stored bytes  to
           aligned buf and then from input buffer*/
        memcpy( buf, ctx->left_over_bytes, ctx->last );
        memcpy( buf + ctx->last, input, fill);

        platform_hwcrypto_md5_update( &ctx->hw_ctx, buf, 64 );
        ctx->last = 0;
        input += fill;
        ilen  -= fill;
    }

    while ( ilen > 64 )
    {
        memcpy(buf, input, 64);
        platform_hwcrypto_md5_update( &ctx->hw_ctx, buf, 64 );

        input += 64;
        ilen -= 64;
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->left_over_bytes + ctx->last, input, ilen );
        ctx->last = ctx->last + ilen;
    }

    return;
}

void wiced_hw_md5_finish( wiced_hw_md5_context *ctx, unsigned char output[ MD5_HASH_SIZE ] )
{
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char outbuf[MD5_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    if ( ctx->total < 64 )
    {
        memcpy(buf, ctx->left_over_bytes, ctx->total);
        /* input/output buffer should be aligned for full hash also, else seeing a crash */
        platform_hwcrypto_md5( buf, ctx->total, outbuf);
    }
    else
    {
        memcpy(buf, ctx->left_over_bytes, ctx->last);

        platform_hwcrypto_md5_finish( &ctx->hw_ctx, buf, ctx->last, outbuf );
    }
    memcpy(output, outbuf, MD5_HASH_SIZE);

    return;
}

void wiced_md5_hmac_starts( wiced_hw_md5_hmac_context * ctx, const unsigned char *key, uint32_t keylen )
{
    uint32_t offset = 0;
    unsigned char *payloadbuf;

    memset( &ctx->ctx.hw_ctx, 0, sizeof( md5_context_t ) );
    memset( ctx, 0, sizeof( wiced_hw_md5_hmac_context ) );

    payloadbuf = ctx->ctx.payload_buffer;

    if( ( ( (unsigned long)payloadbuf) & (HWCRYPTO_ALIGNMENT_BYTES - 1) ) != 0)
    {
        offset = HWCRYPTO_ALIGNMENT_BYTES - ( ((unsigned long)payloadbuf) & (HWCRYPTO_ALIGNMENT_BYTES - 1));
    }

    ctx->ctx.hw_ctx.payload_buffer = payloadbuf + offset;

    platform_hwcrypto_md5_hmac_starts( &ctx->ctx.hw_ctx, key, keylen );

    return;
}

void wiced_md5_hmac_update( wiced_hw_md5_hmac_context * ctx, const unsigned char *input, uint32_t ilen )
{
    uint32_t fill = 0;
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    fill = 64 - ctx->ctx.last;
    ctx->ctx.total += ilen;

    if ( ctx->ctx.total >= 64)
    {
        /* first 64-bytes available, we can call starts crypto
           API */
        if ( ctx->ctx.last > 0)
        {
            /* first copy the previously stored bytes  to
               aligned buf and then from input buffer*/
            memcpy(buf, ctx->ctx.left_over_bytes, ctx->ctx.last);
        }
        memcpy(buf + ctx->ctx.last, input, fill);

        platform_hwcrypto_md5_hmac_update( &ctx->ctx.hw_ctx, buf, 64 );

        input += fill;
        ilen  -= fill;

        ctx->ctx.last = 0;
    }

    while ( ilen > 64 )
    {
        memcpy(buf, input, 64);
        platform_hwcrypto_md5_hmac_update( &ctx->ctx.hw_ctx, buf, 64 );

        input += 64;
        ilen -= 64;
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->ctx.left_over_bytes + ctx->ctx.last, input, ilen );
        ctx->ctx.last = ctx->ctx.last + ilen;
    }

    return;
}

void wiced_md5_hmac_finish( wiced_hw_md5_hmac_context * ctx, unsigned char output[ MD5_HASH_SIZE ] )
{
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char outbuf[MD5_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    memcpy(buf, ctx->ctx.left_over_bytes, 64);

    platform_hwcrypto_md5_hmac_finish( &ctx->ctx.hw_ctx, buf, (int32_t) ctx->ctx.last, outbuf );
    memcpy( output, outbuf, MD5_HASH_SIZE);

    return;
}

void wiced_md5_hmac( const unsigned char *key, int32_t keylen, const unsigned char *input, int32_t ilen, unsigned char output[ MD5_HASH_SIZE ] )
{
    wiced_hw_md5_hmac_context ctx;

    wiced_md5_hmac_starts( &ctx, key, (uint32_t) keylen);
    wiced_md5_hmac_update( &ctx, input, (uint32_t) ilen );
    wiced_md5_hmac_finish( &ctx, output );

    return;

}

void wiced_hw_md5_process( wiced_hw_md5_context *ctx, const unsigned char data[ 64 ] )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( data );

    return;
}

void wiced_hw_md5( const unsigned char *input, size_t ilen, unsigned char output[ MD5_HASH_SIZE ] )
{
    wiced_hw_md5_context ctx;

    wiced_hw_md5_init( &ctx );
    wiced_hw_md5_starts( &ctx );
    wiced_hw_md5_update( &ctx, input, ilen );
    wiced_hw_md5_finish( &ctx, output );
    wiced_hw_md5_free( &ctx );

    return;
}
#endif /* defined(MBEDTLS_MD5_ALT) */

/********************MD5 END*************************/

/********************SHA256 STARTS*************************/

#if defined(MBEDTLS_SHA256_ALT)
void wiced_hw_sha256_init( wiced_hw_sha256_context *ctx )
{
    uint8_t *payloadbuffer;
    uint32_t offset = 0;

    memset( &ctx->hw_ctx, 0, sizeof(sha256_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha256_context) );

    payloadbuffer = ctx->payload_buffer;

    if( ( ( (unsigned long)payloadbuffer) & (HWCRYPTO_ALIGNMENT_BYTES - 1) ) != 0)
    {
        offset = HWCRYPTO_ALIGNMENT_BYTES - ( ((unsigned long)payloadbuffer) & (HWCRYPTO_ALIGNMENT_BYTES - 1));
    }

    ctx->hw_ctx.payload_buffer = payloadbuffer + offset;

    return;
}

void wiced_hw_sha256_free( wiced_hw_sha256_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }
    memset( &ctx->hw_ctx, 0, sizeof(sha256_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha256_context) );

    return;
}

void wiced_hw_sha256_clone( wiced_hw_sha256_context *dst, const wiced_hw_sha256_context *src )
{
    uint8_t *payload_buffer = dst->hw_ctx.payload_buffer;

    memcpy(dst,src,sizeof(wiced_hw_sha256_context));
    dst->hw_ctx.payload_buffer = payload_buffer;
}

void wiced_hw_sha256_starts( wiced_hw_sha256_context *ctx, int is224 )
{
    uint8_t *payload_buffer = ctx->hw_ctx.payload_buffer;

    /* reinitializing the context structures in _starts also
       to handle the sequence of calls init->starts->update->finish
       and after this again starts->update->finish->free
       ( w/o invoking free for first sequence, second set is called)
       storing aligned payload_buffer and restoring it after memset */
    memset( &ctx->hw_ctx, 0, sizeof(sha256_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha256_context) );

    ctx->hw_ctx.payload_buffer = payload_buffer;
    ctx->flags = HASH_STARTS;
    ctx->hw_ctx.is224 = is224;
}

void wiced_hw_sha256_update( wiced_hw_sha256_context *ctx, const unsigned char *input, size_t ilen )
{
    uint32_t fill = 0;
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    fill = 64 - ctx->last;
    ctx->total += ilen;

    if ( ctx->flags == HASH_STARTS && ctx->total >= 64)
    {
        /* first 64-bytes available, we can call starts crypto
           API */
        if ( ctx->last > 0)
        {
            /* first copy the previously stored bytes  to
               aligned buf and then from input buffer*/
            memcpy(buf, ctx->left_over_bytes, ctx->last);
        }
        memcpy(buf + ctx->last, input, fill);

        platform_hwcrypto_sha2_starts( &ctx->hw_ctx, buf, 64, ctx->hw_ctx.is224 );

        input += fill;
        ilen  -= fill;

        ctx->last = 0;
        ctx->flags = HASH_UPDATE;
    }

    if ( ctx->last > 0 && ilen >= fill )
    {
        /* first copy the previously stored bytes  to
           aligned buf and then from input buffer*/
        memcpy( buf, ctx->left_over_bytes, ctx->last );
        memcpy( buf + ctx->last, input, fill);

        platform_hwcrypto_sha2_update( &ctx->hw_ctx, buf, 64 );
        ctx->last = 0;
        input += fill;
        ilen  -= fill;
    }

    while ( ilen > 64 )
    {
        memcpy(buf, input, 64);
        platform_hwcrypto_sha2_update( &ctx->hw_ctx, buf, 64 );

        input += 64;
        ilen -= 64;
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->left_over_bytes + ctx->last, input, ilen );
        ctx->last = ctx->last + ilen;
    }

    return;
}

void wiced_hw_sha256_finish( wiced_hw_sha256_context *ctx, unsigned char output[ SHA256_HASH_SIZE ] )
{
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char outbuf[SHA256_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    memcpy(buf, ctx->left_over_bytes, 64);

    if ( ctx->total < 64 )
    {
        /* input/output buffer should be aligned for full hash also, else seeing a crash */
        platform_hwcrypto_sha2( buf, ctx->total, outbuf, ctx->hw_ctx.is224 );
    }
    else
    {
        platform_hwcrypto_sha2_finish( &ctx->hw_ctx, buf, ctx->last, outbuf );
    }
    memcpy(output, outbuf, SHA256_HASH_SIZE);

    return;
}

void wiced_sha256_hmac_starts( wiced_hw_sha256_hmac_context * ctx, const unsigned char *key, uint32_t keylen , int32_t is224)
{
    uint32_t offset = 0;
    unsigned char *payloadbuf;

    memset( &ctx->ctx.hw_ctx, 0, sizeof(sha256_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha256_hmac_context) );

    payloadbuf = ctx->ctx.payload_buffer;

    if( ( ( (unsigned long)payloadbuf) & (HWCRYPTO_ALIGNMENT_BYTES - 1) ) != 0)
    {
        offset = HWCRYPTO_ALIGNMENT_BYTES - ( ((unsigned long)payloadbuf) & (HWCRYPTO_ALIGNMENT_BYTES - 1));
    }

    ctx->ctx.hw_ctx.payload_buffer = payloadbuf + offset;

    platform_hwcrypto_sha2_hmac_starts(&ctx->ctx.hw_ctx, key, keylen,is224);

    return;
}

void wiced_sha256_hmac_update( wiced_hw_sha256_hmac_context * ctx, const unsigned char *input, uint32_t ilen )
{
    uint32_t fill = 0;
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    fill = 64 - ctx->ctx.last;
    ctx->ctx.total += ilen;

    if ( ctx->ctx.total >= 64)
    {
        /* first 64-bytes available, we can call starts crypto
           API */
        if ( ctx->ctx.last > 0)
        {
            /* first copy the previously stored bytes  to
               aligned buf and then from input buffer*/
            memcpy(buf, ctx->ctx.left_over_bytes, ctx->ctx.last);
        }
        memcpy(buf + ctx->ctx.last, input, fill);

        platform_hwcrypto_sha2_hmac_update( &ctx->ctx.hw_ctx, buf, 64 );

        input += fill;
        ilen  -= fill;

        ctx->ctx.last = 0;
    }

    while ( ilen > 64 )
    {
        memcpy(buf, input, 64);
        platform_hwcrypto_sha2_hmac_update( &ctx->ctx.hw_ctx, buf, 64 );

        input += 64;
        ilen -= 64;
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->ctx.left_over_bytes + ctx->ctx.last, input, ilen );
        ctx->ctx.last = ctx->ctx.last + ilen;
    }

    return;
}

void wiced_sha256_hmac_finish( wiced_hw_sha256_hmac_context * ctx, unsigned char output[ SHA256_HASH_SIZE ] )
{
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char outbuf[SHA256_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    memcpy(buf, ctx->ctx.left_over_bytes, 64);
    platform_hwcrypto_sha2_hmac_finish( &ctx->ctx.hw_ctx, buf, (int32_t) ctx->ctx.last, outbuf );
    memcpy( output, outbuf, SHA256_HASH_SIZE);

    return;
}

void wiced_sha256_hmac( const unsigned char *key, int32_t keylen, const unsigned char *input, int32_t ilen, unsigned char output[ SHA256_HASH_SIZE ], int32_t is224 )
{
    wiced_hw_sha256_hmac_context ctx;

    wiced_sha256_hmac_starts( &ctx, key, (uint32_t) keylen , is224);
    wiced_sha256_hmac_update( &ctx, input, (uint32_t) ilen );
    wiced_sha256_hmac_finish( &ctx, output );
    return;
}

void wiced_hw_sha256_process( wiced_hw_sha256_context *ctx, const unsigned char data[ 64 ] )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( data );

    return;
}

void wiced_hw_sha256( const unsigned char *input, size_t ilen,unsigned char output[32], int is224 )
{
    //platform_hwcrypto_sha2( input, ilen, output, is224 );
    wiced_hw_sha256_context ctx;

    wiced_hw_sha256_init( &ctx );
    wiced_hw_sha256_starts( &ctx, is224 );
    wiced_hw_sha256_update( &ctx, input, ilen );
    wiced_hw_sha256_finish( &ctx, output );
    wiced_hw_sha256_free( &ctx );
    return;
}
#endif /* defined(MBEDTLS_SHA256_ALT) */
/********************SHA256 END*************************/

/********************SHA1 STARTS*************************/

#if defined(MBEDTLS_SHA1_ALT)

void wiced_hw_sha1_init( wiced_hw_sha1_context *ctx )
{
    uint8_t *payloadbuffer;
    uint32_t offset = 0;
    memset( &ctx->hw_ctx, 0, sizeof(sha1_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha1_context) );

    payloadbuffer = ctx->payload_buffer;

    if( ( ( (unsigned long)payloadbuffer) & (HWCRYPTO_ALIGNMENT_BYTES - 1) ) != 0)
    {
        offset = HWCRYPTO_ALIGNMENT_BYTES - ( ((unsigned long)payloadbuffer) & (HWCRYPTO_ALIGNMENT_BYTES - 1));
    }

    ctx->hw_ctx.payload_buffer = payloadbuffer + offset;
    return;
}

void wiced_hw_sha1_free( wiced_hw_sha1_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }
    memset( &ctx->hw_ctx, 0, sizeof(sha1_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha1_context) );

    return;
}

void wiced_hw_sha1_clone( wiced_hw_sha1_context *dst, const wiced_hw_sha1_context *src )
{
    uint8_t *payload_buffer = dst->hw_ctx.payload_buffer;

    memcpy(dst,src,sizeof(wiced_hw_sha1_context));
    dst->hw_ctx.payload_buffer = payload_buffer;
}

void wiced_hw_sha1_starts( wiced_hw_sha1_context *ctx)
{
    uint8_t *payload_buffer = ctx->hw_ctx.payload_buffer;
    /* reinitializing the context structures in _starts also
       to handle the sequence of calls init->starts->update->finish
       and after this again starts->update->finish->free
       ( w/o invoking free for first sequence, second set is called)
       storing aligned payload_buffer and restoring it after memset */
    memset( &ctx->hw_ctx, 0, sizeof(sha1_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha1_context) );

    ctx->hw_ctx.payload_buffer = payload_buffer;
    ctx->flags = HASH_STARTS;

    return;
}

void wiced_hw_sha1_update( wiced_hw_sha1_context *ctx, const unsigned char *input, size_t ilen )
{
    uint32_t fill = 0;
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    fill = 64 - ctx->last;
    ctx->total += ilen;

    if ( ctx->flags == HASH_STARTS && ctx->total >= 64)
    {
        /* first 64-bytes available, we can call starts crypto
           API */
        if ( ctx->last > 0)
        {
            /* first copy the previously stored bytes  to
               aligned buf and then from input buffer*/
            memcpy(buf, ctx->left_over_bytes, ctx->last);
        }
        memcpy(buf + ctx->last, input, fill);

        platform_hwcrypto_sha1_starts( &ctx->hw_ctx, buf, 64 );

        input += fill;
        ilen  -= fill;

        ctx->last = 0;
        ctx->flags = HASH_UPDATE;
    }

    if ( ctx->last > 0 && ilen >= fill )
    {
        /* first copy the previously stored bytes  to
           aligned buf and then from input buffer*/
        memcpy( buf, ctx->left_over_bytes, ctx->last );
        memcpy( buf + ctx->last, input, fill);

        platform_hwcrypto_sha1_update( &ctx->hw_ctx, buf, 64 );
        ctx->last = 0;
        input += fill;
        ilen  -= fill;
    }

    while ( ilen > 64 )
    {
        memcpy(buf, input, 64);
        platform_hwcrypto_sha1_update( &ctx->hw_ctx, buf, 64 );

        input += 64;
        ilen -= 64;
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->left_over_bytes + ctx->last, input, ilen );
        ctx->last = ctx->last + ilen;
    }

    return;
}

void wiced_hw_sha1_finish( wiced_hw_sha1_context *ctx, unsigned char output[ SHA1_HASH_SIZE ] )
{
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char outbuf[SHA1_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    memcpy(buf, ctx->left_over_bytes, 64);

    if ( ctx->total < 64 )
    {
        platform_hwcrypto_sha1( buf, ctx->total, outbuf );
    }
    else
    {
        platform_hwcrypto_sha1_finish( &ctx->hw_ctx, buf, ctx->last, outbuf );
    }
    memcpy(output, outbuf, SHA1_HASH_SIZE);

    return;
}

void wiced_sha1_hmac_starts( wiced_hw_sha1_hmac_context * ctx, const unsigned char *key, uint32_t keylen )
{
    uint32_t offset = 0;
    unsigned char *payloadbuf;

    memset( &ctx->ctx.hw_ctx, 0, sizeof( sha1_context_t ) );
    memset( ctx, 0, sizeof( wiced_hw_sha1_hmac_context ) );

    payloadbuf = ctx->ctx.payload_buffer;

    if( ( ( (unsigned long)payloadbuf) & (HWCRYPTO_ALIGNMENT_BYTES - 1) ) != 0)
    {
        offset = HWCRYPTO_ALIGNMENT_BYTES - ( ((unsigned long)payloadbuf) & (HWCRYPTO_ALIGNMENT_BYTES - 1));
    }

    ctx->ctx.hw_ctx.payload_buffer = payloadbuf + offset;

    platform_hwcrypto_sha1_hmac_starts( &ctx->ctx.hw_ctx, key, keylen );

    return;
}

void wiced_sha1_hmac_update( wiced_hw_sha1_hmac_context * ctx, const unsigned char *input, uint32_t ilen )
{
    uint32_t fill = 0;
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    fill = 64 - ctx->ctx.last;
    ctx->ctx.total += ilen;

    if ( ctx->ctx.total >= 64)
    {
        /* first 64-bytes available, we can call starts crypto
           API */
        if ( ctx->ctx.last > 0)
        {
            /* first copy the previously stored bytes  to
               aligned buf and then from input buffer*/
            memcpy(buf, ctx->ctx.left_over_bytes, ctx->ctx.last);
        }
        memcpy(buf + ctx->ctx.last, input, fill);

        platform_hwcrypto_sha1_hmac_update( &ctx->ctx.hw_ctx, buf, 64 );

        input += fill;
        ilen  -= fill;

        ctx->ctx.last = 0;
    }

    while ( ilen > 64 )
    {
        memcpy(buf, input, 64);
        platform_hwcrypto_sha1_hmac_update( &ctx->ctx.hw_ctx, buf, 64 );

        input += 64;
        ilen -= 64;
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->ctx.left_over_bytes + ctx->ctx.last, input, ilen );
        ctx->ctx.last = ctx->ctx.last + ilen;
    }

    return;
}

void wiced_sha1_hmac_finish( wiced_hw_sha1_hmac_context * ctx, unsigned char output[ SHA1_HASH_SIZE ] )
{
    unsigned char buf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char outbuf[SHA1_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    memcpy(buf, ctx->ctx.left_over_bytes, 64);
    platform_hwcrypto_sha1_hmac_finish( &ctx->ctx.hw_ctx, buf, (int32_t) ctx->ctx.last, outbuf );
    memcpy( output, outbuf, SHA1_HASH_SIZE);

    return;
}

void wiced_sha1_hmac( const unsigned char *key, int32_t keylen, const unsigned char *input, int32_t ilen, unsigned char output[ SHA1_HASH_SIZE ] )
{
    wiced_hw_sha1_hmac_context ctx;

    wiced_sha1_hmac_starts( &ctx, key, (uint32_t) keylen);
    wiced_sha1_hmac_update( &ctx, input, (uint32_t) ilen );
    wiced_sha1_hmac_finish( &ctx, output );

    return;
}

void wiced_hw_sha1_process( wiced_hw_sha1_context *ctx, const unsigned char data[ 64 ] )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( data );

    return;
}

void wiced_hw_sha1( const unsigned char *input, size_t ilen,
        unsigned char output[20] )
{
    // platform_hwcrypto_sha1( input, ilen, output );
    wiced_hw_sha1_context ctx;

    wiced_hw_sha1_init( &ctx );
    wiced_hw_sha1_starts( &ctx );
    wiced_hw_sha1_update( &ctx, input, ilen );
    wiced_hw_sha1_finish( &ctx, output );
    wiced_hw_sha1_free( &ctx );
    return;
}
#endif /* #if defined(MBEDTLS_SHA256_ALT) */
/********************SHA1 END*************************/
