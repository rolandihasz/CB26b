#include <gtest/gtest.h>
#include "eagleeye_classifier.h"
#include <QImage>
#include <QDir>
#include <QFile>
#include <cmath>

class EagleEyeClassifierTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Find eagle_eye_weights.bin from the repository root or build output
        m_weightsPath = QStringLiteral("../eagle_eye_weights.bin");
        if (!QFile::exists(m_weightsPath)) {
            m_weightsPath = QStringLiteral("eagle_eye_weights.bin");
        }
        if (!QFile::exists(m_weightsPath)) {
            m_weightsPath = QStringLiteral("../../eagle_eye_weights.bin");
        }
    }

    QString m_weightsPath;
};

// ----------------------------------------------------------------------------
// Initial and Invalid State Tests
// ----------------------------------------------------------------------------

TEST_F(EagleEyeClassifierTest, UnloadedModelBehavior) {
    EagleEyeClassifier classifier;
    EXPECT_FALSE(classifier.isLoaded());

    float dummyInput[28 * 28] = {0.0f};
    std::vector<float> logits = classifier.forward(dummyInput);
    ASSERT_EQ(logits.size(), 10u);
    for (float val : logits) {
        EXPECT_FLOAT_EQ(val, 0.0f);
    }

    float confidence = 1.0f;
    QImage testImg(28, 28, QImage::Format_Grayscale8);
    int pred = classifier.predictDigit(testImg, &confidence);
    EXPECT_EQ(pred, 0);
    EXPECT_FLOAT_EQ(confidence, 0.0f);
}

TEST_F(EagleEyeClassifierTest, LoadWeightsNonExistentFile) {
    EagleEyeClassifier classifier;
    bool result = classifier.loadWeights(QStringLiteral("non_existent_weights_file.bin"));
    EXPECT_FALSE(result);
    EXPECT_FALSE(classifier.isLoaded());
}

TEST_F(EagleEyeClassifierTest, LoadWeightsCorruptedSize) {
    // Create a temporary file with corrupted/incomplete size
    QString tempFile = QStringLiteral("corrupted_weights.bin");
    QFile f(tempFile);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write(QByteArray(128, 'x')); // Incorrect size
    f.close();

    EagleEyeClassifier classifier;
    EXPECT_FALSE(classifier.loadWeights(tempFile));
    EXPECT_FALSE(classifier.isLoaded());

    QFile::remove(tempFile);
}

// ----------------------------------------------------------------------------
// Inference and Forward Pass Tests (with Model Weights)
// ----------------------------------------------------------------------------

TEST_F(EagleEyeClassifierTest, LoadRealWeightsAndForwardPass) {
    if (!QFile::exists(m_weightsPath)) {
        GTEST_SKIP() << "eagle_eye_weights.bin not found at " << m_weightsPath.toStdString();
    }

    EagleEyeClassifier classifier;
    ASSERT_TRUE(classifier.loadWeights(m_weightsPath));
    EXPECT_TRUE(classifier.isLoaded());

    // Run forward pass with synthetic normalized input [-1, 1]
    float input[28 * 28];
    for (int i = 0; i < 28 * 28; ++i) {
        input[i] = (i % 2 == 0) ? -1.0f : 1.0f;
    }

    std::vector<float> logits = classifier.forward(input);
    ASSERT_EQ(logits.size(), 10u);

    for (float l : logits) {
        EXPECT_FALSE(std::isnan(l));
        EXPECT_FALSE(std::isinf(l));
    }
}

TEST_F(EagleEyeClassifierTest, PredictDigitRangeAndConfidence) {
    if (!QFile::exists(m_weightsPath)) {
        GTEST_SKIP() << "eagle_eye_weights.bin not found at " << m_weightsPath.toStdString();
    }

    EagleEyeClassifier classifier;
    ASSERT_TRUE(classifier.loadWeights(m_weightsPath));

    // Create a synthetic 28x28 test image
    QImage testImg(28, 28, QImage::Format_RGB32);
    testImg.fill(Qt::black);

    float confidence = 0.0f;
    int digit = classifier.predictDigit(testImg, &confidence);

    EXPECT_GE(digit, 0);
    EXPECT_LE(digit, 9);
    EXPECT_GE(confidence, 0.0f);
    EXPECT_LE(confidence, 1.0f);
}

TEST_F(EagleEyeClassifierTest, PredictRoundBlankOrNoRedReturnsEmpty) {
    if (!QFile::exists(m_weightsPath)) {
        GTEST_SKIP() << "eagle_eye_weights.bin not found at " << m_weightsPath.toStdString();
    }

    EagleEyeClassifier classifier;
    ASSERT_TRUE(classifier.loadWeights(m_weightsPath));

    // When no red digits are in the ROI (e.g. pause menu, lobby, blackout), returns empty string
    QImage blankImg(189, 130, QImage::Format_RGB32);
    blankImg.fill(Qt::black);

    int roundNum = -1;
    float avgConf = 1.0f;
    QString resultStr = classifier.predictRound(blankImg, &roundNum, &avgConf);

    EXPECT_TRUE(resultStr.isEmpty());
    EXPECT_EQ(roundNum, 0);
    EXPECT_FLOAT_EQ(avgConf, 0.0f);
}

