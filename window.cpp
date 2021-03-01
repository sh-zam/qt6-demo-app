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
#include <QPainter>
#include <QGuiApplication>
#include <QCommandLineParser>
#include <QWindow>
#include <QPlatformSurfaceEvent>
#include <QElapsedTimer>
#include <QTimer>
#include <QLoggingCategory>
#include <QColorSpace>

#include "window.h"

QShader getShader(const QString &name)
{
    QFile f(name);
    if (f.open(QIODevice::ReadOnly))
        return QShader::fromSerialized(f.readAll());

    return QShader();
}

GraphicsApi graphicsApi;
QRhi::Flags rhiFlags = QRhi::EnableDebugMarkers;
int sampleCount = 1;
QRhiSwapChain::Flags scFlags;
QRhi::BeginFrameFlags beginFrameFlags;
QRhi::EndFrameFlags endFrameFlags;
int framesUntilTdr = -1;
bool transparentBackground = false;
bool debugLayer = true;

/*
 * Copyright (c) 2015-2016 The Khronos Group Inc.
 * Copyright (c) 2015-2016 Valve Corporation
 * Copyright (c) 2015-2016 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Chia-I Wu <olv@lunarg.com>
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Ian Elliott <ian@LunarG.com>
 * Author: Ian Elliott <ianelliott@google.com>
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Gwan-gyeong Mun <elongbug@gmail.com>
 * Author: Tony Barbour <tony@LunarG.com>
 * Author: Bill Hollings <bill.hollings@brenwill.com>
 */

// clang-format off
static const float cube[] = {
    -1.0f,-1.0f,-1.0f,  // -X side
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,

    -1.0f,-1.0f,-1.0f,  // -Z side
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,

    -1.0f,-1.0f,-1.0f,  // -Y side
     1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,

    -1.0f, 1.0f,-1.0f,  // +Y side
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,

     1.0f, 1.0f,-1.0f,  // +X side
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,

    -1.0f, 1.0f, 1.0f,  // +Z side
    -1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,

    0.0f, 1.0f,  // -X side
    1.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,

    1.0f, 1.0f,  // -Z side
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,  // -Y side
    1.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,  // +Y side
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,

    1.0f, 0.0f,  // +X side
    0.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,

    0.0f, 0.0f,  // +Z side
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
};
// clang-format on

Window::Window()
{
    // Tell the platform plugin what we want.
    switch (graphicsApi) {
    case OpenGL:
#if QT_CONFIG(opengl)
        setSurfaceType(OpenGLSurface);
        setFormat(QRhiGles2InitParams::adjustedFormat());
#endif
        break;
    case Vulkan:
        setSurfaceType(VulkanSurface);
        break;
    case D3D11:
        setSurfaceType(Direct3DSurface);
        break;
    case Metal:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
        setSurfaceType(MetalSurface);
#endif
        break;
    default:
        break;
    }

    m_clearColor = transparentBackground ? Qt::transparent : QColor::fromRgbF(0.4f, 0.7f, 0.0f, 1.0f);
}

Window::~Window()
{
    releaseResources();
}

void Window::exposeEvent(QExposeEvent *)
{
    // initialize and start rendering when the window becomes usable for graphics purposes
    if (isExposed() && !m_running) {
        m_running = true;
        init();
        resizeSwapChain();
    }

    const QSize surfaceSize = m_hasSwapChain ? m_sc->surfacePixelSize() : QSize();

    // stop pushing frames when not exposed (or size is 0)
    if ((!isExposed() || (m_hasSwapChain && surfaceSize.isEmpty())) && m_running)
        m_notExposed = true;

    // continue when exposed again and the surface has a valid size.
    // note that the surface size can be (0, 0) even though size() reports a valid one...
    if (isExposed() && m_running && m_notExposed && !surfaceSize.isEmpty()) {
        m_notExposed = false;
        m_newlyExposed = true;
    }

    // always render a frame on exposeEvent() (when exposed) in order to update
    // immediately on window resize.
    if (isExposed() && !surfaceSize.isEmpty())
        render();
}

