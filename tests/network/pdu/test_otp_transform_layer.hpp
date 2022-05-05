#ifndef TEST_OTP_TRANSFORM_LAYER_H
#define TEST_OTP_TRANSFORM_LAYER_H

#include <QtTest/QTest>

#include "network/pdu/otp_transform_layer.hpp"
using namespace OTP::PDU::OTPTransformLayer;

namespace TEST_OTP::PDU
{
    class OTPTransformLayer : public QObject
    {
        Q_OBJECT

    public:
        OTPTransformLayer() = default;
        ~OTPTransformLayer() = default;

    private slots:
        void initTestCase();

        void isValid();
        void toFromPDUByteArray();
        void vector();
        void length();
        void system();
        void timestamp();
        void options();
        void reserved();

    private:
        const OTP::PDU::OTPTransformLayer::Layer DefaultLayer;
        PDUByteArray DefaultPDUByteArray;
    };
}

#endif // TEST_OTP_TRANSFORM_LAYER_H
