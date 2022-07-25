#ifndef TEST_OTP_HELPER_H
#define TEST_OTP_HELPER_H

#include "network/pdu/pdu_types.hpp"

namespace TEST_OTP::HELPER
{
    template <typename T, typename TLayer>
    void COMPARE_toFromPDUByteArray(
            const OTP::PDU::PDUByteArray &DefaultPDUByteArray,
            const TLayer &,
            unsigned int octlet,
            size_t fieldSize,
            T valueMin,
            T valueMax,
            T forStep = 1)
    {
        using namespace OTP::PDU;
        PDUByteArray pdu(DefaultPDUByteArray);
        TLayer layer(pdu);
        for (auto value = valueMin; value < valueMax; value += forStep)
        {
            PDUByteArray ba; ba << value;
            pdu.replace(octlet, static_cast<int>(fieldSize), ba);
            layer.fromPDUByteArray(pdu);
            QCOMPARE(layer.toPDUByteArray(), pdu);
        }
    }
}


#endif // TEST_OTP_HELPER_H
