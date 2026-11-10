#include "eagleeye_classifier.h"

#include <QFile>
#include <QDebug>
#include <QtEndian>
#include <cmath>
#include <algorithm>

EagleEyeClassifier::EagleEyeClassifier()
{
}

bool EagleEyeClassifier::loadWeights(const QString &weightsPath)
{
    QFile file(weightsPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "EagleEyeClassifier: Failed to open weights file:" << weightsPath;
        m_loaded = false;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    const size_t tensorFloatCounts[8] = {144, 16, 4608, 32, 200704, 128, 1280, 10};
    const size_t expectedFloats = 144 + 16 + 4608 + 32 + 200704 + 128 + 1280 + 10; // 206922 floats = 827688 bytes

    // Support PyTorch .pth (ZIP archive containing */data/0 .. */data/7) natively
    if (data.size() >= 22 && data.startsWith("PK\x03\x04")) {
        int eocdPos = data.lastIndexOf(QByteArray("PK\x05\x06", 4));
        if (eocdPos < 0 || eocdPos + 22 > data.size()) {
            qWarning() << "EagleEyeClassifier: Invalid .pth ZIP archive (missing EOCD):" << weightsPath;
            m_loaded = false;
            return false;
        }

        const uchar *raw = reinterpret_cast<const uchar*>(data.constData());
        const quint16 numEntries = qFromLittleEndian<quint16>(raw + eocdPos + 10);
        const quint32 cdOffset = qFromLittleEndian<quint32>(raw + eocdPos + 16);

        if (cdOffset >= static_cast<quint32>(data.size())) {
            qWarning() << "EagleEyeClassifier: Invalid .pth Central Directory offset:" << weightsPath;
            m_loaded = false;
            return false;
        }

        const float *tensorPtrs[8] = {nullptr};
        quint32 pos = cdOffset;

        for (quint16 i = 0; i < numEntries; ++i) {
            if (pos + 46 > static_cast<quint32>(data.size())) break;
            const quint32 sig = qFromLittleEndian<quint32>(raw + pos);
            if (sig != 0x02014b50u) break;

            const quint16 compMethod = qFromLittleEndian<quint16>(raw + pos + 10);
            const quint32 uncompSize = qFromLittleEndian<quint32>(raw + pos + 24);
            const quint16 fnLen = qFromLittleEndian<quint16>(raw + pos + 28);
            const quint16 extraLen = qFromLittleEndian<quint16>(raw + pos + 30);
            const quint16 commentLen = qFromLittleEndian<quint16>(raw + pos + 32);
            const quint32 lfhOffset = qFromLittleEndian<quint32>(raw + pos + 42);

            if (pos + 46 + fnLen > static_cast<quint32>(data.size())) break;
            QByteArray entryName(reinterpret_cast<const char*>(raw + pos + 46), fnLen);
            pos += 46 + fnLen + extraLen + commentLen;

            for (int t = 0; t < 8; ++t) {
                QByteArray suffix = QByteArray("/data/") + QByteArray::number(t);
                if (entryName.endsWith(suffix)) {
                    if (compMethod != 0 || uncompSize != tensorFloatCounts[t] * sizeof(float)) {
                        qWarning() << "EagleEyeClassifier: Tensor" << t << "size/compression mismatch in .pth";
                        m_loaded = false;
                        return false;
                    }
                    if (lfhOffset + 30 > static_cast<quint32>(data.size())) break;
                    const quint32 lfhSig = qFromLittleEndian<quint32>(raw + lfhOffset);
                    if (lfhSig != 0x04034b50u) break;
                    const quint16 lfhFnLen = qFromLittleEndian<quint16>(raw + lfhOffset + 26);
                    const quint16 lfhExtraLen = qFromLittleEndian<quint16>(raw + lfhOffset + 28);
                    const quint32 dataStart = lfhOffset + 30 + lfhFnLen + lfhExtraLen;
                    if (dataStart + uncompSize > static_cast<quint32>(data.size())) break;

                    tensorPtrs[t] = reinterpret_cast<const float*>(raw + dataStart);
                }
            }
        }

        for (int t = 0; t < 8; ++t) {
            if (!tensorPtrs[t]) {
                qWarning() << "EagleEyeClassifier: Missing tensor data/" << t << "in .pth file:" << weightsPath;
                m_loaded = false;
                return false;
            }
        }

        m_conv1_w.assign(tensorPtrs[0], tensorPtrs[0] + 144);
        m_conv1_b.assign(tensorPtrs[1], tensorPtrs[1] + 16);
        m_conv2_w.assign(tensorPtrs[2], tensorPtrs[2] + 4608);
        m_conv2_b.assign(tensorPtrs[3], tensorPtrs[3] + 32);
        m_fc1_w.assign(tensorPtrs[4], tensorPtrs[4] + 200704);
        m_fc1_b.assign(tensorPtrs[5], tensorPtrs[5] + 128);
        m_fc2_w.assign(tensorPtrs[6], tensorPtrs[6] + 1280);
        m_fc2_b.assign(tensorPtrs[7], tensorPtrs[7] + 10);

        m_loaded = true;
        qInfo() << "EagleEyeClassifier: Successfully loaded PyTorch .pth model (" << expectedFloats << "floats) from" << weightsPath;
        return true;
    }
    if (data.size() != (int)(expectedFloats * sizeof(float))) {
        qWarning() << "EagleEyeClassifier: Weights file size mismatch. Expected" 
                   << expectedFloats * sizeof(float) << "bytes, got" << data.size();
        m_loaded = false;
        return false;
    }

    const float *ptr = reinterpret_cast<const float*>(data.constData());

    m_conv1_w.assign(ptr, ptr + 144); ptr += 144;
    m_conv1_b.assign(ptr, ptr + 16);  ptr += 16;
    m_conv2_w.assign(ptr, ptr + 4608); ptr += 4608;
    m_conv2_b.assign(ptr, ptr + 32);  ptr += 32;
    m_fc1_w.assign(ptr, ptr + 200704); ptr += 200704;
    m_fc1_b.assign(ptr, ptr + 128);  ptr += 128;
    m_fc2_w.assign(ptr, ptr + 1280); ptr += 1280;
    m_fc2_b.assign(ptr, ptr + 10);   ptr += 10;

    m_loaded = true;
    qInfo() << "EagleEyeClassifier: Successfully loaded model weights (" << expectedFloats << "floats)";
    return true;
}

std::vector<float> EagleEyeClassifier::forward(const float *input28x28) const
{
    if (!m_loaded) {
        return std::vector<float>(10, 0.0f);
    }

    // 1. Conv1: [1, 28, 28] -> [16, 28, 28], kernel 3x3, pad 1, relu
    float conv1_out[16][28][28];
    for (int oc = 0; oc < 16; ++oc) {
        float b = m_conv1_b[oc];
        const float *w_oc = &m_conv1_w[oc * 9];
        for (int y = 0; y < 28; ++y) {
            for (int x = 0; x < 28; ++x) {
                float sum = b;
                for (int ky = 0; ky < 3; ++ky) {
                    int iy = y + ky - 1;
                    if (iy < 0 || iy >= 28) continue;
                    for (int kx = 0; kx < 3; ++kx) {
                        int ix = x + kx - 1;
                        if (ix < 0 || ix >= 28) continue;
                        sum += w_oc[ky * 3 + kx] * input28x28[iy * 28 + ix];
                    }
                }
                conv1_out[oc][y][x] = std::max(0.0f, sum);
            }
        }
    }

    // 2. MaxPool1: 2x2, stride 2 -> [16, 14, 14]
    float pool1_out[16][14][14];
    for (int oc = 0; oc < 16; ++oc) {
        for (int py = 0; py < 14; ++py) {
            int sy = py * 2;
            for (int px = 0; px < 14; ++px) {
                int sx = px * 2;
                float m = conv1_out[oc][sy][sx];
                m = std::max(m, conv1_out[oc][sy + 1][sx]);
                m = std::max(m, conv1_out[oc][sy][sx + 1]);
                m = std::max(m, conv1_out[oc][sy + 1][sx + 1]);
                pool1_out[oc][py][px] = m;
            }
        }
    }

    // 3. Conv2: [16, 14, 14] -> [32, 14, 14], kernel 3x3, pad 1, relu
    float conv2_out[32][14][14];
    for (int oc = 0; oc < 32; ++oc) {
        float b = m_conv2_b[oc];
        for (int y = 0; y < 14; ++y) {
            for (int x = 0; x < 14; ++x) {
                float sum = b;
                for (int ic = 0; ic < 16; ++ic) {
                    const float *w_filter = &m_conv2_w[(oc * 16 + ic) * 9];
                    for (int ky = 0; ky < 3; ++ky) {
                        int iy = y + ky - 1;
                        if (iy < 0 || iy >= 14) continue;
                        for (int kx = 0; kx < 3; ++kx) {
                            int ix = x + kx - 1;
                            if (ix < 0 || ix >= 14) continue;
                            sum += w_filter[ky * 3 + kx] * pool1_out[ic][iy][ix];
                        }
                    }
                }
                conv2_out[oc][y][x] = std::max(0.0f, sum);
            }
        }
    }

    // 4. MaxPool2: 2x2, stride 2 -> [32, 7, 7]
    float flat[1568];
    for (int oc = 0; oc < 32; ++oc) {
        int base = oc * 49;
        for (int py = 0; py < 7; ++py) {
            int sy = py * 2;
            for (int px = 0; px < 7; ++px) {
                int sx = px * 2;
                float m = conv2_out[oc][sy][sx];
                m = std::max(m, conv2_out[oc][sy + 1][sx]);
                m = std::max(m, conv2_out[oc][sy][sx + 1]);
                m = std::max(m, conv2_out[oc][sy + 1][sx + 1]);
                flat[base + py * 7 + px] = m;
            }
        }
    }

    // 5. FC1: 1568 -> 128, relu
    float fc1_out[128];
    for (int i = 0; i < 128; ++i) {
        float sum = m_fc1_b[i];
        const float *w_row = &m_fc1_w[i * 1568];
        for (int j = 0; j < 1568; ++j) {
            sum += w_row[j] * flat[j];
        }
        fc1_out[i] = std::max(0.0f, sum);
    }

    // 6. FC2: 128 -> 10
    std::vector<float> fc2_out(10);
    for (int i = 0; i < 10; ++i) {
        float sum = m_fc2_b[i];
        const float *w_row = &m_fc2_w[i * 128];
        for (int j = 0; j < 128; ++j) {
            sum += w_row[j] * fc1_out[j];
        }
        fc2_out[i] = sum;
    }

    return fc2_out;
}

int EagleEyeClassifier::predictDigit(const QImage &img, float *confidence) const
{
    if (img.isNull() || !m_loaded) {
        if (confidence) *confidence = 0.0f;
        return 0;
    }

    // 1. Convert to 8-bit Grayscale using exact Rec.601 weights (matching PIL / PyTorch transforms.Grayscale):
    // L = 0.299 * R + 0.587 * G + 0.114 * B
    QImage grayImg;
    if (img.format() == QImage::Format_Grayscale8) {
        grayImg = img;
    } else {
        QImage rgb = img.convertToFormat(QImage::Format_RGB32);
        int w = rgb.width();
        int h = rgb.height();
        grayImg = QImage(w, h, QImage::Format_Grayscale8);
        for (int y = 0; y < h; ++y) {
            const QRgb *src = reinterpret_cast<const QRgb*>(rgb.constScanLine(y));
            uchar *dst = grayImg.scanLine(y);
            for (int x = 0; x < w; ++x) {
                QRgb p = src[x];
                dst[x] = static_cast<uchar>((19595 * qRed(p) + 38470 * qGreen(p) + 7471 * qBlue(p) + 32768) >> 16);
            }
        }
    }

    // 2. Resize to 28x28 grayscale (matching transforms.Resize((28, 28)))
    // 2. Resize to 28x28 grayscale using exact PIL Image.resize((28, 28), Image.BILINEAR) 2-pass tent filter
    const int inW = grayImg.width();
    const int inH = grayImg.height();
    QImage scaled(28, 28, QImage::Format_Grayscale8);
    if (inW == 28 && inH == 28) {
        scaled = grayImg;
    } else {
        std::vector<uchar> horiz(inH * 28, 0);
        // Horizontal pass: inW -> 28
        {
            const double scale = static_cast<double>(inW) / 28.0;
            const double filterscale = std::max(1.0, scale);
            const double support = 1.0 * filterscale;
            const int ksize = static_cast<int>(std::ceil(support)) * 2 + 1;
            for (int xx = 0; xx < 28; ++xx) {
                const double center = (xx + 0.5) * scale;
                const int xmin = std::max(0, static_cast<int>(center - support + 0.5));
                const int xmax = std::min(inW, xmin + ksize);
                std::vector<double> w(xmax - xmin);
                double wsum = 0.0;
                for (int x = xmin; x < xmax; ++x) {
                    double d = (x - center + 0.5) / filterscale;
                    double v = std::max(0.0, 1.0 - std::abs(d));
                    w[x - xmin] = v;
                    wsum += v;
                }
                std::vector<qint64> wf(xmax - xmin);
                for (int i = 0; i < xmax - xmin; ++i) {
                    double wn = (wsum > 0.0) ? (w[i] / wsum) : 0.0;
                    wf[i] = static_cast<qint64>(std::floor(wn * (1 << 22) + 0.5));
                }
                for (int y = 0; y < inH; ++y) {
                    const uchar *srcLine = grayImg.constScanLine(y);
                    qint64 acc = 0;
                    for (int x = xmin; x < xmax; ++x) {
                        acc += static_cast<qint64>(srcLine[x]) * wf[x - xmin];
                    }
                    int val = static_cast<int>((acc + (1 << 21)) >> 22);
                    horiz[y * 28 + xx] = static_cast<uchar>(std::clamp(val, 0, 255));
                }
            }
        }
        // Vertical pass: inH -> 28
        {
            const double scale = static_cast<double>(inH) / 28.0;
            const double filterscale = std::max(1.0, scale);
            const double support = 1.0 * filterscale;
            const int ksize = static_cast<int>(std::ceil(support)) * 2 + 1;
            for (int yy = 0; yy < 28; ++yy) {
                const double center = (yy + 0.5) * scale;
                const int ymin = std::max(0, static_cast<int>(center - support + 0.5));
                const int ymax = std::min(inH, ymin + ksize);
                std::vector<double> w(ymax - ymin);
                double wsum = 0.0;
                for (int y = ymin; y < ymax; ++y) {
                    double d = (y - center + 0.5) / filterscale;
                    double v = std::max(0.0, 1.0 - std::abs(d));
                    w[y - ymin] = v;
                    wsum += v;
                }
                std::vector<qint64> wf(ymax - ymin);
                for (int i = 0; i < ymax - ymin; ++i) {
                    double wn = (wsum > 0.0) ? (w[i] / wsum) : 0.0;
                    wf[i] = static_cast<qint64>(std::floor(wn * (1 << 22) + 0.5));
                }
                uchar *dstLine = scaled.scanLine(yy);
                for (int xx = 0; xx < 28; ++xx) {
                    qint64 acc = 0;
                    for (int y = ymin; y < ymax; ++y) {
                        acc += static_cast<qint64>(horiz[y * 28 + xx]) * wf[y - ymin];
                    }
                    int val = static_cast<int>((acc + (1 << 21)) >> 22);
                    dstLine[xx] = static_cast<uchar>(std::clamp(val, 0, 255));
                }
            }
        }
    }

    // Normalize: (x/255.0 - 0.5) / 0.5
    float input[28 * 28];
    for (int y = 0; y < 28; ++y) {
        const uchar *line = scaled.constScanLine(y);
        for (int x = 0; x < 28; ++x) {
            float val = static_cast<float>(line[x]) / 255.0f;
            input[y * 28 + x] = (val - 0.5f) / 0.5f;
        }
    }

    std::vector<float> logits = forward(input);

    int bestDigit = 0;
    float maxLogit = logits[0];
    for (int i = 1; i < 10; ++i) {
        if (logits[i] > maxLogit) {
            maxLogit = logits[i];
            bestDigit = i;
        }
    }

    if (confidence) {
        // Softmax computation
        float sumExp = 0.0f;
        for (int i = 0; i < 10; ++i) {
            sumExp += std::exp(logits[i] - maxLogit);
        }
        *confidence = 1.0f / sumExp;
    }

    return bestDigit;
}

QString EagleEyeClassifier::predictRound(const QImage &roiImg, int *roundNumber, float *avgConf, DigitMode mode) const
{
    if (roiImg.isNull() || !m_loaded) {
        if (roundNumber) *roundNumber = 0;
        if (avgConf) *avgConf = 0.0f;
        return QString();
    }

    // If Auto mode, resolve to 1-digit or 2-digit mode based on ROI aspect ratio if user cropped a smaller box
    if (mode == DigitMode::Auto) {
        double aspect = static_cast<double>(roiImg.width()) / std::max(1, roiImg.height());
        if (aspect < 0.65) {
            mode = DigitMode::OneDigit;
        } else if (aspect < 1.15) {
            mode = DigitMode::TwoDigits;
        }
    }

    // 1-Digit Manual Mode: The entire ROI represents a single digit
    if (mode == DigitMode::OneDigit) {
        QImage rgbImg = roiImg.convertToFormat(QImage::Format_RGB32);
        int w = rgbImg.width();
        int h = rgbImg.height();
        int redCount = 0;
        for (int y = 0; y < h; ++y) {
            const QRgb *line = reinterpret_cast<const QRgb*>(rgbImg.constScanLine(y));
            for (int x = 0; x < w; ++x) {
                QRgb p = line[x];
                int r = qRed(p);
                int g = qGreen(p);
                int b = qBlue(p);
                if (r > 75 && r > static_cast<int>(g * 1.3f) && r > static_cast<int>(b * 1.3f)) {
                    redCount++;
                }
            }
        }
        if (redCount < 30) {
            if (roundNumber) *roundNumber = 0;
            if (avgConf) *avgConf = 0.0f;
            return QString();
        }

        float conf = 0.0f;
        int d = predictDigit(roiImg, &conf);
        if (roundNumber) *roundNumber = d;
        if (avgConf) *avgConf = conf;
        return QString::number(d);
    }

    // 2-Digit Manual Mode: The ROI is split horizontally into 2 slots (Tens and Ones)
    if (mode == DigitMode::TwoDigits) {
        QImage rgbImg = roiImg.convertToFormat(QImage::Format_RGB32);
        int w = rgbImg.width();
        int h = rgbImg.height();
        int redCount = 0;
        for (int y = 0; y < h; ++y) {
            const QRgb *line = reinterpret_cast<const QRgb*>(rgbImg.constScanLine(y));
            for (int x = 0; x < w; ++x) {
                QRgb p = line[x];
                int r = qRed(p);
                int g = qGreen(p);
                int b = qBlue(p);
                if (r > 75 && r > static_cast<int>(g * 1.3f) && r > static_cast<int>(b * 1.3f)) {
                    redCount++;
                }
            }
        }
        if (redCount < 50) {
            if (roundNumber) *roundNumber = 0;
            if (avgConf) *avgConf = 0.0f;
            return QString();
        }

        int splitX = w / 2;
        QImage c1 = roiImg.copy(0, 0, splitX, h);
        QImage c2 = roiImg.copy(splitX, 0, w - splitX, h);

        float conf1 = 0.0f, conf2 = 0.0f;
        int d1 = predictDigit(c1, &conf1);
        int d2 = predictDigit(c2, &conf2);

        if (d1 == 0) {
            d1 = d2; // Fallback to avoid '05' artifact
        }

        int val = d1 * 10 + d2;
        float c = (conf1 + conf2) / 2.0f;
        if (roundNumber) *roundNumber = val;
        if (avgConf) *avgConf = c;
        return QString::asprintf("%d%d", d1, d2);
    }

    // 3-Digit Manual Mode: Force 3-digit detection across the 3 canonical slots
    if (mode == DigitMode::ThreeDigits) {
        QImage canonicalRoi = (roiImg.width() == 189 && roiImg.height() == 130)
            ? roiImg
            : roiImg.scaled(189, 130, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QImage rgbImg = canonicalRoi.convertToFormat(QImage::Format_RGB32);
        const int w = 189;
        const int h = 130;
        int redCount = 0;
        for (int y = 0; y < h; ++y) {
            const QRgb *line = reinterpret_cast<const QRgb*>(rgbImg.constScanLine(y));
            for (int x = 0; x < w; ++x) {
                QRgb p = line[x];
                int r = qRed(p);
                int g = qGreen(p);
                int b = qBlue(p);
                if (r > 75 && r > static_cast<int>(g * 1.3f) && r > static_cast<int>(b * 1.3f)) {
                    redCount++;
                }
            }
        }
        if (redCount < 70) {
            if (roundNumber) *roundNumber = 0;
            if (avgConf) *avgConf = 0.0f;
            return QString();
        }

        QImage c1 = canonicalRoi.copy(0, 0, 66, 130);
        QImage c2 = canonicalRoi.copy(66, 0, 58, 130);
        QImage c3 = canonicalRoi.copy(124, 0, 65, 130);

        float conf1 = 0.0f, conf2 = 0.0f, conf3 = 0.0f;
        int d1 = predictDigit(c1, &conf1);
        int d2 = predictDigit(c2, &conf2);
        int d3 = predictDigit(c3, &conf3);

        int val = d1 * 100 + d2 * 10 + d3;
        float c = (conf1 + conf2 + conf3) / 3.0f;
        if (roundNumber) *roundNumber = val;
        if (avgConf) *avgConf = c;
        return QString::asprintf("%d%d%d", d1, d2, d3);
    }

    // Default 3-slot Auto mode:
    // Scale ROI to canonical 189x130 (the exact resolution used for model training & calibration)
    QImage canonicalRoi = (roiImg.width() == 189 && roiImg.height() == 130)
        ? roiImg
        : roiImg.scaled(189, 130, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QImage rgbImg = canonicalRoi.convertToFormat(QImage::Format_RGB32);
    const int w = 189;
    const int h = 130;

    // Detect red blood markings in the 3 digit slots:
    // Slot 1 (Hundreds): x in [0, 66)
    // Slot 2 (Tens):     x in [66, 124)
    // Slot 3 (Ones):     x in [124, 189)
    int redCount1 = 0;
    int redCount2 = 0;
    int redCount3 = 0;

    for (int y = 0; y < h; ++y) {
        const QRgb *line = reinterpret_cast<const QRgb*>(rgbImg.constScanLine(y));
        for (int x = 0; x < w; ++x) {
            QRgb p = line[x];
            int r = qRed(p);
            int g = qGreen(p);
            int b = qBlue(p);
            // Red pixel condition: R is prominent over G and B (characteristic of Zombies blood tally numbers)
            if (r > 75 && r > static_cast<int>(g * 1.3f) && r > static_cast<int>(b * 1.3f)) {
                if (x < 66) {
                    redCount1++;
                } else if (x < 124) {
                    redCount2++;
                } else {
                    redCount3++;
                }
            }
        }
    }

    const int RED_THRESHOLD = 70;
    bool hasSlot1 = (redCount1 >= RED_THRESHOLD);
    bool hasSlot2 = (redCount2 >= RED_THRESHOLD);
    bool hasSlot3 = (redCount3 >= RED_THRESHOLD);

    // If no digits detected at all (e.g. pause menu, lobby, blackout transition)
    // Also reject frames flooded by full-screen red zombie damage vignette / transition flash
    const int totalRed = redCount1 + redCount2 + redCount3;
    if ((!hasSlot3 && !hasSlot2 && !hasSlot1) || redCount1 > 5200 || totalRed > 16500) {
        if (roundNumber) *roundNumber = 0;
        if (avgConf) *avgConf = 0.0f;
        return QString();
    }

    // Crop the 3 canonical slots matching capture_dataset.py and auto_sorter.py
    QImage c1 = canonicalRoi.copy(0, 0, 66, 130);
    QImage c2 = canonicalRoi.copy(66, 0, 58, 130);
    QImage c3 = canonicalRoi.copy(124, 0, 65, 130);

    float conf1 = 0.0f, conf2 = 0.0f, conf3 = 0.0f;
    int d1 = 0, d2 = 0, d3 = 0;
    int val = 0;
    float c = 0.0f;
    QString resultStr;

    if (hasSlot1 && hasSlot2 && hasSlot3) {
        // CASE 3: Three digits (e.g. 100 - 999), with leading-zero fallback to 2D / 1D
        d1 = predictDigit(c1, &conf1);
        d2 = predictDigit(c2, &conf2);
        d3 = predictDigit(c3, &conf3);

        if (d1 > 0) {
            val = d1 * 100 + d2 * 10 + d3;
            c = (conf1 + conf2 + conf3) / 3.0f;
            resultStr = QString::asprintf("%d%d%d", d1, d2, d3);
        } else if (d2 > 0) {
            val = d2 * 10 + d3;
            c = (conf2 + conf3) / 2.0f;
            resultStr = QString::asprintf("%d%d", d2, d3);
        } else {
            val = d3;
            c = conf3;
            resultStr = QString::number(d3);
        }
    }
    else if (hasSlot2 && hasSlot3) {
        // CASE 2a: Two digits in Tens + Ones slots (e.g. 10 - 99)
        d2 = predictDigit(c2, &conf2);
        d3 = predictDigit(c3, &conf3);

        if (d2 > 0) {
            val = d2 * 10 + d3;
            c = (conf2 + conf3) / 2.0f;
            resultStr = QString::asprintf("%d%d", d2, d3);
        } else {
            val = d3;
            c = conf3;
            resultStr = QString::number(d3);
        }
    }
    else if (hasSlot1 && hasSlot2) {
        // CASE 2b: Two digits in Left + Middle slots (e.g. 10 - 99)
        d1 = predictDigit(c1, &conf1);
        d2 = predictDigit(c2, &conf2);

        if (d1 > 0) {
            val = d1 * 10 + d2;
            c = (conf1 + conf2) / 2.0f;
            resultStr = QString::asprintf("%d%d", d1, d2);
        } else {
            val = d2;
            c = conf2;
            resultStr = QString::number(d2);
        }
    }
    else if (hasSlot3) {
        // CASE 1a: Single digit in right slot (e.g. 6 - 9)
        d3 = predictDigit(c3, &conf3);
        val = d3;
        c = conf3;
        resultStr = QString::number(d3);
    }
    else if (hasSlot2) {
        // CASE 1b: Single digit in middle slot (e.g. 6 - 9)
        d2 = predictDigit(c2, &conf2);
        val = d2;
        c = conf2;
        resultStr = QString::number(d2);
    }
    else if (hasSlot1) {
        // CASE 1c: Single digit in left slot (e.g. 6 - 9)
        d1 = predictDigit(c1, &conf1);
        val = d1;
        c = conf1;
        resultStr = QString::number(d1);
    }

    // Rounds 1-5 are Roman numerals; 6 is the lowest valid Arabic digit round
    if (val < 6) {
        if (roundNumber) *roundNumber = 0;
        if (avgConf) *avgConf = 0.0f;
        return QString();
    }

    if (roundNumber) *roundNumber = val;
    if (avgConf) *avgConf = c;
    return resultStr;
}
