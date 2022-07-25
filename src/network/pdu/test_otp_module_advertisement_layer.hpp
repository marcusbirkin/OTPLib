#ifndef TEST_OTP_MODULE_ADVERTISEMENT_LAYER_H
#define TEST_OTP_MODULE_ADVERTISEMENT_LAYER_H

#include <QtTest/QTest>

#include "network/pdu/otp_module_advertisement_layer.hpp"
using namespace OTP::PDU::OTPModuleAdvertisementLayer;

namespace TEST_OTP::PDU
{
    class OTPModuleAdvertisementLayer : public QObject
    {
        Q_OBJECT

    public:
        OTPModuleAdvertisementLayer() = default;
        ~OTPModuleAdvertisementLayer() = default;

    private slots:
        void initTestCase();

        void isValid();
        void toFromPDUByteArray();
        void vector();
        void length();
        void reserved();
        void list();

    private:
        const OTP::PDU::OTPModuleAdvertisementLayer::Layer DefaultLayer;
        PDUByteArray DefaultPDUByteArray;
    };
}

#endif // TEST_OTP_MODULE_ADVERTISEMENT_LAYER_H
