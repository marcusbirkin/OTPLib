#include <QtTest/QTest>
#include "test_otp_layer.hpp"

int main(int argc, char *argv[])
{
    int status = 0;

    {
        TEST_OTP::PDU::OTPLayer testObject;
        status |= QTest::qExec(&testObject, argc, argv);
    }

    return status;
}
