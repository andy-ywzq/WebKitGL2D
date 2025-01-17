/*
 * Copyright (C) 2010, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2011 Samsung Electronics
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Logging.h"

#include <wtf/text/CString.h>

#if !LOG_DISABLED

namespace WebKit {

#define DEFINE_LOG_CHANNEL(name) \
    WTFLogChannel JOIN_LOG_CHANNEL_WITH_PREFIX(LOG_CHANNEL_PREFIX, name) = { WTFLogChannelOff, #name };
WEBKIT2_LOG_CHANNELS(DEFINE_LOG_CHANNEL)

#define LOG_CHANNEL_ADDRESS(name)  &JOIN_LOG_CHANNEL_WITH_PREFIX(LOG_CHANNEL_PREFIX, name),
static WTFLogChannel* logChannels[] = {
    WEBKIT2_LOG_CHANNELS(LOG_CHANNEL_ADDRESS)
};

const size_t logChannelCount = WTF_ARRAY_LENGTH(logChannels);

void initializeLogChannelsIfNecessary()
{
    static bool haveInitializedLogChannels = false;
    if (haveInitializedLogChannels)
        return;
    haveInitializedLogChannels = true;

    static bool haveInitializedLoggingChannels = false;
    if (haveInitializedLoggingChannels)
        return;
    haveInitializedLoggingChannels = true;
    
    WTFInitializeLogChannelStatesFromString(logChannels, logChannelCount, logLevelString().utf8().data());
}

#if PLATFORM(GTK) || PLATFORM(EFL) || PLATFORM(NIX)
WTFLogChannel* logChannelByName(const String& name)
{
    return WTFLogChannelByName(logChannels, logChannelCount, name.utf8().data());
}
#endif

#if !PLATFORM(MAC) && !PLATFORM(GTK) && !PLATFORM(EFL) && !PLATFORM(NIX)
String logLevelString()
{
    // FIXME: Each platform will need to define their own logLevelString();
    return emptyString();
}
#endif

} // namespace WebKit

#endif // !LOG_DISABLED
