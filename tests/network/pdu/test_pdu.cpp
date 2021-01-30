#include <QtTest/QTest>
#include "test_otp_layer.hpp"
#include "test_otp_transform_layer.hpp"

int main(int argc, char *argv[])
{
    int status = 0;

    {
        TEST_OTP::PDU::OTPLayer testObject;
        status |= QTest::qExec(&testObject, argc, argv);
    }

    {
        TEST_OTP::PDU::OTPTransformLayer testObject;
        status |= QTest::qExec(&testObject, argc, argv);
    }

    return status;
}
