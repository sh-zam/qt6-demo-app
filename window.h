/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
// Adapted from hellominimalcrossgfxtriangle with the frame rendering stripped out.
// Include this file and implement Window::customInit, release and render.
// Debug/validation layer is enabled for D3D and Vulkan.
#ifndef WINDOW_H
#define WINDOW_H


#include <QGuiApplication>
#include <QWindow>
#include <QtGui/private/qshader_p.h>
#include <QFile>
#include <QtGui/private/qrhiprofiler_p.h>
#include <QtGui/private/qrhinull_p.h>

#ifndef QT_NO_OPENGL
#include <QtGui/private/qrhigles2_p.h>
#include <QOffscreenSurface>
#endif

#if QT_CONFIG(vulkan) && 0
#include <QtGui/private/qrhivulkan_p.h>
#endif

#ifdef Q_OS_WIN
#include <QtGui/private/qrhid3d11_p.h>
#endif

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
#include <QtGui/private/qrhimetal_p.h>
#endif


enum GraphicsApi
{
    Null,
    OpenGL,
    Vulkan,
    D3D11,
    Metal
};

extern GraphicsApi graphicsApi;

extern QRhi::Flags rhiFlags;
extern int sampleCount;
extern QRhiSwapChain::Flags scFlags;
extern QRhi::BeginFrameFlags beginFrameFlags;
extern QRhi::EndFrameFlags endFrameFlags;
extern int framesUntilTdr;
extern bool transparentBackground ;
extern bool debugLayer;



class Window : public QWindow
{
public:
    Window();
    ~Window();

protected:
    void init();
    void releaseResources();
    void resizeSwapChain();
    void releaseSwapChain();
    void render();

    void customInit();
    void customRelease();
    void customRender();

    void exposeEvent(QExposeEvent *) override;
    bool event(QEvent *) override;
#ifdef EXAMPLEFW_KEYPRESS_EVENTS
    void keyPressEvent(QKeyEvent *e) override;
#endif

    bool m_running = false;
    bool m_notExposed = false;
    bool m_newlyExposed = false;

    QRhi *m_r = nullptr;
    bool m_hasSwapChain = false;
    QRhiSwapChain *m_sc = nullptr;
    QRhiRenderBuffer *m_ds = nullptr;
    QRhiRenderPassDescriptor *m_rp = nullptr;

    QMatrix4x4 m_proj;

    QElapsedTimer m_timer;
    int m_frameCount;

#ifndef QT_NO_OPENGL
    QOffscreenSurface *m_fallbackSurface = nullptr;
#endif

    QColor m_clearColor;

    friend int main(int, char**);
};


#endif // WINDOW_H
