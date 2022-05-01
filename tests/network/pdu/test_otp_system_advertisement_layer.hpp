#ifndef TEST_OTP_SYSTEM_ADVERTISEMENT_LAYER_H
#define TEST_OTP_SYSTEM_ADVERTISEMENT_LAYER_H

#include <QtTest/QTest>

#include "network/pdu/otp_system_advertisement_layer.hpp"
using namespace OTP::PDU::OTPSystemAdvertisementLayer;

namespace TEST_OTP::PDU
{
    class OTPSystemAdvertisementLayer : public QObject
    {
        Q_OBJECT

    public:
        OTPSystemAdvertisementLayer() = default;
        ~OTPSystemAdvertisementLayer() = default;

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

        const OTP::PDU::OTPSystemAdvertisementLayer::Layer DefaultLayer;
        PDUByteArray DefaultPDUByteArray;
    };
}

#endif // TEST_OTP_SYSTEM_ADVERTISEMENT_LAYER_H