bool Window::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::UpdateRequest:
        render();
        break;

    case QEvent::PlatformSurface:
        // this is the proper time to tear down the swapchain (while the native window and surface are still around)
        if (static_cast<QPlatformSurfaceEvent *>(e)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)
            releaseSwapChain();
        break;

    default:
        break;
    }

    return QWindow::event(e);
}

void Window::init()
{
    if (graphicsApi == Null) {
        QRhiNullInitParams params;
        m_r = QRhi::create(QRhi::Null, &params, rhiFlags);
    }

#ifndef QT_NO_OPENGL
    if (graphicsApi == OpenGL) {
        m_fallbackSurface = QRhiGles2InitParams::newFallbackSurface();
        QRhiGles2InitParams params;
        params.fallbackSurface = m_fallbackSurface;
        params.window = this;
        m_r = QRhi::create(QRhi::OpenGLES2, &params, rhiFlags);
    }
#endif

#if QT_CONFIG(vulkan) && 0
    if (graphicsApi == Vulkan) {
        QRhiVulkanInitParams params;
        params.inst = vulkanInstance();
        params.window = this;
        m_r = QRhi::create(QRhi::Vulkan, &params, rhiFlags);
    }
#endif

#ifdef Q_OS_WIN
    if (graphicsApi == D3D11) {
        QRhiD3D11InitParams params;
        if (debugLayer)
            qDebug("Enabling D3D11 debug layer");
        params.enableDebugLayer = debugLayer;
        if (framesUntilTdr > 0) {
            params.framesUntilKillingDeviceViaTdr = framesUntilTdr;
            params.repeatDeviceKill = true;
        }
        m_r = QRhi::create(QRhi::D3D11, &params, rhiFlags);
    }
#endif

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
    if (graphicsApi == Metal) {
        QRhiMetalInitParams params;
        m_r = QRhi::create(QRhi::Metal, &params, rhiFlags);
    }
#endif

    if (!m_r)
        qFatal("Failed to create RHI backend");

    // now onto the backend-independent init

    m_sc = m_r->newSwapChain();
    // allow depth-stencil, although we do not actually enable depth test/write for the triangle
    m_ds = m_r->newRenderBuffer(QRhiRenderBuffer::DepthStencil,
                                QSize(), // no need to set the size here, due to UsedWithSwapChainOnly
                                sampleCount,
                                QRhiRenderBuffer::UsedWithSwapChainOnly);
    m_sc->setWindow(this);
    m_sc->setDepthStencil(m_ds);
    m_sc->setSampleCount(sampleCount);
    m_sc->setFlags(scFlags);
    m_rp = m_sc->newCompatibleRenderPassDescriptor();
    m_sc->setRenderPassDescriptor(m_rp);

    customInit();
}

void Window::releaseResources()
{
    customRelease();

    delete m_rp;
    m_rp = nullptr;

    delete m_ds;
    m_ds = nullptr;

    delete m_sc;
    m_sc = nullptr;

    delete m_r;
    m_r = nullptr;

#ifndef QT_NO_OPENGL
    delete m_fallbackSurface;
    m_fallbackSurface = nullptr;
#endif
}

void Window::resizeSwapChain()
{
    m_hasSwapChain = m_sc->createOrResize(); // also handles m_ds

    m_frameCount = 0;
    m_timer.restart();

    const QSize outputSize = m_sc->currentPixelSize();
    m_proj = m_r->clipSpaceCorrMatrix();
    m_proj.perspective(45.0f, outputSize.width() / (float) outputSize.height(), 0.01f, 1000.0f);
    m_proj.translate(0, 0, -4);
}

void Window::releaseSwapChain()
{
    if (m_hasSwapChain) {
        m_hasSwapChain = false;
        m_sc->destroy();
    }
}

