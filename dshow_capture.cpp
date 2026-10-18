#include "dshow_capture.h"

#include <iostream>
#define STRSAFE_NO_DEPRECATE
#include <windows.h>
#include <dshow.h>
#include <qedit.h>
#include <vector>
#include <algorithm>
#include <QElapsedTimer>
#include <QDebug>
#include <QMutex>

static const CLSID CLSID_SampleGrabber_Local = 
    { 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };

static const CLSID CLSID_NullRenderer_Local = 
    { 0xC1F400A4, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };

static const GUID MEDIASUBTYPE_NV12_Local = 
    { 0x3231564E, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };

static int r_table[256];
static int g_table_u[256];
static int g_table_v[256];
static int b_table[256];
static uint8_t clamp_table[1024];

static void initTables()
{
    static bool inited = false;
    if (inited) return;
    for (int i = 0; i < 256; ++i) {
        int d = i - 128;
        r_table[i] = (409 * d + 128) >> 8;
        g_table_u[i] = (100 * d) >> 8;
        g_table_v[i] = (208 * d) >> 8;
        b_table[i] = (516 * d + 128) >> 8;
    }
    for (int i = -512; i < 512; ++i) {
        clamp_table[i + 512] = static_cast<uint8_t>(std::clamp(i, 0, 255));
    }
    inited = true;
}

