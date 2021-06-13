#ifndef TEST_OTP_NAME_ADVERTISEMENT_LAYER_H
#define TEST_OTP_NAME_ADVERTISEMENT_LAYER_H

#include <QtTest/QTest>

#include "network/pdu/otp_name_advertisement_layer.hpp"
using namespace OTP::PDU::OTPNameAdvertisementLayer;

namespace TEST_OTP::PDU
{
    class OTPNameAdvertisementLayer : public QObject
    {
        Q_OBJECT

    public:
        OTPNameAdvertisementLayer() = default;
        ~OTPNameAdvertisementLayer() = default;

    private slots:
        void initTestCase();

        void isValid();
        void toFromPDUByteArray();
        void vector();
        void length();
        void options();
        void reserved();
        void list();

    private:
        template <typename T>
        void helper_toFromPDUByteArray(unsigned int octlet,
                size_t fieldSize,
                T valueMin,
                T valueMax,
                T forStep = 1);

        const OTP::PDU::OTPNameAdvertisementLayer::Layer DefaultLayer;
        PDUByteArray DefaultPDUByteArray;
    };
}

#endif // TEST_OTP_NAME_ADVERTISEMENT_LAYER_H
