#ifndef TEST_OTP_ADVERTISEMENT_LAYER_H
#define TEST_OTP_ADVERTISEMENT_LAYER_H

#include <QtTest/QTest>

#include "network/pdu/otp_advertisement_layer.hpp"
using namespace OTP::PDU::OTPAdvertisementLayer;

namespace TEST_OTP::PDU
{
    class OTPAdvertisementLayer : public QObject
    {
        Q_OBJECT

    public:
        OTPAdvertisementLayer() = default;
        ~OTPAdvertisementLayer() = default;

    private slots:
        void initTestCase();

        void isValid();
        void toFromPDUByteArray();
        void vector();
        void length();
        void reserved();

    private:
        template <typename T>
        void helper_toFromPDUByteArray(unsigned int octlet,
                size_t fieldSize,
                T valueMin,
                T valueMax,
                T forStep = 1);

        const OTP::PDU::OTPAdvertisementLayer::Layer DefaultLayer;
        PDUByteArray DefaultPDUByteArray;
    };
}

#endif // TEST_OTP_ADVERTISEMENT_LAYER_H