static QImage fastNv12ToRgb(const uint8_t *nv12, int width, int height)
{
    initTables();
    QImage image(width, height, QImage::Format_RGB32);
    const uint8_t *yPlane = nv12;
    const uint8_t *uvPlane = nv12 + width * height;

    for (int y = 0; y < height; ++y) {
        uint32_t *scanLine = reinterpret_cast<uint32_t*>(image.scanLine(y));
        const uint8_t *yLine = yPlane + y * width;
        const uint8_t *uvLine = uvPlane + (y / 2) * width;

        for (int x = 0; x < width; ++x) {
            int yVal = yLine[x];
            int uvIdx = (x & ~1);
            int uVal = uvLine[uvIdx];
            int vVal = uvLine[uvIdx + 1];

            int r = clamp_table[yVal + r_table[vVal] + 512];
            int g = clamp_table[yVal - g_table_u[uVal] - g_table_v[vVal] + 512];
            int b = clamp_table[yVal + b_table[uVal] + 512];

            scanLine[x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
    }
    return image;
}

class DirectShowCallback : public ISampleGrabberCB
{
public:
    DirectShowCapture *parentCapture = nullptr;
    int width = 1920;
    int height = 1080;
    QElapsedTimer throttleTimer;
    long frameCounter = 0;
    int intervalMs = 15; // Target 60 FPS live streaming (~16ms)
    int targetFps = 60;

    DirectShowCallback() {
        throttleTimer.start();
    }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) override {
        if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) {
            *ppv = static_cast<ISampleGrabberCB*>(this);
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return 2; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }
    STDMETHODIMP SampleCB(double, IMediaSample *) override { return S_OK; }

    STDMETHODIMP BufferCB(double, BYTE *pBuffer, long BufferLen) override {
        if (!parentCapture || !pBuffer) return S_OK;

        // Deliver at up to target FPS (e.g. 60 FPS live video stream)
        if (intervalMs > 0 && throttleTimer.elapsed() < intervalMs) {
            return S_OK;
        }
        throttleTimer.restart();

        if (BufferLen == width * height * 3 / 2) {
            QImage img = fastNv12ToRgb(pBuffer, width, height);
            emit parentCapture->frameReady(img);
        } else if (BufferLen == width * height * 3) {
            QImage img(pBuffer, width, height, width * 3, QImage::Format_BGR888);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
            emit parentCapture->frameReady(img.mirrored(false, true));
#pragma clang diagnostic pop
        }

        if (++frameCounter == 1) {
            emit parentCapture->statusMessage(QStringLiteral("DirectShow: Live 60 FPS video stream active (%1x%2)")
                                              .arg(width).arg(height));
        }
        return S_OK;
    }
};

class DirectShowCaptureWorker
{
public:
    IGraphBuilder *graph = nullptr;
    ICaptureGraphBuilder2 *builder = nullptr;
    IMediaControl *control = nullptr;
    IBaseFilter *srcFilter = nullptr;
    IBaseFilter *grabberFilter = nullptr;
    ISampleGrabber *sampleGrabber = nullptr;
    IBaseFilter *nullRenderer = nullptr;
    DirectShowCallback callback;
    bool isRunning = false;
    bool comInitialized = false;

    ~DirectShowCaptureWorker() {
        cleanup();
    }

    void cleanup() {
        if (control) {
            control->Stop();
            control->Release();
            control = nullptr;
        }
        if (sampleGrabber) {
            sampleGrabber->SetCallback(nullptr, 0);
            sampleGrabber->Release();
            sampleGrabber = nullptr;
        }
        if (grabberFilter) {
            grabberFilter->Release();
            grabberFilter = nullptr;
        }
        if (nullRenderer) {
            nullRenderer->Release();
            nullRenderer = nullptr;
        }
        if (srcFilter) {
            srcFilter->Release();
            srcFilter = nullptr;
        }
        if (builder) {
            builder->Release();
            builder = nullptr;
        }
        if (graph) {
            graph->Release();
            graph = nullptr;
        }
        if (comInitialized) {
            CoUninitialize();
            comInitialized = false;
        }
        isRunning = false;
    }
};

DirectShowCapture::DirectShowCapture(QObject *parent)
    : QObject(parent), m_worker(new DirectShowCaptureWorker())
{
    m_worker->callback.parentCapture = this;
}

DirectShowCapture::~DirectShowCapture()
{
    stop();
    delete m_worker;
}

bool DirectShowCapture::isRunning() const
{
    return m_worker && m_worker->isRunning;
}

void DirectShowCapture::setTargetFps(int fps)
{
    if (m_worker) {
        m_worker->callback.targetFps = fps;
        if (fps >= 60 || fps <= 0) m_worker->callback.intervalMs = 0; // unlimited 60 FPS
        else m_worker->callback.intervalMs = 1000 / fps;
    }
}

int DirectShowCapture::targetFps() const
{
    return m_worker ? m_worker->callback.targetFps : 60;
}

void DirectShowCapture::stop()
{
    if (m_worker) {
        m_worker->cleanup();
    }
}

bool DirectShowCapture::start(const QString &preferredDeviceName)
{
    stop();

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        emit statusMessage(QStringLiteral("DirectShow: CoInitialize failed: %1").arg(hr));
        return false;
    }
    m_worker->comInitialized = true;

    // 1. Create FilterGraph and CaptureGraphBuilder2
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                          IID_IGraphBuilder, (void **)&m_worker->graph);
    if (FAILED(hr)) {
        emit statusMessage(QStringLiteral("DirectShow: FilterGraph creation failed"));
        m_worker->cleanup();
        return false;
    }

    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
                          IID_ICaptureGraphBuilder2, (void **)&m_worker->builder);
    if (FAILED(hr)) {
        emit statusMessage(QStringLiteral("DirectShow: CaptureGraphBuilder2 creation failed"));
        m_worker->cleanup();
        return false;
    }
    m_worker->builder->SetFiltergraph(m_worker->graph);

    // 2. Enumerate DirectShow video capture devices
    ICreateDevEnum *pDevEnum = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                          IID_ICreateDevEnum, (void **)&pDevEnum);
    if (FAILED(hr)) {
        emit statusMessage(QStringLiteral("DirectShow: Device enumerator creation failed"));
        m_worker->cleanup();
        return false;
    }

    IEnumMoniker *pEnum = NULL;
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (hr != S_OK || !pEnum) {
        pDevEnum->Release();
        emit statusMessage(QStringLiteral("DirectShow: No video input devices found"));
        m_worker->cleanup();
        return false;
    }

    IMoniker *pMoniker = NULL;
    QString chosenName;
    IBaseFilter *selectedFilter = NULL;

    while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
        IPropertyBag *pPropBag = NULL;
        if (SUCCEEDED(pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag))) {
            VARIANT var;
            VariantInit(&var);
            if (SUCCEEDED(pPropBag->Read(L"FriendlyName", &var, 0))) {
                QString devName = QString::fromWCharArray(var.bstrVal);
                qInfo() << "DirectShow: Found capture device:" << devName;
                if (selectedFilter == NULL || devName.contains(preferredDeviceName, Qt::CaseInsensitive)) {
                    if (selectedFilter) selectedFilter->Release();
                    pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&selectedFilter);
                    chosenName = devName;
                    VariantClear(&var);
                    pPropBag->Release();
                    pMoniker->Release();
                    if (devName.contains(preferredDeviceName, Qt::CaseInsensitive)) {
                        break;
                    }
                    continue;
                }
                VariantClear(&var);
            }
            pPropBag->Release();
        }
        pMoniker->Release();
    }
    pEnum->Release();
    pDevEnum->Release();

    if (!selectedFilter) {
        emit statusMessage(QStringLiteral("DirectShow: Could not bind to '%1'").arg(preferredDeviceName));
        m_worker->cleanup();
        return false;
    }

    m_worker->srcFilter = selectedFilter;
    m_worker->graph->AddFilter(m_worker->srcFilter, L"Video Capture Source");

    // 3. Create Sample Grabber Filter
    hr = CoCreateInstance(CLSID_SampleGrabber_Local, NULL, CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, (void **)&m_worker->grabberFilter);
    if (FAILED(hr)) {
        emit statusMessage(QStringLiteral("DirectShow: Failed to create SampleGrabber"));
        m_worker->cleanup();
        return false;
    }
    m_worker->graph->AddFilter(m_worker->grabberFilter, L"SampleGrabber");
    m_worker->grabberFilter->QueryInterface(IID_ISampleGrabber, (void **)&m_worker->sampleGrabber);

    AM_MEDIA_TYPE mt;
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_NV12_Local;
    mt.formattype = FORMAT_VideoInfo;
    m_worker->sampleGrabber->SetMediaType(&mt);

    m_worker->callback.width = 1920;
    m_worker->callback.height = 1080;
    m_worker->callback.frameCounter = 0;
    m_worker->sampleGrabber->SetCallback(&m_worker->callback, 1); // 1 = BufferCB

    // 4. Create Null Renderer
    hr = CoCreateInstance(CLSID_NullRenderer_Local, NULL, CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, (void **)&m_worker->nullRenderer);
    if (FAILED(hr)) {
        emit statusMessage(QStringLiteral("DirectShow: Failed to create NullRenderer"));
        m_worker->cleanup();
        return false;
    }
    m_worker->graph->AddFilter(m_worker->nullRenderer, L"NullRenderer");

    // 5. Connect Source -> SampleGrabber -> NullRenderer
    hr = m_worker->builder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                         m_worker->srcFilter, m_worker->grabberFilter,
                                         m_worker->nullRenderer);
    if (FAILED(hr)) {
        // Try without category constraint if PIN_CATEGORY_CAPTURE failed
        hr = m_worker->builder->RenderStream(NULL, &MEDIATYPE_Video,
                                             m_worker->srcFilter, m_worker->grabberFilter,
                                             m_worker->nullRenderer);
    }
    if (FAILED(hr)) {
        emit statusMessage(QStringLiteral("DirectShow: RenderStream failed (hr=0x%1)").arg(QString::number(static_cast<uint>(hr), 16)));
        m_worker->cleanup();
        return false;
    }

    // Inspect connected format
    AM_MEDIA_TYPE connMt;
    ZeroMemory(&connMt, sizeof(AM_MEDIA_TYPE));
    if (SUCCEEDED(m_worker->sampleGrabber->GetConnectedMediaType(&connMt))) {
        if (connMt.formattype == FORMAT_VideoInfo && connMt.cbFormat >= sizeof(VIDEOINFOHEADER)) {
            VIDEOINFOHEADER *vih = reinterpret_cast<VIDEOINFOHEADER *>(connMt.pbFormat);
            m_worker->callback.width = vih->bmiHeader.biWidth;
            m_worker->callback.height = std::abs(vih->bmiHeader.biHeight);
            qInfo() << "DirectShow: Connected capture resolution:" << m_worker->callback.width << "x" << m_worker->callback.height;
        }
        if (connMt.cbFormat != 0) CoTaskMemFree(reinterpret_cast<PVOID>(connMt.pbFormat));
        if (connMt.pUnk != NULL) connMt.pUnk->Release();
    }

    // 6. Run graph
    hr = m_worker->graph->QueryInterface(IID_IMediaControl, (void **)&m_worker->control);
    if (FAILED(hr) || !m_worker->control) {
        emit statusMessage(QStringLiteral("DirectShow: IMediaControl interface missing"));
        m_worker->cleanup();
        return false;
    }

    // CRITICAL: Disable reference clock synchronization for live capture to prevent stream freeze/stall
    IMediaFilter *pMediaFilter = nullptr;
    if (SUCCEEDED(m_worker->graph->QueryInterface(IID_IMediaFilter, (void **)&pMediaFilter)) && pMediaFilter) {
        pMediaFilter->SetSyncSource(NULL);
        pMediaFilter->Release();
    }

    hr = m_worker->control->Run();
    m_worker->isRunning = true;

    emit statusMessage(QStringLiteral("DirectShow Active: Connected to '%1' (%2x%3)")
                       .arg(chosenName)
                       .arg(m_worker->callback.width)
                       .arg(m_worker->callback.height));
    return true;
}
