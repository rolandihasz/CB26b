#ifndef EAGLEEYE_CLASSIFIER_H
#define EAGLEEYE_CLASSIFIER_H

#include <QImage>
#include <QString>
#include <vector>

class EagleEyeClassifier {
public:
    EagleEyeClassifier();
    bool loadWeights(const QString &weightsPath);
    bool isLoaded() const { return m_loaded; }

    // Predicts a single digit from a QImage (scales to 28x28 grayscale and normalizes)
    int predictDigit(const QImage &img, float *confidence = nullptr) const;

    // Predicts from a 3-digit ROI image (splits horizontally into 3 digits)
    QString predictRound(const QImage &roiImg, int *roundNumber = nullptr, float *avgConf = nullptr) const;

    // Direct forward pass on [1, 28, 28] float array normalized in [-1, 1]
    std::vector<float> forward(const float *input28x28) const;

private:
    bool m_loaded = false;

    // Weights matching PyTorch EagleEyeNet
    std::vector<float> m_conv1_w; // 16 * 1 * 3 * 3 = 144
    std::vector<float> m_conv1_b; // 16
    std::vector<float> m_conv2_w; // 32 * 16 * 3 * 3 = 4608
    std::vector<float> m_conv2_b; // 32
    std::vector<float> m_fc1_w;   // 128 * 1568 = 200704
    std::vector<float> m_fc1_b;   // 128
    std::vector<float> m_fc2_w;   // 10 * 128 = 1280
    std::vector<float> m_fc2_b;   // 10
};

#endif // EAGLEEYE_CLASSIFIER_H
