/* ====================================================================
 * Copyright (c) 2014 - 2019 The GmSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the GmSSL Project.
 *    (http://gmssl.org/)"
 *
 * 4. The name "GmSSL Project" must not be used to endorse or promote
 *    products derived from this software without prior written
 *    permission. For written permission, please contact
 *    guanzhi1980@gmail.com.
 *
 * 5. Products derived from this software may not be called "GmSSL"
 *    nor may "GmSSL" appear in their names without prior written
 *    permission of the GmSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the GmSSL Project
 *    (http://gmssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE GmSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE GmSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */

#include <openssl/sms4.h>
#include <openssl/modes.h>


void sms4_ede_set_encrypt_key(sms4_ede_key_t *key,
	const unsigned char user_key[48])
{
	sms4_set_encrypt_key(&key->k1, user_key);
	sms4_set_decrypt_key(&key->k2, user_key + 16);
	sms4_set_encrypt_key(&key->k3, user_key + 32);
}

void sms4_ede_set_decrypt_key(sms4_ede_key_t *key,
	const unsigned char user_key[48])
{
	sms4_set_decrypt_key(&key->k1, user_key + 32);
	sms4_set_encrypt_key(&key->k2, user_key + 16);
	sms4_set_decrypt_key(&key->k3, user_key);
}

void sms4_ede_encrypt(const unsigned char in[16], unsigned char out[16],
	const sms4_ede_key_t *key)
{
	sms4_encrypt(in, out, &key->k1);
	sms4_encrypt(out, out, &key->k2);
	sms4_encrypt(out, out, &key->k3);
}

void sms4_ede_ecb_encrypt(const unsigned char *in, unsigned char *out,
	const sms4_ede_key_t *key, int enc)
{
	sms4_ede_encrypt(in, out, key);
}

void sms4_ede_cbc_encrypt(const unsigned char *in, unsigned char *out,
	size_t len, const sms4_ede_key_t *key, unsigned char *iv, int enc)
{
	if (enc)
		CRYPTO_cbc128_encrypt(in, out, len, key, iv,
			(block128_f)sms4_ede_encrypt);
	else	CRYPTO_cbc128_decrypt(in, out, len, key, iv,
			(block128_f)sms4_ede_encrypt);
}

void sms4_ede_cfb128_encrypt(const unsigned char *in, unsigned char *out,
	size_t len, const sms4_ede_key_t *key, unsigned char *iv, int *num,
	int enc)
{
	CRYPTO_cfb128_encrypt(in, out, len, key, iv, num, enc,
		(block128_f)sms4_ede_encrypt);
}

void sms4_ede_ofb128_encrypt(const unsigned char *in, unsigned char *out,
	size_t len, const sms4_ede_key_t *key, unsigned char *iv, int *num)
{
	CRYPTO_ofb128_encrypt(in, out, len, key, iv, num,
		(block128_f)sms4_ede_encrypt);
}

void sms4_ede_ctr128_encrypt(const unsigned char *in, unsigned char *out,
	size_t len, const sms4_ede_key_t *key, unsigned char *iv,
	unsigned char ecount_buf[SMS4_BLOCK_SIZE], unsigned int *num)
{
	CRYPTO_ctr128_encrypt(in, out, len, key, iv, ecount_buf, num,
		(block128_f)sms4_ede_encrypt);
}

int sms4_ede_wrap_key(sms4_ede_key_t *key, const unsigned char *iv,
	unsigned char *out, const unsigned char *in, unsigned int inlen)
{
	return CRYPTO_128_wrap(key, iv, out, in, inlen,
		(block128_f)sms4_ede_encrypt);
}

int sms4_ede_unwrap_key(sms4_ede_key_t *key, const unsigned char *iv,
	unsigned char *out, const unsigned char *in, unsigned int inlen)
{
	return CRYPTO_128_unwrap(key, iv, out, in, inlen,
		(block128_f)sms4_ede_encrypt);
}
