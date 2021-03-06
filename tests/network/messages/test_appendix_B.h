#ifndef TEST_APPENDIX_B_H
#define TEST_APPENDIX_B_H

#include <QtGlobal>
#include <QString>
#include <QSet>
#include "network/pdu/pdu_const.hpp"

namespace TEST_OTP::MESSAGES::APPENDIX_B {
    typedef struct example_t {
        QString TableName;
        struct OTPLayer{
            quint16 vector;
            quint16 length;
            quint8 footerOptions;
            quint8 footerLength;
            QUuid cid;
            quint32 folio;
            quint16 page;
            quint16 lastPage;
            quint8 options;
            quint32 reserved;
            QString componentName;
        } OTPLayer;
    } exampleDetails_t;

    // Table B-1 Transform Message Example
    static const unsigned char ExampleB_1[] = {
        /* OTP Packet Identifier */ 0x4f,0x54,0x50,0x2d,0x45,0x31,0x2e,0x35,0x39,0x00,0x00,0x00,
        /* Vector */ 0xff,0x01, // VECTOR_OTP_TRANSFORM_MESSAGE
        /* Length */ 0x00,0xB9, // 185
        /* Footer Options */ 0x00,
        /* Footer Length */ 0x00,
        /* CID */ 0x4d,0x6f,0x76,0x65,0x73,0x20,0x40,0x39,0xb0,0x20,0x6f,0x62,0x6a,0x65,0x63,0x74,
        /* Folio Number */ 0x00,0x00,0x1,0x46, // 326
        /* Page */ 0x00,0x01,
        /* Last Page */ 0x00,0x01,
        /* Options */ 0x00,
        /* Reserved */ 0x00,0x00,0x00,0x00,
        /* Component Name */
            0x41,0x75,0x74,0x6f,0x6d,0x61,0x74,0x69,0x6f,0x6e,
            0x2d,0x53,0x65,0x72,0x76,0x65,0x72,0x2d,0x50,0x72,
            0x69,0x6d,0x61,0x72,0x79,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00 // Automation-Server-Primary
    };

    // Table B-2: System Advertisement Message Consumer Example
    static const unsigned char ExampleB_2[] = {
        /* OTP Packet Identifier */ 0x4f,0x54,0x50,0x2d,0x45,0x31,0x2e,0x35,0x39,0x00,0x00,0x00,
        /* Vector */ 0xff,0x02, // VECTOR_OTP_ADVERTISEMENT_MESSAGE
        /* Length */ 0x00,0x50, // 80
        /* Footer Options */ 0x00,
        /* Footer Length */ 0x00,
        /* CID */ 0x54,0x69,0x6c,0x74,0x73,0x20,0x40,0x35,0xb0,0x20,0x6c,0x69,0x67,0x68,0x74,0x73,
        /* Folio Number */ 0x00,0x00,0x18,0x94, // 6292
        /* Page */ 0x00,0x01,
        /* Last Page */ 0x00,0x01,
        /* Options */ 0x00,
        /* Reserved */ 0x00,0x00,0x00,0x00,
        /* Component Name */
            0x4c,0x69,0x67,0x68,0x74,0x69,0x6e,0x67,0x2d,0x43,
            0x6f,0x6e,0x73,0x6f,0x6c,0x65,0x2d,0x50,0x72,0x69,
            0x6d,0x61,0x72,0x79,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00 // Lighting-Console-Primary
    };

    // Table B-3: System Advertisement Message Producer Example
    static const unsigned char ExampleB_3[] = {
        /* OTP Packet Identifier */ 0x4f,0x54,0x50,0x2d,0x45,0x31,0x2e,0x35,0x39,0x00,0x00,0x00,
        /* Vector */ 0xff,0x02, // VECTOR_OTP_ADVERTISEMENT_MESSAGE
        /* Length */ 0x00,0x52, // 82
        /* Footer Options */ 0x00,
        /* Footer Length */ 0x00,
        /* CID */ 0x4d,0x6f,0x76,0x65,0x73,0x20,0x40,0x39,0xb0,0x20,0x6f,0x62,0x6a,0x65,0x63,0x74,
        /* Folio Number */ 0x00,0x00,0x00,0x64, // 100
        /* Page */ 0x00,0x01,
        /* Last Page */ 0x00,0x01,
        /* Options */ 0x00,
        /* Reserved */ 0x00,0x00,0x00,0x00,
        /* Component Name */
            0x41,0x75,0x74,0x6f,0x6d,0x61,0x74,0x69,0x6f,0x6e,
            0x2d,0x53,0x65,0x72,0x76,0x65,0x72,0x2d,0x50,0x72,
            0x69,0x6d,0x61,0x72,0x79,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00 // Automation-Server-Primary
    };

