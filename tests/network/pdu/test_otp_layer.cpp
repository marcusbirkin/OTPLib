#include "test_otp_layer.hpp"
#include "test_appendix_B.h"
#include "test_otp_helper.hpp"

const size_t PDUOctlet = 0;
const size_t PDULength = (78+1) - PDUOctlet;

void TEST_OTP::PDU::OTPLayer::initTestCase()
{
    DefaultPDUByteArray = DefaultLayer.toPDUByteArray();
    QCOMPARE(DefaultPDUByteArray.size(), PDULength);
}

void TEST_OTP::PDU::OTPLayer::isValid()
{
    QVERIFY(!DefaultLayer.isValid());

    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
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

void TEST_OTP::PDU::OTPLayer::toFromPDUByteArray()
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
            {
                Layer layer(pdu);
                QVERIFY(layer.isValid());
            }
            while (pdu.size())
            {
                pdu.resize(pdu.size() - 1);
                Layer layer(pdu);
                QVERIFY(!layer.isValid());
            }
        }
    }
}

void TEST_OTP::PDU::OTPLayer::packetIdent()
{
    const unsigned int octlet = 0;
    const size_t fieldSize = 12;
    QVERIFY(DefaultLayer.getPacketIdent().size() == fieldSize);

    /* Default constructor */
    {
        Layer layer;
        QCOMPARE(layer.getPacketIdent(), OTP_PACKET_IDENT);
    }

    /* Constructor with options */
    // N/A

    /* Constructor from PDUByteArray */
    {
        Layer layer(DefaultPDUByteArray);
        QCOMPARE(layer.getPacketIdent(), OTP_PACKET_IDENT);
    }
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        pdu.replace(octlet - PDUOctlet, fieldSize, QByteArrayLiteral("\x4f\x54\x50\x2d\x45\x31\x2e\x35\x39\x00\x00\x00"));
        Layer layer(pdu);
        QCOMPARE(layer.getPacketIdent(), OTP_PACKET_IDENT);
    }
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        for (auto n = 0; n <= 0xFF; ++n)
        {
            const auto value = QByteArray(fieldSize, char(n));
            pdu.replace(octlet - PDUOctlet, fieldSize, value);
            Layer layer(pdu);
            QCOMPARE(layer.getPacketIdent(), value);
        }
    }

    /* set function */
    // N/A

    /* fromPDUByteArray <> toPDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        Layer layer(pdu);
        for (auto n = 0; n <= 0xFF; ++n)
        {
            const auto value = QByteArray(fieldSize, char(n));
            pdu.replace(octlet - PDUOctlet, fieldSize, value);
            layer.fromPDUByteArray(pdu);
            QCOMPARE(layer.toPDUByteArray(), pdu);
        }
    }

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getPacketIdent(), OTP_PACKET_IDENT);
    }
}

void TEST_OTP::PDU::OTPLayer::vector()
{
    const unsigned int octlet = 12;
    const size_t fieldSize = 2;
    const quint16 valueMin = std::numeric_limits<quint16>::min();
    const quint16 valueMax = std::numeric_limits<quint16>::max();
    QVERIFY(sizeof(DefaultLayer.getVector()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getVector(), valueMin);
    }

    /* Constructor with options */
    {
        Layer layer(VECTOR_OTP_TRANSFORM_MESSAGE);
        QCOMPARE(layer.getVector(), VECTOR_OTP_TRANSFORM_MESSAGE);
    }
    {
        Layer layer(VECTOR_OTP_ADVERTISEMENT_MESSAGE);
        QCOMPARE(layer.getVector(), VECTOR_OTP_ADVERTISEMENT_MESSAGE);
    }
    {
        for (auto value = valueMin; value < valueMax; ++value)
        {
            Layer layer(value);
           QCOMPARE(layer.getVector(), value);
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
        for (auto value = valueMin; value < valueMax; ++value)
        {
            layer.setVector(value);
            QCOMPARE(layer.getVector(), value);
        }
    }

    /* fromPDUByteArray <> toPDUByteArray */
    TEST_OTP::HELPER::COMPARE_toFromPDUByteArray(
                DefaultPDUByteArray, Layer(),
                octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getVector(), example.second.OTPLayer.vector);
    }
}

void TEST_OTP::PDU::OTPLayer::length()
{
    const unsigned int octlet = 14;
    const size_t fieldSize = 2;
    const quint16 valueMin = std::numeric_limits<quint16>::min();
    const quint16 valueMax = std::numeric_limits<quint16>::max();
    QVERIFY(sizeof(DefaultLayer.getPDULength()) == fieldSize);
    QCOMPARE(LENGTHOFFSET, size_t(16));

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getPDULength(), valueMin);
    }

    /* Constructor with options */
    {
        for (auto value = valueMin; value < valueMax; ++value)
        {
            Layer layer(vector_t(), value);
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
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getPDULength(), example.second.OTPLayer.length);
    }
}

