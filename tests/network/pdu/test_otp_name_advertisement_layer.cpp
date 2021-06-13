#include "test_otp_name_advertisement_layer.hpp"
#include "test_appendix_B.h"

#include <QRandomGenerator>

const size_t PDUOctlet = 87;
const size_t PDULength_Min = 96 - PDUOctlet;
const size_t PDULength_Max = PDULength_Min + 1365;

void TEST_OTP::PDU::OTPNameAdvertisementLayer::initTestCase()
{
    DefaultPDUByteArray = DefaultLayer.toPDUByteArray();
    QCOMPARE(DefaultPDUByteArray.size(), PDULength_Min);
}

template <typename T>
void TEST_OTP::PDU::OTPNameAdvertisementLayer::helper_toFromPDUByteArray(unsigned int octlet,
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

void TEST_OTP::PDU::OTPNameAdvertisementLayer::isValid()
{
    QVERIFY(!DefaultLayer.isValid());

    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME) {
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

void TEST_OTP::PDU::OTPNameAdvertisementLayer::toFromPDUByteArray()
{
    {
        Layer layer(DefaultPDUByteArray);
        QCOMPARE(layer.toPDUByteArray(), DefaultPDUByteArray);
    }

    {
        for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
        {
            if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                    && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME
                    && example.second.OTPNameAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME_LIST)
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

void TEST_OTP::PDU::OTPNameAdvertisementLayer::vector()
{
    const unsigned int octlet = 87;
    const size_t fieldSize = 2;
    const quint16 valueMin = std::numeric_limits<quint16>::min();
    const quint16 valueMax = std::numeric_limits<quint16>::max();
    QVERIFY(sizeof(DefaultLayer.getVector()) == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QCOMPARE(layer.getVector(), VECTOR_OTP_ADVERTISEMENT_NAME_LIST);
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
            if (value != VECTOR_OTP_ADVERTISEMENT_NAME_LIST)
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
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME
                && example.second.OTPNameAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME_LIST)
        {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getVector(), example.second.OTPNameAdvertisementLayer.vector);
        }
    }
}

void TEST_OTP::PDU::OTPNameAdvertisementLayer::length()
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
            Layer layer(value);
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
    helper_toFromPDUByteArray(octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME
                && example.second.OTPNameAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME_LIST)
        {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getPDULength(), example.second.OTPNameAdvertisementLayer.length);
        }
    }
}

void TEST_OTP::PDU::OTPNameAdvertisementLayer::options()
{
    const unsigned int octlet = 91;
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
        QVERIFY(layer.getOptions().isResponse() == false);
        QVERIFY(layer.getOptions().isRequest() == true);
    }

    /* Constructor with options */
    {
        options_t options;
        {
            options.setRequest();
            Layer layer(pduLength_t(), options);
            QVERIFY(layer.getOptions() == options);
            QVERIFY(layer.getOptions().isRequest() == true);
            QVERIFY(layer.getOptions().isResponse() == false);
        }
        {
            options.setResponse();
            Layer layer(pduLength_t(), options);
            QVERIFY(layer.getOptions() == options);
            QVERIFY(layer.getOptions().isRequest() == false);
            QVERIFY(layer.getOptions().isResponse() == true);
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
    {
        {
            Layer layer;
            options_t options;
            options.setRequest();
            layer.setOptions(options);
            QVERIFY(layer.getOptions() == options);
            QVERIFY(layer.getOptions().isRequest() == true);
            QVERIFY(layer.getOptions().isResponse() == false);
        }
        {
            Layer layer;
            options_t options;
            options.setResponse();
            layer.setOptions(options);
            QVERIFY(layer.getOptions() == options);
            QVERIFY(layer.getOptions().isRequest() == false);
            QVERIFY(layer.getOptions().isResponse() == true);
        }
    }

    /* fromPDUByteArray <> toPDUByteArray */
    helper_toFromPDUByteArray(octlet - PDUOctlet, fieldSize, valueMin, valueMax);

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME
                && example.second.OTPNameAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME_LIST)
        {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getOptions().isResponse(), example.second.OTPNameAdvertisementLayer.option_Response);
        }
    }
}

void TEST_OTP::PDU::OTPNameAdvertisementLayer::reserved()
{
    const unsigned int octlet = 92;
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
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME
                && example.second.OTPNameAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME_LIST)
        {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet, DefaultPDUByteArray.size()));
            Layer layer(pdu);
            QCOMPARE(layer.getReserved(), example.second.OTPNameAdvertisementLayer.reserved);
        }
    }
}