void Window::render()
{
    if (!m_hasSwapChain || m_notExposed)
        return;

    // If the window got resized or got newly exposed, resize the swapchain.
    // (the newly-exposed case is not actually required by some
    // platforms/backends, but f.ex. Vulkan on Windows seems to need it)
    if (m_sc->currentPixelSize() != m_sc->surfacePixelSize() || m_newlyExposed) {
        resizeSwapChain();
        if (!m_hasSwapChain)
            return;
        m_newlyExposed = false;
    }

    // Start a new frame. This is where we block when too far ahead of
    // GPU/present, and that's what throttles the thread to the refresh rate.
    // (except for OpenGL where it happens either in endFrame or somewhere else
    // depending on the GL implementation)
    QRhi::FrameOpResult r = m_r->beginFrame(m_sc, beginFrameFlags);
    if (r == QRhi::FrameOpSwapChainOutOfDate) {
        resizeSwapChain();
        if (!m_hasSwapChain)
            return;
        r = m_r->beginFrame(m_sc);
    }
    if (r != QRhi::FrameOpSuccess) {
        requestUpdate();
        return;
    }

    m_frameCount += 1;
    if (m_timer.elapsed() > 1000) {
        if (rhiFlags.testFlag(QRhi::EnableProfiling)) {
            const QRhiProfiler::CpuTime ff = m_r->profiler()->frameToFrameTimes(m_sc);
            const QRhiProfiler::CpuTime be = m_r->profiler()->frameBuildTimes(m_sc);
            const QRhiProfiler::GpuTime gp = m_r->profiler()->gpuFrameTimes(m_sc);
            if (m_r->isFeatureSupported(QRhi::Timestamps)) {
                qDebug("ca. %d fps. "
                       "frame-to-frame: min %lld max %lld avg %f. "
                       "frame build: min %lld max %lld avg %f. "
                       "gpu frame time: min %f max %f avg %f",
                       m_frameCount,
                       ff.minTime, ff.maxTime, ff.avgTime,
                       be.minTime, be.maxTime, be.avgTime,
                       gp.minTime, gp.maxTime, gp.avgTime);
            } else {
                qDebug("ca. %d fps. "
                       "frame-to-frame: min %lld max %lld avg %f. "
                       "frame build: min %lld max %lld avg %f. ",
                       m_frameCount,
                       ff.minTime, ff.maxTime, ff.avgTime,
                       be.minTime, be.maxTime, be.avgTime);
            }
        } else {
            qDebug("ca. %d fps", m_frameCount);
        }

        m_timer.restart();
        m_frameCount = 0;
    }

    customRender();

    m_r->endFrame(m_sc, endFrameFlags);

    if (!scFlags.testFlag(QRhiSwapChain::NoVSync))
        requestUpdate();
    else // try prevent all delays when NoVSync
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
}

struct {
    QRhiBuffer *vbuf = nullptr;
    QRhiBuffer *ubuf = nullptr;
    QRhiTexture *tex = nullptr;
    QRhiSampler *sampler = nullptr;
    QRhiShaderResourceBindings *srb = nullptr;
    QRhiGraphicsPipeline *ps = nullptr;
    QList<QRhiResource *> releasePool;

    float rotation = 0;
    QRhiResourceUpdateBatch *initialUpdates = nullptr;
    int frameCount = 0;
    QImage customImage;
    QRhiTexture *newTex = nullptr;
    QRhiTexture *importedTex = nullptr;
    int testStage = 0;

    QRhiShaderResourceBinding bindings[2];
} d;