    // Table B-4: Name Advertisement Message Consumer Example
    static const unsigned char ExampleB_4[] = {
        /* OTP Packet Identifier */ 0x4f,0x54,0x50,0x2d,0x45,0x31,0x2e,0x35,0x39,0x00,0x00,0x00,
        /* Vector */ 0xff,0x02, // VECTOR_OTP_ADVERTISEMENT_MESSAGE
        /* Length */ 0x00,0x50, // 80
        /* Footer Options */ 0x00,
        /* Footer Length */ 0x00,
        /* CID */ 0x54,0x69,0x6c,0x74,0x73,0x20,0x40,0x35,0xb0,0x20,0x6c,0x69,0x67,0x68,0x74,0x73,
        /* Folio Number */ 0x00,0x00,0x01,0x90, // 400
        /* Page */ 0x00,0x01,
        /* Last Page */ 0x00,0x01,
        /* Options */ 0x00,
        /* Reserved */ 0x00,0x00,0x00,0x00,
        /* Component Name */
            0x4c,0x69,0x67,0x68,0x74,0x69,0x6e,0x67,0x2d,0x43,
            0x6f,0x6e,0x73,0x6f,0x6c,0x65,0x2d,0x50,0x72,0x69,
            0x6d,0x61,0x72,0x79,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00 // Lighting-Console-Primary
    };

    // Table B-5: Name Advertisement Message Producer Example
    static const unsigned char ExampleB_5[] = {
        /* OTP Packet Identifier */ 0x4f,0x54,0x50,0x2d,0x45,0x31,0x2e,0x35,0x39,0x00,0x00,0x00,
        /* Vector */ 0xff,0x02, // VECTOR_OTP_ADVERTISEMENT_MESSAGE
        /* Length */ 0x00,0xEC, // 236
        /* Footer Options */ 0x00,
        /* Footer Length */ 0x00,
        /* CID */ 0x4d,0x6f,0x76,0x65,0x73,0x20,0x40,0x39,0xb0,0x20,0x6f,0x62,0x6a,0x65,0x63,0x74,
        /* Folio Number */ 0x00,0x00,0x0B,0xB8, // 3000
        /* Page */ 0x00,0x01,
        /* Last Page */ 0x00,0x01,
        /* Options */ 0x00,
        /* Reserved */ 0x00,0x00,0x00,0x00,
        /* Component Name */
            0x41,0x75,0x74,0x6f,0x6d,0x61,0x74,0x69,0x6f,0x6e,
            0x2d,0x53,0x65,0x72,0x76,0x65,0x72,0x2d,0x50,0x72,
            0x69,0x6d,0x61,0x72,0x79,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00 // Automation-Server-Primary
    };

    // Table B-6: Module Advertisement Message Example
    static const unsigned char ExampleB_6[] = {
        /* OTP Packet Identifier */ 0x4f,0x54,0x50,0x2d,0x45,0x31,0x2e,0x35,0x39,0x00,0x00,0x00,
        /* Vector */ 0xff,0x02, // VECTOR_OTP_ADVERTISEMENT_MESSAGE
        /* Length */ 0x00,0x57, // 87
        /* Footer Options */ 0x00,
        /* Footer Length */ 0x00,
        /* CID */ 0x54,0x69,0x6c,0x74,0x73,0x20,0x40,0x35,0xb0,0x20,0x6c,0x69,0x67,0x68,0x74,0x73,
        /* Folio Number */ 0x00,0x00,0x02,0x59, // 601
        /* Page */ 0x00,0x01,
        /* Last Page */ 0x00,0x01,
        /* Options */ 0x00,
        /* Reserved */ 0x00,0x00,0x00,0x00,
        /* Component Name */
            0x4c,0x69,0x67,0x68,0x74,0x69,0x6e,0x67,0x2d,0x43,
            0x6f,0x6e,0x73,0x6f,0x6c,0x65,0x2d,0x50,0x72,0x69,
            0x6d,0x61,0x72,0x79,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00 // Lighting-Console-Primary
    };