void TEST_OTP::PDU::OTPLayer::footerOptions()
{
    const unsigned int octlet = 16;
    const size_t fieldSize = 1;
    const quint8 valueMin = std::numeric_limits<quint8>::min();
    const quint8 valueMax = std::numeric_limits<quint8>::max();
    QVERIFY(sizeof(DefaultLayer.getFooter().constFlags()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getFooter().constFlags(), valueMin);
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
            QCOMPARE(layer.getFooter().constFlags(), value);
            QVERIFY(!layer.isValid());
        }
    }

    /* set function */
    // N/A

    /* fromPDUByteArray <> toPDUByteArray */
    TEST_OTP::HELPER::COMPARE_toFromPDUByteArray(
                DefaultPDUByteArray, Layer(),
                octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getFooter().constFlags(), example.second.OTPLayer.footerOptions);
    }
}

void TEST_OTP::PDU::OTPLayer::footerLength()
{
    const unsigned int octlet = 17;
    const size_t fieldSize = 1;
    const quint8 valueMin = std::numeric_limits<quint8>::min();
    const quint8 valueMax = std::numeric_limits<quint8>::max();
    QVERIFY(sizeof(DefaultLayer.getFooter().getLength()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(DefaultLayer.getFooter().getLength(), valueMin);
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
            QCOMPARE(layer.getFooter().getLength(), value);
            QVERIFY(!layer.isValid());
        }
    }

    /* set function */
    // N/A

    /* fromPDUByteArray <> toPDUByteArray */
    TEST_OTP::HELPER::COMPARE_toFromPDUByteArray(
                DefaultPDUByteArray, Layer(),
                octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getFooter().getLength(), example.second.OTPLayer.footerLength);
    }
}

void TEST_OTP::PDU::OTPLayer::cid()
{
    const unsigned int octlet = 18;
    const size_t fieldSize = 16;
    QVERIFY(DefaultLayer.getCID().toRfc4122().size() == fieldSize);

    /* Default constructor */
    {
        Layer layer;
        QVERIFY(layer.getCID().toRfc4122() != QByteArray(fieldSize, static_cast<char>(0)));
    }

    /* Constructor with options */
    {
        const auto value = QUuid::createUuid();
        Layer layer(vector_t(), pduLength_t(), value);
        QCOMPARE(layer.getCID(), value);
        QVERIFY(!layer.isValid());
    }

    /* Constructor from PDUByteArray */
    {
        Layer layer(DefaultPDUByteArray);
        QVERIFY(layer.getCID().toRfc4122() != QByteArray(fieldSize, static_cast<char>(0)));
    }
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        const auto value = QUuid::createUuid().toRfc4122();
        pdu.replace(octlet - PDUOctlet, fieldSize, value);
        Layer layer(pdu);
        QCOMPARE(layer.getCID().toRfc4122(), value);
    }

    /* set function */
    {
        Layer layer;
        const auto value = QUuid::createUuid();
        layer.setCID(value);
        QCOMPARE(layer.getCID(), value);
    }

    /* fromPDUByteArray <> toPDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        Layer layer(pdu);
        for (auto n = 0; n <= 0xFF; ++n)
        {
            const auto value = QByteArray(fieldSize, char(n));
            pdu.replace(octlet - PDUOctlet, fieldSize, value);
            layer.fromPDUByteArray(pdu);
            QCOMPARE(layer.toPDUByteArray(), pdu);
        }
    }

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getCID(), example.second.OTPLayer.cid);
    }
}

void TEST_OTP::PDU::OTPLayer::folio()
{
    const unsigned int octlet = 34;
    const size_t fieldSize = 4;
    const quint32 valueMin = std::numeric_limits<quint32>::min();
    const quint32 valueMax = std::numeric_limits<quint32>::max();
    const quint32 valueStep = valueMax / 0xFFFF;
    QVERIFY(sizeof(DefaultLayer.getFolio()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getFolio(), valueMin);
    }

    /* Constructor with options */
    {
        for (auto value = valueMin; value < valueMax; value += valueStep)
        {
            Layer layer(vector_t(), pduLength_t(), cid_t(), value);
            QCOMPARE(layer.getFolio(), value);
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
            QCOMPARE(layer.getFolio(), value);
        }
    }

    /* set function */
    {
        Layer layer;
        for (auto value = valueMin; value < valueMax; value += valueStep)
        {
            layer.setFolio(value);
            QCOMPARE(layer.getFolio(), value);
        }
    }

    /* fromPDUByteArray <> toPDUByteArray */
    TEST_OTP::HELPER::COMPARE_toFromPDUByteArray(
                DefaultPDUByteArray, Layer(),
                octlet - PDUOctlet, fieldSize, valueMin, valueMax, valueStep);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getFolio(), example.second.OTPLayer.folio);
    }
}