void TEST_OTP::PDU::OTPNameAdvertisementLayer::list()
{
    typedef OTP::PDU::OTPTransformLayer::system_t system_t;
    typedef OTP::PDU::OTPPointLayer::group_t group_t;
    typedef OTP::PDU::OTPPointLayer::point_t point_t;

    const unsigned int octlet = 96;
    const size_t fieldSize = 1 + 2 + 4 + 32;
    const size_t listCountMin = 0;
    const size_t listCountMax = 35;
    QVERIFY(typeid(DefaultLayer.getList().first()) == typeid(item_t));
    QVERIFY(item_t().getSize() == fieldSize);

    /* Default constructor*/
    {
        Layer layer;
        QVERIFY(layer.getList().count() == listCountMin);
        QCOMPARE(layer.getList(), list_t());
    }

    /* Constructor with options */
    {
        list_t list;
        for (size_t n = 0; n <= listCountMax; ++n) {
            item_t item(
                QRandomGenerator::global()->bounded(
                    system_t::getMin(),
                    system_t::getMax()
                ),
                QRandomGenerator::global()->bounded(
                    group_t::getMin(),
                    group_t::getMax()
                ),
                QRandomGenerator::global()->bounded(
                    point_t::getMin(),
                    point_t::getMax()
                ),
                QString(Q_FUNC_INFO)
            );
            list.append(item);
        }
        Layer layer(pduLength_t(), options_t(), list);
        std::sort(list.begin(), list.end());
        QVERIFY(layer.getList() == list);
    }

    /* Constructor from PDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        for (size_t n = 0; n < listCountMax -1; ++n)
            pdu.append(fieldSize, QRandomGenerator::global()->bounded(
                std::numeric_limits<char>::min(),
                std::numeric_limits<char>::max()));
        Layer layer(pdu);
        QVERIFY(layer.toPDUByteArray().mid(octlet) != pdu.mid(octlet));
        options_t options;
        options.setResponse();
        layer.setOptions(options);
        QCOMPARE(layer.toPDUByteArray().mid(octlet), pdu.mid(octlet));
    }

    /* fromPDUByteArray <> toPDUByteArray */
    {
        PDUByteArray pdu(DefaultPDUByteArray);
        for (size_t n = 0; n <= listCountMax; ++n)
            pdu.append(fieldSize, QRandomGenerator::global()->bounded(
                std::numeric_limits<char>::min(),
                std::numeric_limits<char>::max()));
        Layer layer;
        layer.fromPDUByteArray(pdu);
        QVERIFY(layer.toPDUByteArray().mid(octlet) != pdu.mid(octlet));
        options_t options;
        options.setResponse();
        layer.setOptions(options);
        QCOMPARE(layer.toPDUByteArray().mid(octlet), pdu.mid(octlet));

    }

    /* set function */
    {
        for (size_t count = 0; count <= listCountMax + 1; ++count) {
            list_t list;
            for (size_t n = 0; n < count; ++n) {
                item_t item(
                    QRandomGenerator::global()->bounded(
                        system_t::getMin(),
                        system_t::getMax()
                    ),
                    QRandomGenerator::global()->bounded(
                        group_t::getMin(),
                        group_t::getMax()
                    ),
                    QRandomGenerator::global()->bounded(
                        point_t::getMin(),
                        point_t::getMax()
                    ),
                    QString(Q_FUNC_INFO)
                );
                list.append(item);
            }
            Layer layer;
            if (count <= listCountMax)
                QVERIFY(layer.setList(list));
            else
                QVERIFY(!layer.setList(list));
            std::sort(list.begin(), list.end());
            if (count <= listCountMax)
                QVERIFY(layer.getList() == list);
            else
                QVERIFY(layer.getList() != list);
        }
    }

    /* add function */
    {
        for (size_t count = 0; count <= listCountMax + 1; ++count) {
            Layer layer;
            for (size_t n = 0; n < count; ++n) {
                item_t item(
                    QRandomGenerator::global()->bounded(
                        system_t::getMin(),
                        system_t::getMax()
                    ),
                    QRandomGenerator::global()->bounded(
                        group_t::getMin(),
                        group_t::getMax()
                    ),
                    QRandomGenerator::global()->bounded(
                        point_t::getMin(),
                        point_t::getMax()
                    ),
                    QString(Q_FUNC_INFO)
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
    }

    /* Examples */
    for (const auto &example : TEST_OTP::MESSAGES::APPENDIX_B::Examples)
    {
        if (example.second.OTPLayer.vector == VECTOR_OTP_ADVERTISEMENT_MESSAGE
                && example.second.OTPAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME
                && example.second.OTPNameAdvertisementLayer.vector == VECTOR_OTP_ADVERTISEMENT_NAME_LIST)
        {
            PDUByteArray pdu;
            pdu.append(example.first.mid(PDUOctlet));
            Layer layer(pdu);

            QVERIFY(layer.getList().count() ==
                    example.second.OTPNameAdvertisementLayer.descriptions.count());
            for (auto idx = 0; idx < layer.getList().count(); ++idx) {
                QCOMPARE(layer.getList().at(idx).System,
                         example.second.OTPNameAdvertisementLayer.descriptions.at(idx).system);
                QCOMPARE(layer.getList().at(idx).Group,
                         example.second.OTPNameAdvertisementLayer.descriptions.at(idx).group);
                QCOMPARE(layer.getList().at(idx).Point,
                         example.second.OTPNameAdvertisementLayer.descriptions.at(idx).point);
                QCOMPARE(layer.getList().at(idx).PointName.toString(),
                         example.second.OTPNameAdvertisementLayer.descriptions.at(idx).description);
            }

        }
    }
}
