#include "test_otp_transform_layer.hpp"
#include "test_appendix_B.h"

const size_t PDUOctlet = 79;
const size_t PDULength = (96+1) - PDUOctlet;

void TEST_OTP::PDU::OTPTransformLayer::initTestCase()
{
    DefaultPDUByteArray = DefaultLayer.toPDUByteArray();
    QCOMPARE(DefaultPDUByteArray.size(), PDULength);
}

template <typename T>
void TEST_OTP::PDU::OTPTransformLayer::helper_toFromPDUByteArray(unsigned int octlet,
        size_t fieldSize,
        T valueMin,
        T valueMax,
        T forStep)
{
    PDUByteArray pdu(DefaultPDUByteArray);
    Layer layer(pdu);
    for (auto value = valueMin; value < valueMax; value += forStep)
    {
        PDUByteArray ba; ba << value;
        pdu.replace(octlet, fieldSize, ba);
        layer.fromPDUByteArray(pdu);
        QCOMPARE(layer.toPDUByteArray(), pdu);
    }
}

void TEST_OTP::PDU::OTPTransformLayer::isValid()
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

void TEST_OTP::PDU::OTPTransformLayer::toFromPDUByteArray()
{
    {
        Layer layer(DefaultPDUByteArray);
        QCOMPARE(layer.toPDUByteArray(), DefaultPDUByteArray);
    }

    {
        for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
        {
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

void TEST_OTP::PDU::OTPTransformLayer::vector()
{
    const unsigned int octlet = 79;
    const size_t fieldSize = 2;
    const quint16 valueMin = std::numeric_limits<quint16>::min();
    const quint16 valueMax = std::numeric_limits<quint16>::max();
    QVERIFY(sizeof(DefaultLayer.getVector()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getVector(), VECTOR_OTP_POINT);
    }

    /* Constructor with options */
    // N/A

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
    // N/A

    /* fromPDUByteArray <> toPDUByteArray */
    helper_toFromPDUByteArray(octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_TRANSFORM_MESSAGE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getVector(), example.second.OTPTransformLayer.vector);
        }
    }
}

void TEST_OTP::PDU::OTPTransformLayer::length()
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
            Layer layer(value);
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
    helper_toFromPDUByteArray(octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_TRANSFORM_MESSAGE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getPDULength(), example.second.OTPTransformLayer.length);
        }
    }
}

void TEST_OTP::PDU::OTPTransformLayer::system()
{
    const unsigned int octlet = 83;
    const size_t fieldSize = 1;
    const OTP::range_t valueRange = {1,200}; // 8.3 System Number
    const quint8 valueMin = std::numeric_limits<quint8>::min();
    const quint8 valueMax = std::numeric_limits<quint8>::max();
    QVERIFY(sizeof(DefaultLayer.getSystem()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getSystem(), valueMin);
    }

    /* Constructor with options */
    {
        for (auto value = valueMin; value < valueMax; ++value)
        {
            Layer layer(pduLength_t(), value);
            QCOMPARE(layer.getSystem(), value);
            if (value >= valueRange.getMin() && value <= valueRange.getMax())
                QVERIFY(layer.getSystem().isValid());
            else
                QVERIFY(!layer.getSystem().isValid());
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
            QCOMPARE(layer.getSystem(), value);
            if (value >= valueRange.getMin() && value <= valueRange.getMax())
                QVERIFY(layer.getSystem().isValid());
            else
                QVERIFY(!layer.getSystem().isValid());
            QVERIFY(!layer.isValid());
        }
    }

    /* set function */
    {
        Layer layer;
        for (auto value = valueMin; value < valueMax; ++value)
        {
            layer.setSystem(value);
            QCOMPARE(layer.getSystem(), value);
        }
    }

    /* fromPDUByteArray <> toPDUByteArray */
    helper_toFromPDUByteArray(octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_TRANSFORM_MESSAGE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getSystem(), example.second.OTPTransformLayer.system);
        }
    }
}

