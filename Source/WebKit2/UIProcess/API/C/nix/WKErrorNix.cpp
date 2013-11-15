/*
 *  Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WKErrorNix.h"

#include "WKSharedAPICast.h"

#if USE(SOUP)
#include "soup/ResourceError.h"
#include <gio/gio.h>
#elif USE(CURL)
#include "curl/ResourceError.h"
#include <curl/curl.h>
#endif

#if USE(SOUP)
static unsigned soupTlsErrorsToNixErrors(unsigned errors)
{
    unsigned nixErrors = 0;
    if (errors & G_TLS_CERTIFICATE_UNKNOWN_CA)
        nixErrors |= NIXTlsErrorUnkownCA;
    if (errors & G_TLS_CERTIFICATE_BAD_IDENTITY)
        nixErrors |= NIXTlsErrorCertificateBadIdentity;
    if (errors & G_TLS_CERTIFICATE_NOT_ACTIVATED)
        nixErrors |= NIXTlsErrorCertificateNotActivated;
    if (errors & G_TLS_CERTIFICATE_EXPIRED)
        nixErrors |= NIXTlsErrorCertificateExpired;
    if (errors & G_TLS_CERTIFICATE_REVOKED)
        nixErrors |= NIXTlsErrorCertificateRevoked;
    if (errors & G_TLS_CERTIFICATE_INSECURE)
        nixErrors |= NIXTlsErrorCertificateInsecure;
    if (errors & G_TLS_CERTIFICATE_GENERIC_ERROR)
        nixErrors |= NIXTlsErrorCertificateGenericError;
    return nixErrors;
}
#endif

void WKErrorGetTLSErrors(WKErrorRef error, unsigned* tlsErrors)
{
    const WebCore::ResourceError& resourceError = WebKit::toImpl(error)->platformError();
#if USE(SOUP)
    *tlsErrors = soupTlsErrorsToNixErrors(resourceError.tlsErrors());
#elif USE(CURL)
    if (resourceError.errorCode() == CURLE_SSL_CACERT)
        *tlsErrors = NIXTlsErrorUnkownCA | NIXTlsErrorCertificateGenericError;
    else
        *tlsErrors = 0;
#endif
}