void TEST_OTP::PDU::OTPLayer::page()
{
    const unsigned int octlet = 38;
    const size_t fieldSize = 2;
    const quint16 valueMin = std::numeric_limits<quint16>::min();
    const quint16 valueMax = std::numeric_limits<quint16>::max();
    QVERIFY(sizeof(DefaultLayer.getPage()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getPage(), valueMin);
    }

    /* Constructor with options */
    {
        for (auto value = valueMin; value < valueMax; ++value)
        {
            Layer layer(vector_t(), pduLength_t(), cid_t(), folio_t(), value);
            QCOMPARE(layer.getPage(), value);
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
            QCOMPARE(layer.getPage(), value);
        }
    }

    /* set function */
    {
        Layer layer;
        for (auto value = valueMin; value < valueMax; ++value)
        {
            layer.setPage(value);
            QCOMPARE(layer.getPage(), value);
        }
    }

    /* fromPDUByteArray <> toPDUByteArray */
    TEST_OTP::HELPER::COMPARE_toFromPDUByteArray(
                DefaultPDUByteArray, Layer(),
                octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getPage(), example.second.OTPLayer.page);
    }
}

void TEST_OTP::PDU::OTPLayer::lastPage()
{
    const unsigned int octlet = 40;
    const size_t fieldSize = 2;
    const quint16 valueMin = std::numeric_limits<quint16>::min();
    const quint16 valueMax = std::numeric_limits<quint16>::max();
    QVERIFY(sizeof(DefaultLayer.getLastPage()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getLastPage(), valueMin);
    }

    /* Constructor with options */
    {
        for (auto value = valueMin; value < valueMax; ++value)
        {
            Layer layer(vector_t(), pduLength_t(), cid_t(), folio_t(), page_t(), value);
            QCOMPARE(layer.getLastPage(), value);
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
            QCOMPARE(layer.getLastPage(), value);
        }
    }

    /* set function */
    {
        Layer layer;
        for (auto value = valueMin; value < valueMax; ++value)
        {
            layer.setLastPage(value);
            QCOMPARE(layer.getLastPage(), value);
        }
    }

    /* fromPDUByteArray <> toPDUByteArray */
    TEST_OTP::HELPER::COMPARE_toFromPDUByteArray(
                DefaultPDUByteArray, Layer(),
                octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getLastPage(), example.second.OTPLayer.lastPage);
    }
}

void TEST_OTP::PDU::OTPLayer::reserved()
{
    const unsigned int octlet = 43;
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
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getReserved(), example.second.OTPLayer.reserved);
    }
}

void TEST_OTP::PDU::OTPLayer::componentName()
{
    const unsigned int octlet = 47;
    const size_t fieldSize = 32;
    QVERIFY(DefaultLayer.getComponentName().size() == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getComponentName(), name_t());
    }

    /* Constructor with options */
    {
        QString value;
        for (size_t n = 0; n <= fieldSize + 1; ++n)
        {
            Layer layer(vector_t(), pduLength_t(), cid_t(), folio_t(), page_t(), page_t(), value);
            QVERIFY(static_cast<size_t>(layer.getComponentName().size()) <= fieldSize);
            QCOMPARE(layer.getComponentName().toString(), QString(value.mid(PDUOctlet, fieldSize)));
            value += char('A') + n;
        }
    }

    /* Constructor from PDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        QString value;
        for (size_t n = 0; n <= fieldSize + 1; ++n)
        {
            PDUByteArray ba; ba << value;
            pdu.replace(octlet - PDUOctlet, fieldSize, ba);
            Layer layer(pdu);
            QVERIFY(static_cast<size_t>(layer.getComponentName().size()) <= fieldSize);
            QCOMPARE(layer.getComponentName().toString(), QString(value.mid(PDUOctlet, fieldSize)));
            value += char('A') + n;
        }
    }

    /* set function */
    {
        Layer layer;
        QString value;
        for (size_t n = 0; n <= fieldSize + 1; ++n)
        {
            layer.setComponentName(value);
            QVERIFY(static_cast<size_t>(layer.getComponentName().size()) <= fieldSize);
            QCOMPARE(layer.getComponentName().toString(), QString(value.mid(PDUOctlet, fieldSize)));
            value += char('A') + n;
        }
    }

    /* fromPDUByteArray <> toPDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        Layer layer(pdu);
        for (auto n = 0; n <= 0xFF; ++n)
        {
            const auto value = QByteArray(fieldSize, char(n));
            pdu.replace(octlet - PDUOctlet, fieldSize, value);
            layer.fromPDUByteArray(pdu);
            QCOMPARE(layer.toPDUByteArray(), pdu);
        }
    }

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        PDUByteArray pdu;
        pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
        Layer layer(pdu);
        QCOMPARE(layer.getComponentName().toString(), example.second.OTPLayer.componentName);
    }
}