TEST_F(EagleEyeClassifierTest, PredictRoundWith3DigitsROI) {
    if (!QFile::exists(m_weightsPath)) {
        GTEST_SKIP() << "eagle_eye_weights.bin not found at " << m_weightsPath.toStdString();
    }

    EagleEyeClassifier classifier;
    ASSERT_TRUE(classifier.loadWeights(m_weightsPath));

    // ROI image with prominent red tally marks
    QImage roiImg(189, 130, QImage::Format_RGB32);
    roiImg.fill(QColor(230, 20, 20));

    int roundNum = -1;
    float avgConf = 0.0f;
    QString resultStr = classifier.predictRound(roiImg, &roundNum, &avgConf, DigitMode::ThreeDigits);

    // Must detect all 3 red slots and return a 3-digit string representation
    EXPECT_EQ(resultStr.length(), 3);
    EXPECT_GE(roundNum, 0);
    EXPECT_LE(roundNum, 999);
    EXPECT_GE(avgConf, 0.0f);
    EXPECT_LE(avgConf, 1.0f);
}

TEST_F(EagleEyeClassifierTest, PredictRoundWith1DigitROI) {
    if (!QFile::exists(m_weightsPath)) {
        GTEST_SKIP() << "eagle_eye_weights.bin not found at " << m_weightsPath.toStdString();
    }

    EagleEyeClassifier classifier;
    ASSERT_TRUE(classifier.loadWeights(m_weightsPath));

    // 1-digit ROI box (65 x 130) with prominent red marking
    QImage roi1D(65, 130, QImage::Format_RGB32);
    roi1D.fill(QColor(230, 20, 20));

    int roundNum = -1;
    float avgConf = 0.0f;
    QString resultStr = classifier.predictRound(roi1D, &roundNum, &avgConf, DigitMode::OneDigit);

    EXPECT_EQ(resultStr.length(), 1);
    EXPECT_GE(roundNum, 0);
    EXPECT_LE(roundNum, 9);
    EXPECT_GE(avgConf, 0.0f);
    EXPECT_LE(avgConf, 1.0f);
}

TEST_F(EagleEyeClassifierTest, PredictRoundWith2DigitsROI) {
    if (!QFile::exists(m_weightsPath)) {
        GTEST_SKIP() << "eagle_eye_weights.bin not found at " << m_weightsPath.toStdString();
    }

    EagleEyeClassifier classifier;
    ASSERT_TRUE(classifier.loadWeights(m_weightsPath));

    // 2-digits ROI box (123 x 130) with prominent red marking
    QImage roi2D(123, 130, QImage::Format_RGB32);
    roi2D.fill(QColor(230, 20, 20));

    int roundNum = -1;
    float avgConf = 0.0f;
    QString resultStr = classifier.predictRound(roi2D, &roundNum, &avgConf, DigitMode::TwoDigits);

    EXPECT_EQ(resultStr.length(), 2);
    EXPECT_GE(roundNum, 0);
    EXPECT_LE(roundNum, 99);
    EXPECT_GE(avgConf, 0.0f);
    EXPECT_LE(avgConf, 1.0f);
}


TEST_F(EagleEyeClassifierTest, LoadPyTorchPthModelMatchesBinWeights) {
    QString pthPath = QStringLiteral("C:/Users/rolan/Documents/EagleEye/eagle_eye_model.pth");
    if (!QFile::exists(pthPath)) {
        pthPath = QStringLiteral("../eagle_eye_model.pth");
    }
    if (!QFile::exists(pthPath)) {
        pthPath = QStringLiteral("eagle_eye_model.pth");
    }
    if (!QFile::exists(pthPath)) {
        pthPath = QStringLiteral("../../eagle_eye_model.pth");
    }
    if (!QFile::exists(pthPath) || !QFile::exists(m_weightsPath)) {
        GTEST_SKIP() << "Model files not found for .pth vs .bin comparison";
    }

    EagleEyeClassifier pthClassifier;
    EagleEyeClassifier binClassifier;
    ASSERT_TRUE(pthClassifier.loadWeights(pthPath));
    ASSERT_TRUE(binClassifier.loadWeights(m_weightsPath));

    float input[28 * 28];
    for (int i = 0; i < 28 * 28; ++i) {
        input[i] = std::sin(static_cast<float>(i) * 0.1f);
    }

    std::vector<float> pthLogits = pthClassifier.forward(input);
    std::vector<float> binLogits = binClassifier.forward(input);
    ASSERT_EQ(pthLogits.size(), 10u);
    ASSERT_EQ(binLogits.size(), 10u);
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_FLOAT_EQ(pthLogits[i], binLogits[i]);
    }
}

TEST_F(EagleEyeClassifierTest, AutoModeIgnoresRomanNumeralRoundsBelowSix) {
    if (!QFile::exists(m_weightsPath)) {
        GTEST_SKIP() << "eagle_eye_weights.bin not found at " << m_weightsPath.toStdString();
    }

    EagleEyeClassifier classifier;
    ASSERT_TRUE(classifier.loadWeights(m_weightsPath));

    // Create a 3-slot canonical ROI (189 x 130) with only 1 active red slot (ones slot)
    // where a uniform red block predicts a digit < 6
    QImage roiImg(189, 130, QImage::Format_RGB32);
    roiImg.fill(Qt::black);
    for (int y = 20; y < 110; ++y) {
        for (int x = 140; x < 170; ++x) {
            roiImg.setPixelColor(x, y, QColor(220, 20, 20));
        }
    }

    // Check what single digit is predicted on the right slot directly
    int rawDigit = classifier.predictDigit(roiImg.copy(124, 0, 65, 130));
    if (rawDigit < 6) {
        int roundNum = -1;
        float conf = 1.0f;
        QString roundStr = classifier.predictRound(roiImg, &roundNum, &conf, DigitMode::Auto);
        EXPECT_TRUE(roundStr.isEmpty());
        EXPECT_EQ(roundNum, 0);
        EXPECT_FLOAT_EQ(conf, 0.0f);
    }
}