void Window::customInit()
{
    d.vbuf = m_r->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(cube));
    d.releasePool << d.vbuf;
    d.vbuf->create();

    d.ubuf = m_r->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 68);
    d.releasePool << d.ubuf;
    d.ubuf->create();

    QImage baseImage(QLatin1String(":/qt256.png"));
    d.tex = m_r->newTexture(QRhiTexture::RGBA8, baseImage.size(), 1, QRhiTexture::UsedAsTransferSource);
    d.releasePool << d.tex;
    d.tex->create();

    // As an alternative to what some of the other examples do, prepare an
    // update batch right here instead of relying on vbufReady and similar flags.
    d.initialUpdates = m_r->nextResourceUpdateBatch();
    d.initialUpdates->uploadStaticBuffer(d.vbuf, cube);
    qint32 flip = 0;
    d.initialUpdates->updateDynamicBuffer(d.ubuf, 64, 4, &flip);
    d.initialUpdates->uploadTexture(d.tex, baseImage);

    d.sampler = m_r->newSampler(QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None,
                                QRhiSampler::ClampToEdge, QRhiSampler::ClampToEdge);
    d.releasePool << d.sampler;
    d.sampler->create();

    d.srb = m_r->newShaderResourceBindings();
    d.releasePool << d.srb;

    d.bindings[0] = QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage, d.ubuf);
    d.bindings[1] = QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::FragmentStage, d.tex, d.sampler);
    d.srb->setBindings(d.bindings, d.bindings + 2);
    d.srb->create();

    d.ps = m_r->newGraphicsPipeline();
    d.releasePool << d.ps;

    d.ps->setDepthTest(true);
    d.ps->setDepthWrite(true);
    d.ps->setDepthOp(QRhiGraphicsPipeline::Less);

    d.ps->setCullMode(QRhiGraphicsPipeline::Back);
    d.ps->setFrontFace(QRhiGraphicsPipeline::CCW);

    const QShader vs = getShader(QLatin1String(":/texture.vert.qsb"));
    if (!vs.isValid())
        qFatal("Failed to load shader pack (vertex)");
    const QShader fs = getShader(QLatin1String(":/texture.frag.qsb"));
    if (!fs.isValid())
        qFatal("Failed to load shader pack (fragment)");

    d.ps->setShaderStages({
        { QRhiShaderStage::Vertex, vs },
        { QRhiShaderStage::Fragment, fs }
    });

    QRhiVertexInputLayout inputLayout;
    inputLayout.setBindings({
        { 3 * sizeof(float) },
        { 2 * sizeof(float) }
    });
    inputLayout.setAttributes({
        { 0, 0, QRhiVertexInputAttribute::Float3, 0 },
        { 1, 1, QRhiVertexInputAttribute::Float2, 0 }
    });

    d.ps->setVertexInputLayout(inputLayout);
    d.ps->setShaderResourceBindings(d.srb);
    d.ps->setRenderPassDescriptor(m_rp);

    d.ps->create();

    d.customImage = QImage(128, 64, QImage::Format_RGBA8888);
    d.customImage.fill(Qt::red);
    QPainter painter(&d.customImage);
    // the text may look different on different platforms, so no guarantee the
    // output on the screen will be identical everywhere
    painter.drawText(5, 25, "Hello world");
    painter.end();
}

void Window::customRelease()
{
    qDeleteAll(d.releasePool);
    d.releasePool.clear();
}

