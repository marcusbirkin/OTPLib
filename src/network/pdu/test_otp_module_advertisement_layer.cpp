#include "test_otp_module_advertisement_layer.hpp"
#include "network/messages/test_appendix_B.hpp"
#include "test_otp_helper.hpp"

#include <QRandomGenerator>

const size_t PDUOctlet = 87;
const size_t PDULength_Min = 95 - PDUOctlet + 4;
const size_t PDULength_Max = PDULength_Min + 1376;

int test_otp_module_advertisement_layer(int argc, char *argv[])
{
    TEST_OTP::PDU::OTPModuleAdvertisementLayer testObject;
    return QTest::qExec(&testObject, argc, argv);
}

void TEST_OTP::PDU::OTPModuleAdvertisementLayer::initTestCase()
{
    DefaultPDUByteArray = DefaultLayer.toPDUByteArray();
    QCOMPARE(DefaultPDUByteArray.size(), PDULength_Min - 4);
}

void TEST_OTP::PDU::OTPModuleAdvertisementLayer::isValid()
{
    QVERIFY(!DefaultLayer.isValid());

    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE) {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet));
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

void TEST_OTP::PDU::OTPModuleAdvertisementLayer::toFromPDUByteArray()
{
    {
        Layer layer(DefaultPDUByteArray);
        QCOMPARE(layer.toPDUByteArray(), DefaultPDUByteArray);
    }

    {
        for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
        {
            if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                    && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE
                    && example.second.OTPModuleAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE_LIST)
            {
                PDUByteArray pdu;
                pdu.append(example.first.mid(PDUOctlet));
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
}

void TEST_OTP::PDU::OTPModuleAdvertisementLayer::vector()
{
    const unsigned int octlet = 87;
    const size_t fieldSize = 2;
    const quint16 valueMin = std::numeric_limits<quint16>::min();
    const quint16 valueMax = std::numeric_limits<quint16>::max();
    QVERIFY(sizeof(DefaultLayer.getVector()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getVector(), VECTOR_OTP_ADVERTISEMENT_MODULE_LIST);
        QVERIFY(!layer.isValid());
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
            if (value != VECTOR_OTP_ADVERTISEMENT_MODULE_LIST)
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
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE
                && example.second.OTPModuleAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE_LIST)
        {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getVector(), example.second.OTPModuleAdvertisementLayer.vector);
        }
    }
}

void TEST_OTP::PDU::OTPModuleAdvertisementLayer::length()
{
    const unsigned int octlet = 89;
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
            Layer layer(value, list_t({item_t()}));
            QCOMPARE(layer.getPDULength(), value);
            if (value == layer.toPDUByteArray().size() - LENGTHOFFSET)
                QVERIFY(layer.isValid());
            else
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
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE
                && example.second.OTPModuleAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE_LIST)
        {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getPDULength(), example.second.OTPModuleAdvertisementLayer.length);
        }
    }
}

void TEST_OTP::PDU::OTPModuleAdvertisementLayer::reserved()
{
    const unsigned int octlet = 91;
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
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE
                && example.second.OTPModuleAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE_LIST)
        {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getReserved(), example.second.OTPModuleAdvertisementLayer.reserved);
        }
    }
}

