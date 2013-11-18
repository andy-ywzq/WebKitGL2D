/*
 * Copyright (C) 2011 Samsung Electronics
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
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
#include "WebPageProxy.h"

#include "PageClient.h"
#include "NotImplemented.h"
#include "WebPageMessages.h"
#include "WebProcessProxy.h"
#include "WebKitVersion.h"

#include <sys/utsname.h>
#include <wtf/text/StringBuilder.h>

namespace WebKit {

String WebPageProxy::standardUserAgent(const String& applicationName)
{
    WTF::String platform;
    WTF::String version;
    WTF::String osVersion;

#if PLATFORM(X11)
    platform = "X11";
#else
    platform = "Unknown";
#endif

    version = String::number(WEBKIT_MAJOR_VERSION) + '.' + String::number(WEBKIT_MINOR_VERSION);
    struct utsname name;
    if (uname(&name) != -1)
        osVersion = WTF::String(name.sysname) + " " + WTF::String(name.machine);
    else
        osVersion = "Unknown";

    WTF::StringBuilder userAgent;
    userAgent.append("Mozilla/5.0 (");
    userAgent.append(platform);
    userAgent.append("; ");
    userAgent.append(osVersion);
    userAgent.append(") AppleWebKit/");
    userAgent.append(version);
    userAgent.append(" (KHTML, like Gecko)");
    if (!applicationName.isEmpty()) {
        userAgent.append(" ");
        userAgent.append(applicationName);
    }
    userAgent.append(" Safari/");
    userAgent.append(version);

    return userAgent.toString();
}

void WebPageProxy::platformInitialize()
{
}

void WebPageProxy::getEditorCommandsForKeyEvent(Vector<WTF::String>&)
{
    notImplemented();
}

void WebPageProxy::saveRecentSearches(const String&, const Vector<String>&)
{
    notImplemented();
}

void WebPageProxy::loadRecentSearches(const String&, Vector<String>&)
{
    notImplemented();
}

void WebPageProxy::initializeUIPopupMenuClient(const WKPageUIPopupMenuClient* client)
{
    m_uiPopupMenuClient.initialize(client);
}

} // namespace WebKit