void TEST_OTP::PDU::OTPTransformLayer::timestamp()
{
    const unsigned int octlet = 84;
    const size_t fieldSize = 8;
    const quint64 valueMin = std::numeric_limits<quint64>::min();
    const quint64 valueMax = std::numeric_limits<quint64>::max();
    const quint64 valueStep = valueMax / 0xFFFF;
    QVERIFY(sizeof(DefaultLayer.getTimestamp()) == fieldSize);

    /* Default constructor*/
    {
        decltype(valueMax) ref1 = QDateTime::currentMSecsSinceEpoch() * 1000;
        Layer layer;
        decltype(valueMax) ref2 = QDateTime::currentMSecsSinceEpoch() * 1000;
        QVERIFY(layer.getTimestamp() >= ref1 && layer.getTimestamp() <= ref2);
    }

    /* Constructor with options */
    {
        for (auto value = valueMin; value < valueMax; value += valueStep)
        {
            Layer layer(pduLength_t(),system_t(),value);
            QCOMPARE(layer.getTimestamp(), value);
            QVERIFY(!layer.isValid());
        }
    }

    /* Constructor from PDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        for (auto value = valueMin; value < valueMax; value += valueStep)
        {
            PDUByteArray ba; ba << value;
            pdu.replace(octlet - PDUOctlet, fieldSize, ba);
            Layer layer(pdu);
            QCOMPARE(layer.getTimestamp(), value);
        }
    }

    /* set function */
    {
        Layer layer;
        for (auto value = valueMin; value < valueMax; value += valueStep)
        {
            layer.setTimestamp(value);
            QCOMPARE(layer.getTimestamp(), value);
        }
    }

    /* fromPDUByteArray <> toPDUByteArray */
    helper_toFromPDUByteArray(octlet - PDUOctlet, fieldSize, valueMin, valueMax, valueStep);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_TRANSFORM_MESSAGE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getTimestamp(), example.second.OTPTransformLayer.timestamp);
        }
    }
}

void TEST_OTP::PDU::OTPTransformLayer::options()
{
    const unsigned int octlet = 92;
    const size_t fieldSize = 1;
    const quint8 valueMin = std::numeric_limits<quint8>::min();
    const quint8 valueMax = std::numeric_limits<quint8>::max();
    {
        PDUByteArray ba; ba << DefaultLayer.getOptions();
        QVERIFY(ba.size() == fieldSize);
    }

    /* Default constructor*/
    {
        Layer layer;
        PDUByteArray ba; ba << layer.getOptions();
        QCOMPARE(ba.at(0), 0);
        QVERIFY(layer.getOptions().isFullPointSet() == false);
    }

    /* Constructor with options */
    {
        options_t options;
        {
            options.setFullPointSet(false);
            Layer layer(pduLength_t(), system_t(), timestamp_t(), options);
            QVERIFY(layer.getOptions().isFullPointSet() == false);
        }
        {
            options.setFullPointSet(true);
            Layer layer(pduLength_t(), system_t(), timestamp_t(), options);
            QVERIFY(layer.getOptions().isFullPointSet() == true);
        }
    }

    /* Constructor from PDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        for (auto value = valueMin; value < valueMax; ++value)
        {
            PDUByteArray baIn; baIn << value;
            pdu.replace(octlet - PDUOctlet, fieldSize, baIn);
            Layer layer(pdu);
            PDUByteArray baOut; baOut << layer.getOptions();
            QCOMPARE(static_cast<decltype(value)>(baOut.at(0)), value);
            QVERIFY(!layer.isValid());
        }
    }

    /* set function */
    // N/A

    /* fromPDUByteArray <> toPDUByteArray */
    helper_toFromPDUByteArray(octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_TRANSFORM_MESSAGE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getOptions().isFullPointSet(), example.second.OTPTransformLayer.option_isFullPointSet);
        }
    }
}

void TEST_OTP::PDU::OTPTransformLayer::reserved()
{
    const unsigned int octlet = 93;
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
    helper_toFromPDUByteArray(octlet - PDUOctlet, fieldSize, valueMin, valueMax, valueStep);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_TRANSFORM_MESSAGE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getReserved(), example.second.OTPTransformLayer.reserved);
        }
    }
}