void Window::customRender()
{
    QRhiResourceUpdateBatch *u = m_r->nextResourceUpdateBatch();

    // take the initial set of updates, if this is the first frame
    if (d.initialUpdates) {
        u->merge(d.initialUpdates);
        d.initialUpdates->release();
        d.initialUpdates = nullptr;
    }

    d.rotation += 1.0f;
    QMatrix4x4 mvp = m_proj;
    mvp.scale(0.5f);
    mvp.rotate(d.rotation, 0, 1, 0);
    u->updateDynamicBuffer(d.ubuf, 0, 64, mvp.constData());

    if (d.frameCount > 0 && (d.frameCount % 100) == 0) {
        d.testStage += 1;
        qDebug("testStage = %d", d.testStage);

        // Partially change the texture.
        if (d.testStage == 1) {
            QRhiTextureSubresourceUploadDescription mipDesc(d.customImage);
            // The image here is smaller than the original. Use a non-zero position
            // to make it more interesting.
            mipDesc.setDestinationTopLeft(QPoint(100, 20));
            QRhiTextureUploadDescription desc({ 0, 0, mipDesc });
            u->uploadTexture(d.tex, desc);
        }

        // Exercise image copying.
        if (d.testStage == 2) {
            const QSize sz = d.tex->pixelSize();
            d.newTex = m_r->newTexture(QRhiTexture::RGBA8, sz);
            d.releasePool << d.newTex;
            d.newTex->create();

            QImage empty(sz.width(), sz.height(), QImage::Format_RGBA8888);
            empty.fill(Qt::blue);
            u->uploadTexture(d.newTex, empty);

            // Copy the left-half of tex to the right-half of newTex, while
            // leaving the left-half of newTex blue. Keep a 20 pixel gap at
            // the top.
            QRhiTextureCopyDescription desc;
            desc.setSourceTopLeft(QPoint(0, 20));
            desc.setPixelSize(QSize(sz.width() / 2, sz.height() - 20));
            desc.setDestinationTopLeft(QPoint(sz.width() / 2, 20));
            u->copyTexture(d.newTex, d.tex, desc);

            // Now replace d.tex with d.newTex as the shader resource.
            d.bindings[1] = QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::FragmentStage, d.newTex, d.sampler);
            d.srb->setBindings(d.bindings, d.bindings + 2);
            // "rebuild", whatever that means for a given backend. This srb is
            // already live as the ps in the setGraphicsPipeline references it,
            // but that's fine. Changes will be picked up automatically.
            d.srb->create();
        }

        // Exercise simple, full texture copy.
        if (d.testStage == 4)
            u->copyTexture(d.newTex, d.tex);

        // Now again upload customImage but this time only a part of it.
        if (d.testStage == 5) {
            QRhiTextureSubresourceUploadDescription mipDesc(d.customImage);
            mipDesc.setDestinationTopLeft(QPoint(10, 120));
            mipDesc.setSourceSize(QSize(50, 40));
            mipDesc.setSourceTopLeft(QPoint(20, 10));
            QRhiTextureUploadDescription desc({ 0, 0, mipDesc });
            u->uploadTexture(d.newTex, desc);
        }

        // Exercise texture object export/import.
        if (d.testStage == 6) {
            const QRhiTexture::NativeTexture nativeTexture = d.tex->nativeTexture();
            if (nativeTexture.object) {
#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
                if (graphicsApi == Metal) {
                    qDebug() << "Metal texture: " << nativeTexture.object;
                    // Now could cast to id<MTLTexture> and do something with
                    // it, keeping in mind that copy operations are only done
                    // in beginPass, while rendering into a texture may only
                    // have proper results in current_frame + 2, or after a
                    // finish(). The QRhiTexture still owns the native object.
                }
#endif
                // omit for other backends, the idea is the same

                d.importedTex = m_r->newTexture(QRhiTexture::RGBA8, d.tex->pixelSize());
                d.releasePool << d.importedTex;
                if (!d.importedTex->createFrom(nativeTexture))
                    qWarning("Texture import failed");

                // now d.tex and d.importedTex use the same MTLTexture
                // underneath (owned by d.tex)

                // switch to showing d.importedTex
                d.bindings[1] = QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::FragmentStage, d.importedTex, d.sampler);
                d.srb->setBindings(d.bindings, d.bindings + 2);
                d.srb->create();
            } else {
                qWarning("Accessing native texture object is not supported");
            }
        }

        // Exercise uploading uncompressed data without a QImage.
        if (d.testStage == 7) {
            d.bindings[1] = QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::FragmentStage, d.newTex, d.sampler);
            d.srb->setBindings(d.bindings, d.bindings + 2);
            d.srb->create();

            const QSize sz(221, 139);
            QByteArray data;
            data.resize(sz.width() * sz.height() * 4);
            for (int y = 0; y < sz.height(); ++y) {
                uchar *p = reinterpret_cast<uchar *>(data.data()) + y * sz.width() * 4;
                for (int x = 0; x < sz.width(); ++x) {
                    *p++ = 0; *p++ = 0; *p++ = y * (255 / sz.height());
                    *p++ = 255;
                }
            }
            QRhiTextureSubresourceUploadDescription mipDesc(data.constData(), data.size());
            mipDesc.setSourceSize(sz);
            mipDesc.setDestinationTopLeft(QPoint(5, 25));
            QRhiTextureUploadDescription desc({ 0, 0, mipDesc });
            u->uploadTexture(d.newTex, desc);
        }
    }

    QRhiCommandBuffer *cb = m_sc->currentFrameCommandBuffer();
    const QSize outputSizeInPixels = m_sc->currentPixelSize();

    cb->beginPass(m_sc->currentFrameRenderTarget(), m_clearColor, { 1.0f, 0 }, u);

    cb->setGraphicsPipeline(d.ps);
    cb->setViewport({ 0, 0, float(outputSizeInPixels.width()), float(outputSizeInPixels.height()) });
    cb->setShaderResources();
    const QRhiCommandBuffer::VertexInput vbufBindings[] = {
        { d.vbuf, 0 },
        { d.vbuf, quint32(36 * 3 * sizeof(float)) }
    };
    cb->setVertexInput(0, 2, vbufBindings);
    cb->draw(36);

    cb->endPass();

    d.frameCount += 1;
}

