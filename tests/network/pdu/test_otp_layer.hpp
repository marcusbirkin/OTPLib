#ifndef TEST_OTP_LAYER_H
#define TEST_OTP_LAYER_H

#include <QtTest/QTest>

#include "network/pdu/otp_layer.hpp"
using namespace OTP::PDU::OTPLayer;

namespace TEST_OTP::PDU
{
    class OTPLayer : public QObject
    {
        Q_OBJECT

    public:
        OTPLayer() = default;
        ~OTPLayer() = default;

    private slots:
        void initTestCase();

        void isValid();
        void toFromPDUByteArray();
        void packetIdent();
        void vector();
        void length();
        void footerOptions();
        void footerLength();
        void cid();
        void folio();
        void page();
        void lastPage();
        void reserved();
        void componentName();

    private:
        const Layer DefaultLayer;
        PDUByteArray DefaultPDUByteArray;
    };
}

#endif // TEST_OTP_LAYER_H
