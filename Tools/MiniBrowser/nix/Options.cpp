/*
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Options.h"

#include <cstdio>
#include <cstring>
#include <fstream>

Options::Options()
    : width(0)
    , height(0)
    , viewportHorizontalDisplacement(0)
    , viewportVerticalDisplacement(0)
    , desktopModeEnabled(false)
    , forceTouchEmulationEnabled(false)
    , devicePixelRatio(1.0)
{
}

struct Device {
    enum Type {
        Default,
        N9,
        IPad,
        IPhone,
        Android
    };

    int width;
    int height;
    const char* userAgent;
};

Device deviceList[] = {
    { 1024, 768, "" },
    { 854, 480, "Mozilla/5.0 (MeeGo; NokiaN9) AppleWebKit/534.13 (KHTML, like Gecko) NokiaBrowser/8.5.0 Mobile Safari/534.13" },
    { 1024, 768, "Mozilla/5.0 (iPad; CPU OS 5_0 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9A334 Safari/7534.48.3" },
    { 960, 640, "Mozilla/5.0 (iPhone; CPU iPhone OS 5_0 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9A334 Safari/7534.48.3" },
    { 800, 480, "Mozilla/5.0 (Linux; U; Android 4.0.2; en-us; Galaxy Nexus Build/ICL53F) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30" }
};

bool Options::parse(int argc, char* argv[])
{
    Device::Type device = Device::Default;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--desktop"))
            desktopModeEnabled = true;
        else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--touch-emulation"))
            forceTouchEmulationEnabled = true;
        else if (!strcmp(argv[i], "--window-size")) {
            if (i + 1 == argc) {
                fprintf(stderr, "--window-size requires an argument.\n");
                return false;
            }
            if (sscanf(argv[++i], "%dx%d", &width, &height) != 2) {
                fprintf(stderr, "--window-size format is WIDTHxHEIGHT.\n");
                return false;
            }
        } else if (!strcmp(argv[i], "--viewport-displacement")) {
            if (i + 1 == argc) {
                fprintf(stderr, "--viewport-displacement requires an argument.\n");
                return false;
            }
            if (sscanf(argv[++i], "%dx%d", &viewportHorizontalDisplacement, &viewportVerticalDisplacement) != 2) {
                fprintf(stderr, "--viewport-displacement format is HORIZDISPLACEMENTxVERTDISPLACEMENT.\n");
                return false;
            }
        } else if (!strcmp(argv[i], "--n9"))
            device = Device::N9;
        else if (!strcmp(argv[i], "--ipad"))
            device = Device::IPad;
        else if (!strcmp(argv[i], "--iphone"))
            device = Device::IPhone;
        else if (!strcmp(argv[i], "--android"))
            device = Device::Android;
        else if (!strcmp(argv[i], "--dpr")) {
            if (sscanf(argv[++i], "%f", &devicePixelRatio) != 1) {
                fprintf(stderr, "--dpr format is x.yz.\n");
                return false;
            }
        }
        else
            url = argv[i];
    }

    if (width == 0)
        width = deviceList[device].width;

    if (height == 0)
        height = deviceList[device].height;

    userAgent = deviceList[device].userAgent;

    if (url.empty())
        url = "http://www.google.com";
    else if (url.find("http") != 0 && url.find("file://") != 0) {
        std::ifstream localFile(url.c_str());
        url.insert(0, localFile ? "file://" : "http://");
    }

    return true;
}