    const QList<std::pair<QByteArray,exampleDetails_t>> Examples = {
        {
            QByteArray::fromRawData(reinterpret_cast<const char*>(ExampleB_1), sizeof(ExampleB_1)),
            {
                .TableName = "Table B-1: Transform Message Example",
                .OTPLayer = {
                    .vector = VECTOR_OTP_TRANSFORM_MESSAGE,
                    .length = 185,
                    .footerOptions = 0,
                    .footerLength = 0,
                    .cid = QUuid(0x4d6f7665, 0x7320, 0x4039, 0xb0, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74),
                    .folio = 326,
                    .page = 1,
                    .lastPage = 1,
                    .options = 0,
                    .reserved = 0,
                    .componentName = "Automation-Server-Primary",
                }
            }
        },
        {
            QByteArray::fromRawData(reinterpret_cast<const char*>(ExampleB_2), sizeof(ExampleB_2)),
            {
                .TableName = "Table B-2: System Advertisement Message Consumer Example",
                .OTPLayer = {
                    .vector = VECTOR_OTP_ADVERTISEMENT_MESSAGE,
                    .length = 80,
                    .footerOptions = 0,
                    .footerLength = 0,
                    .cid = QUuid(0x54696c74, 0x7320, 0x4035, 0xb0, 0x20, 0x6c, 0x69, 0x67, 0x68, 0x74, 0x73),
                    .folio = 6292,
                    .page = 1,
                    .lastPage = 1,
                    .options = 0,
                    .reserved = 0,
                    .componentName = "Lighting-Console-Primary",
                }
            }
        },
        {
            QByteArray::fromRawData(reinterpret_cast<const char*>(ExampleB_3), sizeof(ExampleB_3)),
            {
                .TableName = "Table B-3: System Advertisement Message Producer Example",
                .OTPLayer = {
                    .vector = VECTOR_OTP_ADVERTISEMENT_MESSAGE,
                    .length = 82,
                    .footerOptions = 0,
                    .footerLength = 0,
                    .cid = QUuid(0x4d6f7665, 0x7320, 0x4039, 0xb0, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74),
                    .folio = 100,
                    .page = 1,
                    .lastPage = 1,
                    .options = 0,
                    .reserved = 0,
                    .componentName = "Automation-Server-Primary",
                }
            }
        },
        {
            QByteArray::fromRawData(reinterpret_cast<const char*>(ExampleB_4), sizeof(ExampleB_4)),
            {
                .TableName = "Table B-4: Name Advertisement Message Consumer Example",
                .OTPLayer = {
                    .vector = VECTOR_OTP_ADVERTISEMENT_MESSAGE,
                    .length = 80,
                    .footerOptions = 0,
                    .footerLength = 0,
                    .cid = QUuid(0x54696c74, 0x7320, 0x4035, 0xb0, 0x20, 0x6c, 0x69, 0x67, 0x68, 0x74, 0x73),
                    .folio = 400,
                    .page = 1,
                    .lastPage = 1,
                    .options = 0,
                    .reserved = 0,
                    .componentName = "Lighting-Console-Primary",
                }
            }
        },
        {
            QByteArray::fromRawData(reinterpret_cast<const char*>(ExampleB_5), sizeof(ExampleB_5)),
            {
                .TableName = "Table B-5: Name Advertisement Message Producer Example",
                .OTPLayer = {
                    .vector = VECTOR_OTP_ADVERTISEMENT_MESSAGE,
                    .length = 236,
                    .footerOptions = 0,
                    .footerLength = 0,
                    .cid = QUuid(0x4d6f7665, 0x7320, 0x4039, 0xb0, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74),
                    .folio = 3000,
                    .page = 1,
                    .lastPage = 1,
                    .options = 0,
                    .reserved = 0,
                    .componentName = "Automation-Server-Primary",
                }
            }
        },
        {
            QByteArray::fromRawData(reinterpret_cast<const char*>(ExampleB_6), sizeof(ExampleB_6)),
            {
                .TableName = "Table B-6: Module Advertisement Message Example",
                .OTPLayer = {
                    .vector = VECTOR_OTP_ADVERTISEMENT_MESSAGE,
                    .length = 87,
                    .footerOptions = 0,
                    .footerLength = 0,
                    .cid = QUuid(0x54696c74, 0x7320, 0x4035, 0xb0, 0x20, 0x6c, 0x69, 0x67, 0x68, 0x74, 0x73),
                    .folio = 601,
                    .page = 1,
                    .lastPage = 1,
                    .options = 0,
                    .reserved = 0,
                    .componentName = "Lighting-Console-Primary",
                }
            }
        }
    };
}
#endif // TEST_APPENDIX_B_H
