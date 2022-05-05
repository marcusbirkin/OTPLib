#include "test_otp_advertisement_layer.hpp"
#include "test_appendix_B.h"
#include "test_otp_helper.hpp"

const size_t PDUOctlet = 79;
const size_t PDULength = (86+1) - PDUOctlet;

void TEST_OTP::PDU::OTPAdvertisementLayer::initTestCase()
{
    DefaultPDUByteArray = DefaultLayer.toPDUByteArray();
    QCOMPARE(DefaultPDUByteArray.size(), PDULength);
}

void TEST_OTP::PDU::OTPAdvertisementLayer::isValid()
{
    QVERIFY(!DefaultLayer.isValid());

    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_TRANSFORM_MESSAGE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QVERIFY2(layer.isValid(),
                     QString(
                        "Appendix B: Example Messages (Informative)\n"
                        "#%1 Failed")
                     .arg(example.second.TableName)
                     .toLatin1());
        }
    }
}

void TEST_OTP::PDU::OTPAdvertisementLayer::toFromPDUByteArray()
{
    {
        Layer layer(DefaultPDUByteArray);
        QCOMPARE(layer.toPDUByteArray(), DefaultPDUByteArray);
    }

    {
        for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
        {
            if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE) {
                PDUByteArray pdu;
                pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
                while (pdu.size())
                {
                    pdu.resize(pdu.size() - 1);
                    Layer layer(pdu);
                    QVERIFY(!layer.isValid());
                }
            }
        }
    }
}

void TEST_OTP::PDU::OTPAdvertisementLayer::vector()
{
    const unsigned int octlet = 79;
    const size_t fieldSize = 2;
    const quint16 valueMin = std::numeric_limits<quint16>::min();
    const quint16 valueMax = std::numeric_limits<quint16>::max();
    QVERIFY(sizeof(DefaultLayer.getVector()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getVector(), 0);
        QVERIFY(!layer.isValid());
    }

    /* Constructor with options */
    {
        {
            Layer layer(VECTOR_OTP_ADVERTISEMENT_MODULE);
            QCOMPARE(layer.getVector(), VECTOR_OTP_ADVERTISEMENT_MODULE);
        }
        {
            Layer layer(VECTOR_OTP_ADVERTISEMENT_NAME);
            QCOMPARE(layer.getVector(), VECTOR_OTP_ADVERTISEMENT_NAME);
        }
        {
            Layer layer(VECTOR_OTP_ADVERTISEMENT_SYSTEM);
            QCOMPARE(layer.getVector(), VECTOR_OTP_ADVERTISEMENT_SYSTEM);
        }
    }

    /* Constructor from PDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        for (auto value = valueMin; value < valueMax; ++value)
        {
            PDUByteArray ba; ba << value;
            pdu.replace(octlet - PDUOctlet, fieldSize, ba);
            Layer layer(pdu);
            QCOMPARE(layer.getVector(), value);
        }
    }

    /* set function */
    {
        Layer layer;
        {
            layer.setVector(VECTOR_OTP_ADVERTISEMENT_MODULE);
            QCOMPARE(layer.getVector(), VECTOR_OTP_ADVERTISEMENT_MODULE);
        }
        {
            layer.setVector(VECTOR_OTP_ADVERTISEMENT_NAME);
            QCOMPARE(layer.getVector(), VECTOR_OTP_ADVERTISEMENT_NAME);
        }
        {
            layer.setVector(VECTOR_OTP_ADVERTISEMENT_SYSTEM);
            QCOMPARE(layer.getVector(), VECTOR_OTP_ADVERTISEMENT_SYSTEM);
        }
    }

    /* fromPDUByteArray <> toPDUByteArray */
    TEST_OTP::HELPER::COMPARE_toFromPDUByteArray(
                DefaultPDUByteArray, Layer(),
                octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getVector(), example.second.OTPAdvertisementLayer.vector);
        }
    }
}

void TEST_OTP::PDU::OTPAdvertisementLayer::length()
{
    const unsigned int octlet = 81;
    const size_t fieldSize = 2;
    const quint16 valueMin = std::numeric_limits<quint16>::min();
    const quint16 valueMax = std::numeric_limits<quint16>::max();
    QVERIFY(sizeof(DefaultLayer.getPDULength()) == fieldSize);
    QCOMPARE(LENGTHOFFSET, size_t(4));

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getPDULength(), valueMin);
    }

    /* Constructor with options */
    {
        for (auto value = valueMin; value < valueMax; ++value)
        {
            Layer layer(0, value);
            QCOMPARE(layer.getPDULength(), value);
            QVERIFY(!layer.isValid());
        }
    }

    /* Constructor from PDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        for (auto value = valueMin; value < valueMax; ++value)
        {
            PDUByteArray ba; ba << value;
            pdu.replace(octlet - PDUOctlet, fieldSize, ba);
            Layer layer(pdu);
            QCOMPARE(layer.getPDULength(), value);
        }
    }

    /* set function */
    {
        Layer layer;
        for (auto value = valueMin; value < valueMax; ++value)
        {
            layer.setPDULength(value);
            QCOMPARE(layer.getPDULength(), value);
        }
    }

    /* fromPDUByteArray <> toPDUByteArray */
    TEST_OTP::HELPER::COMPARE_toFromPDUByteArray(
                DefaultPDUByteArray, Layer(),
                octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getPDULength(), example.second.OTPAdvertisementLayer.length);
        }
    }
}

void TEST_OTP::PDU::OTPAdvertisementLayer::reserved()
{
    const unsigned int octlet = 83;
    const size_t fieldSize = 4;
    const quint32 valueMin = std::numeric_limits<quint32>::min();
    const quint32 valueMax = std::numeric_limits<quint32>::max();
    const quint32 valueStep = valueMax / 0xFFFF;
    QVERIFY(sizeof(DefaultLayer.getReserved()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getReserved(), valueMin);
    }

    /* Constructor with options */
    // N/A

    /* Constructor from PDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        for (auto value = valueMin; value < valueMax; value += valueStep)
        {
            PDUByteArray ba; ba << value;
            pdu.replace(octlet - PDUOctlet, fieldSize, ba);
            Layer layer(pdu);
            QCOMPARE(layer.getReserved(), value);
        }
    }

    /* set function */
    // N/A

    /* fromPDUByteArray <> toPDUByteArray */
    TEST_OTP::HELPER::COMPARE_toFromPDUByteArray(
                DefaultPDUByteArray, Layer(),
                octlet - PDUOctlet, fieldSize, valueMin, valueMax, valueStep);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getReserved(), example.second.OTPAdvertisementLayer.reserved);
        }
    }
}