void TEST_OTP::PDU::OTPModuleAdvertisementLayer::list()
{
    typedef OTP::PDU::OTPModuleLayer::manufacturerID_t manufacturerID_t;
    typedef OTP::PDU::OTPModuleLayer::moduleNumber_t moduleNumber_t;

    const unsigned int octlet = 95;
    const size_t fieldSize = 2 + 2;
    const size_t listCountMin = 1;
    const size_t listCountMax = 344;
    QVERIFY(typeid(DefaultLayer.getList().first()) == typeid(item_t));
    QVERIFY(item_t().getSize() == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        if (static_cast<size_t>(layer.getList().count()) < listCountMin)
            QVERIFY(!layer.isValid());
        else
            QVERIFY(layer.isValid());
        QCOMPARE(layer.getList(), list_t());
    }

    /* Constructor with options */
    {
        {
            list_t list;
            for (size_t n = 0; n < listCountMax; ++n) {
                item_t item(
                    QRandomGenerator::global()->bounded(
                        manufacturerID_t::getMin(),
                        manufacturerID_t::getMax() + 1
                    ),
                    QRandomGenerator::global()->bounded(
                        moduleNumber_t::getMin(),
                        moduleNumber_t::getMax() + 1
                    )
                );
                list.append(item);
            }
            Layer layer(pduLength_t(), list);
            std::sort(list.begin(), list.end());
            QVERIFY(layer.getList() == list);
            QVERIFY(!layer.isValid());
        }

        if (manufacturerID_t::getMax() < std::numeric_limits<quint16>::max() ||
                 moduleNumber_t::getMax() < std::numeric_limits<quint16>::max())
        {
            list_t list;
            for (size_t n = 0; n < listCountMax; ++n) {
                item_t item(
                    QRandomGenerator::global()->bounded(
                        manufacturerID_t::getMax(),
                        std::numeric_limits<quint16>::max()
                    ),
                    QRandomGenerator::global()->bounded(
                        moduleNumber_t::getMax(),
                        std::numeric_limits<quint16>::max()
                    )
                );
                list.append(item);
            }
            Layer layer(pduLength_t(), list);
            QVERIFY(!layer.isValid());
        }
    }

    /* Constructor from PDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        for (size_t n = 0; n < listCountMax; ++n) {
            pdu.append(fieldSize, QRandomGenerator::global()->bounded(
                std::numeric_limits<char>::min(),
                std::numeric_limits<char>::max()));
            QVERIFY(pdu.size() >= PDULength_Min && pdu.size() <= PDULength_Max);
        }
        Layer layer(pdu);
        QCOMPARE(layer.toPDUByteArray().mid(octlet), pdu.mid(octlet));
    }

    /* fromPDUByteArray <> toPDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        for (size_t n = 0; n < listCountMax; ++n) {
            pdu.append(fieldSize, QRandomGenerator::global()->bounded(
                std::numeric_limits<char>::min(),
                std::numeric_limits<char>::max()));
            QVERIFY(pdu.size() >= PDULength_Min && pdu.size() <= PDULength_Max);
        }
        Layer layer;
        layer.fromPDUByteArray(pdu);
        QCOMPARE(layer.toPDUByteArray().mid(octlet), pdu.mid(octlet));
    }

    /* set function */
    {
        for (size_t count = 0; count <= listCountMax + 1; ++count) {
            list_t list;
            for (size_t n = 0; n < count; ++n) {
                item_t item(
                    QRandomGenerator::global()->bounded(
                        manufacturerID_t::getMin(),
                        manufacturerID_t::getMax() + 1
                    ),
                    QRandomGenerator::global()->bounded(
                        moduleNumber_t::getMin(),
                        moduleNumber_t::getMax() + 1
                    )
                );
                list.append(item);
            }
            Layer layer;
            if (count >= listCountMin && count <= listCountMax)
                QVERIFY(layer.setList(list));
            else
                QVERIFY(!layer.setList(list));
            std::sort(list.begin(), list.end());
            if (count <= listCountMax)
                QVERIFY(layer.getList() == list);
            else
                QVERIFY(layer.getList() != list);
        }

        if (manufacturerID_t::getMax() < std::numeric_limits<quint16>::max() ||
                 moduleNumber_t::getMax() < std::numeric_limits<quint16>::max())
        {
            for (size_t count = 0; count <= listCountMax + 1; ++count) {
                list_t list;
                for (size_t n = 0; n < count; ++n) {
                    item_t item(
                        QRandomGenerator::global()->bounded(
                            manufacturerID_t::getMax(),
                            std::numeric_limits<quint16>::max()
                        ),
                        QRandomGenerator::global()->bounded(
                            moduleNumber_t::getMax(),
                            std::numeric_limits<quint16>::max()
                        )
                    );
                    list.append(item);
                }
                Layer layer;
                layer.setList(list);
                QVERIFY(!layer.isValid());
            }
        }
    }

    /* add function */
    {
        for (size_t count = 0; count <= listCountMax + 1; ++count) {
            Layer layer;
            for (size_t n = 0; n < count; ++n) {
                item_t item(
                    QRandomGenerator::global()->bounded(
                        manufacturerID_t::getMin(),
                        manufacturerID_t::getMax() + 1
                    ),
                    QRandomGenerator::global()->bounded(
                        moduleNumber_t::getMin(),
                        moduleNumber_t::getMax() + 1
                    )
                );
                if (n < listCountMax) {
                    QVERIFY(layer.addItem(item));
                    QVERIFY(layer.getList().contains(item));
                } else {
                    QVERIFY(!layer.addItem(item));
                    QVERIFY(!layer.getList().contains(item));
                }
            }
        }


        if (manufacturerID_t::getMax() < std::numeric_limits<quint16>::max() ||
                 moduleNumber_t::getMax() < std::numeric_limits<quint16>::max())
        {
            Layer layer;
            for (size_t count = 0; count <= listCountMax + 1; ++count) {
                item_t item(
                    QRandomGenerator::global()->bounded(
                        manufacturerID_t::getMax(),
                        std::numeric_limits<quint16>::max()
                    ),
                    QRandomGenerator::global()->bounded(
                        moduleNumber_t::getMax(),
                        std::numeric_limits<quint16>::max()
                    )
                );
                QVERIFY(!layer.addItem(item));
            }
        }
    }

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE
                && example.second.OTPModuleAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_MODULE_LIST)
        {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet));
            Layer layer(pdu);

            QVERIFY(layer.getList().count() ==
                    example.second.OTPModuleAdvertisementLayer.idents.count());
            for (auto idx = 0; idx < layer.getList().count(); ++idx) {
                QCOMPARE(layer.getList().at(idx).ManufacturerID,
                         example.second.OTPModuleAdvertisementLayer.idents.at(idx).manufacturerID);
                QCOMPARE(layer.getList().at(idx).ModuleNumber,
                         example.second.OTPModuleAdvertisementLayer.idents.at(idx).moduleNumber);
            }
        }
    }
}
