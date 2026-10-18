#include "eagleeye_classifier.h"

#include <QFile>
#include <QDebug>
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

    const size_t expectedFloats = 144 + 16 + 4608 + 32 + 200704 + 128 + 1280 + 10; // 206922 floats = 827688 bytes
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
                dst[x] = static_cast<uchar>(0.299f * qRed(p) + 0.587f * qGreen(p) + 0.114f * qBlue(p));
            }
        }
    }

    // 2. Resize to 28x28 grayscale (matching transforms.Resize((28, 28)))
    QImage scaled = grayImg.scaled(28, 28, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

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

QString EagleEyeClassifier::predictRound(const QImage &roiImg, int *roundNumber, float *avgConf) const
{
    if (roiImg.isNull() || !m_loaded) {
        if (roundNumber) *roundNumber = 0;
        if (avgConf) *avgConf = 0.0f;
        return QString();
    }

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
    if (!hasSlot3 && !hasSlot2 && !hasSlot1) {
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

    if (hasSlot1 && hasSlot2 && hasSlot3) {
        // CASE 3: Three digits (e.g. 100 - 999)
        d1 = predictDigit(c1, &conf1);
        d2 = predictDigit(c2, &conf2);
        d3 = predictDigit(c3, &conf3);

        int val = d1 * 100 + d2 * 10 + d3;
        float c = (conf1 + conf2 + conf3) / 3.0f;
        if (roundNumber) *roundNumber = val;
        if (avgConf) *avgConf = c;
        return QString::asprintf("%d%d%d", d1, d2, d3);
    }
    else if (hasSlot2 && hasSlot3) {
        // CASE 2: Two digits (e.g. 10 - 99)
        d2 = predictDigit(c2, &conf2);
        d3 = predictDigit(c3, &conf3);

        int val = d2 * 10 + d3;
        float c = (conf2 + conf3) / 2.0f;
        if (roundNumber) *roundNumber = val;
        if (avgConf) *avgConf = c;
        return QString::asprintf("%d%d", d2, d3);
    }
    else if (hasSlot3) {
        // CASE 1: Single digit (e.g. 1 - 9)
        d3 = predictDigit(c3, &conf3);

        int val = d3;
        float c = conf3;
        if (roundNumber) *roundNumber = val;
        if (avgConf) *avgConf = c;
        return QString::number(d3);
    }
    else if (hasSlot2) {
        // Fallback: single digit detected in slot 2
        d2 = predictDigit(c2, &conf2);
        int val = d2;
        float c = conf2;
        if (roundNumber) *roundNumber = val;
        if (avgConf) *avgConf = c;
        return QString::number(d2);
    }

    if (roundNumber) *roundNumber = 0;
    if (avgConf) *avgConf = 0.0f;
    return QString();
}
