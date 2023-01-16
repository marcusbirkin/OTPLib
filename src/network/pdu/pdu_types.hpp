/**
 * @file        pdu_types.hpp
 * @brief       Custom types for OTP PDUs
 * @details     Part of OTPLib - A QT interface for E1.59
 * @authors     Marcus Birkin
 * @copyright   Copyright (C) 2020 Marcus Birkin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef PDU_TYPES_HPP
#define PDU_TYPES_HPP

#include "../../bugs.hpp"
#include <QByteArray>
#include <QUuid>
#include <QtEndian>
#include <QHostAddress>
#include <bitset>

#if defined MAKE_OTP_LIB
    /**
     * @brief Export symbol as shared library
     * 
     */
    #define OTP_LIB_EXPORT Q_DECL_EXPORT
#else
    /**
     * @brief Import symbol from shared library
     * 
     */
    #define OTP_LIB_EXPORT Q_DECL_IMPORT
#endif

namespace OTP::PDU
{
    class PDUByteArray;
    namespace OTPModuleLayer{
        class additional_t;
    }

    /**
     * @brief PDU Length type
     * @details Length of PDU excluding Vector and Length fields
     * 
     */
    typedef quint16 pduLength_t;

    /**
     * @brief Vector type
     * @details It is a key that describes the format of the rest of the information contained in the Data field of that PDU.
     * 
     */
    typedef quint16 vector_t;

    /**
     * @brief Name type
     * @details 
     * Name shall not exceed 32 octets. 
     * Shorter names shall be null-padded to fill out the full field length. 
     * Longer names must be truncated, and shall not break anywhere other than a rune boundary 
     * (e.g., a 33 octet name ending in the rune specified by 0xF09F92A1 would need to be truncated back to the 29th octet, and then followed by 0x000000).
     * 
     */
    class OTP_LIB_EXPORT name_t : public QByteArray
    {
    public:
        name_t();

        /**
         * @brief Construct a new name object
         * 
         * @param ba Name Bytearray
         */
        name_t(const QByteArray &ba);

        /**
         * @brief Construct a new name object
         * 
         * @param s Name string
         */
        name_t(const QString &s);

        /**
         * @brief Get maximum allow size
         * 
         * @return Max byte size
         */
        static int maxSize();

        /**
         * @brief Is the name valid?
         * 
         * @return Validity
         */
        bool isValid() const;

        /**
         * @brief Return the name as normal QString
         * 
         * @return QString
         */
        operator QString() const;

        /**
         * @brief Return the name as normal QString
         * 
         * @return QString
         */
        QString toString() const;

        /**
         * @brief Convert a Qstring to name type
         * 
         * @param s QString to convert
         */
        void fromString(const QString &s);
    };
    PDUByteArray& operator<<(PDUByteArray &l, const name_t &r);
    PDUByteArray& operator>>(PDUByteArray &l, name_t &r);
    inline bool operator==(const name_t &l, const name_t &r) {
        return (static_cast<QString>(l).toUtf8() == static_cast<QString>(r).toUtf8());
    }
    inline bool operator!=(const name_t &l, const name_t &r) { return !(l == r); }



    /**
     * @internal
     * @brief Byte array type
     * @details Packed byte array, primary datatype of undissected packed OTP data
     * Stored in network byte order
     * 
     */
    class PDUByteArray : public QByteArray
    {
    public:
        PDUByteArray() : QByteArray() {}

        /**
         * @brief Return size of array
         * 
         * @return pduLength_t 
         */
        inline pduLength_t size() const {
            return static_cast<pduLength_t>(QByteArray::size());
        }
    };
    PDUByteArray& operator<<(PDUByteArray &l, const quint8 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const quint16 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const quint32 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const quint64 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const qint8 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const qint16 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const qint32 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const qint64 &r);
    PDUByteArray& operator>>(PDUByteArray &l, quint8 &r);
    PDUByteArray& operator>>(PDUByteArray &l, quint16 &r);
    PDUByteArray& operator>>(PDUByteArray &l, quint32 &r);
    PDUByteArray& operator>>(PDUByteArray &l, quint64 &r);
    PDUByteArray& operator>>(PDUByteArray &l, qint8 &r);
    PDUByteArray& operator>>(PDUByteArray &l, qint16 &r);
    PDUByteArray& operator>>(PDUByteArray &l, qint32 &r);
    PDUByteArray& operator>>(PDUByteArray &l, qint64 &r);

    namespace OTPLayer {
        /**
         * @internal
         * @brief OTP Packet Identifier
         * @details 
         * The OTP Packet Identifier shall contain the following 
         * sequence of hexadecimal characters: 0x4f 0x54 0x50 0x2d 0x45 0x31 0x2e 0x35 0x39 0x00 0x00 0x00.
         * 
         */
        class otpIdent_t : public QByteArray
        {
        public:
            otpIdent_t() : QByteArray() {}

            /**
             * @brief Construct a new OTP Packet Identifier object
             * 
             * @param ba Packet Identifier ByteArray
             */
            otpIdent_t(const QByteArray &ba) : QByteArray(ba) {}
        };
        PDUByteArray& operator<<(PDUByteArray &l, const otpIdent_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, otpIdent_t &r);

        /**
         * @brief Component IDenifier
         * @details
         * The OTP Layer contains a Component Identifier (CID). 
         * 
         * This CID shall be a UUID (Universally Unique Identifier) [UUID] 
         * that is a 128-bit number that is unique across space and time, compliant with RFC 4122 [UUID]. 
         * 
         * Each piece of equipment should maintain the same CID for its entire lifetime 
         * (e.g. by storing it in read-only memory). 
         * This means that a particular Component on the network can be identified as the same entity from day to day, 
         * despite network interruptions, power down, or other disruptions. 
         * In the case of a system where volatile Components are created “on the fly,” 
         * it is acceptable to dynamically generate CIDsas required. 
         * 
         */
        class OTP_LIB_EXPORT cid_t : public QUuid
        {
        public:
            cid_t() : QUuid() {}

            /**
             * @brief Construct a new CID object
             * 
             * @param id CID UUID
             */
            cid_t(const QUuid &id) : QUuid(id) {}

            /**
             * @brief Construct a new CID object
             * 
             * @param id CID ByteArray
             */
            explicit cid_t(const QByteArray &id) : QUuid(id) {}

            /**
             * @brief Create a new Component IDenifier
             * 
             * @return New CID 
             */
            static cid_t createUuid();
        };
        PDUByteArray& operator<<(PDUByteArray &l, const cid_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, cid_t &r);

        /**
         * @internal
         * @brief Footer
         * @details 
         * The Footer Options bit field is reserved for future use.
         * The footer that these flags describe is undefined and reserved for future use.
         * 
         */
        class footer_t {
            typedef quint8 flags_t; /**< The Footer Options bit field */
            typedef quint8 length_t; /**< The Footer Length field is an 8-bit field containing the length of the footer in octets */
            typedef QByteArray type;
        public:
            footer_t() : flags(0) {}

            /**
             * @brief Construct a new Footer object
             * 
             * @param flags Flags field
             * @param length Length field
             */
            footer_t(flags_t flags, length_t length = 0) : flags(flags) { footer.resize(length); }

            /**
             * @brief Get footer
             * 
             * @return Footer 
             */
            operator type() const { return footer; }

            /**
             * @brief Get the footer option flags
             * 
             * @return Flags field
             */
            flags_t &Flags() { return flags; }

            /**
             * @copydoc Flags()
             */
            const flags_t &constFlags() const { return flags; }

            /**
             * @brief Get the footer length field
             * 
             * @return Length field 
             */
            length_t getLength() const { return footer.size(); }

            /**
             * @brief Set the footer length field
             * 
             * @param value New length field
             */
            void setLength(length_t value) { footer.resize(value); }
        private:
            flags_t flags;
            type footer;
        };

        /**
         * @brief Folio type
         * 
         * <b>OTP Folio:</b> An OTP Folio is a set of OTP Messages with the same folio number. 
         * Together they make up a snapshot of information which due to its size has to be split across multiple messages.
         * 
         */
        class folio_t {
            typedef quint32 type;
        public:
            folio_t() : data(0) {}

            /**
             * @brief Construct a new Folio object
             * 
             * @param value Folio Number
             */
            folio_t(type value) : data(value) {}

            /**
             * @brief Get Folio number
             * 
             * @return Folio number 
             */
            operator type() const { return data; }

            /**
             * @brief Check valid folio sequence
             * @details Is the new value valid for being next in sequence?
             * 
             * @param value New Folio number to compare
             * @return true Value is next in sequence
             * @return false Value is older than current folio number
             */
            bool checkSequence(folio_t value);

            friend PDUByteArray& operator>>(PDUByteArray &l, folio_t &r);
            folio_t& operator++();
            folio_t operator++(int);
        private:
            type data;
        };

        /**
         * @brief Page type
         * @details 
         * It is possible that a single Producer or Consumer 
         * may need to transmit so much data that it will not fit in a single OTP Layer PDU. 
         * A single Producer may need to transmit many Point PDUs, 
         * a Consumer may support so many Modules that its List of Module Identifiers is too large, 
         * or a Producer may provide object transform data for so many Points that the List of Address Point Descriptions does not fit. 
         * In these cases, the message may be split into multiple Pages. 
         * Data shall not be subdivided in the middle of an entry (a Module Identifier, Address Point Description, Point PDU, etc...) 
         * and may only be split on a boundary.
         * 
         */
        typedef quint16 page_t;

        /**
         * @brief Options type
         * @details Octets are reserved for future use.
         */
        typedef quint8 options_t;

        /**
         * @brief Reserved type
         * @details Octets are reserved for future use.
         */
        typedef quint32 reserved_t;
    }

    namespace OTPTransformLayer {
        /**
         * @brief System number type
         * @details
         * <b>System:</b> A user-defined collection of one or more Groups (OTPPointLayer::group_t).
         * Systems provide part of the method of partitioning network traffic.
         *
         * <b>System Number:</b> Each System is assigned a unique System Number, identifying the System.
         */
        class OTP_LIB_EXPORT system_t
        {
            typedef quint8 type;
        public:
            system_t();

            /**
             * @brief Construct a new System number object
             * 
             * @param value System number
             */
            system_t(type value) : data(value) {}

            /**
             * @overload
             * @copydoc system_t(type value)
             */
            explicit system_t(quint64 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc system_t(type value)
             */
            explicit system_t(quint32 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc system_t(type value)
             */
            explicit system_t(quint16 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc system_t(type value)
             */
            explicit system_t(qint64 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc system_t(type value)
             */
            explicit system_t(qint32 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc system_t(type value)
             */
            explicit system_t(qint16 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc system_t(type value)
             */
            explicit system_t(qint8 value) : data(static_cast<type>(value)) {}

            /**
             * @brief Is the system number valid
             * 
             * @return Validity
             */
            bool isValid() const;

            /**
             * @brief Maximum value allowed by System Number type
             * 
             * @return Maximum value
             */
            static system_t getMin();

            /**
             * @brief Minimum value allowed by System Number type
             * 
             * @return Minimum value
             */
            static system_t getMax();

            /**
             * @brief Get the nunber of bytes used by System number type
             * 
             * @return Bytes used 
             */
            size_t getSize() const { return sizeof(type); }

            /**
             * @brief Get System number
             * 
             * @return System number 
             */
            operator type() const { return data; }

            friend PDUByteArray& operator>>(PDUByteArray &l, system_t &r);
            friend OTPModuleLayer::additional_t& operator>>(OTPModuleLayer::additional_t &l, system_t &r);
            system_t& operator++();
            system_t operator++(int);
            system_t& operator--();
            system_t operator--(int);
        private:
            type data;
        };
        inline Q_IPV6ADDR operator+(Q_IPV6ADDR l, const system_t& r)
        {
            l.c[15] += r;
            return l;
        }
        
        /**
         * @brief Timestamp type
         * @details 
         * The Timestamp is the time, in microseconds since the Time Origin, 
         * that the information in this message was generated, 
         * not the time it is being sent (unless they are identical).
         * 
         */
        typedef quint64 timestamp_t;

        /**
         * @internal
         * @brief Options type
         * @details This bit-oriented field is used to encode flags indicating how to process the remaining data in the PDU.
         * 
         */
        typedef struct options_t
        {
            options_t() : data(0) {}

            /**
             * @brief Construct a new Options object
             * 
             * @param FullPointSet Is layer a full point set?
             */
            options_t(bool FullPointSet) : data(0) {
                setFullPointSet(FullPointSet);
            }

            /**
             * @brief Is layer a full point set
             * @details Indicates that this Transform Layer PDU contains a Full Point Set from this Producer.
             * 
             * @return true Contains a full point set
             * @return false Does not contain a full point set
             */
            bool isFullPointSet() const { return data[FULL_POINT_SET_BIT]; }

            /**
             * @brief Set Full Point Set flag
             * @details Indicates that this Transform Layer PDU contains a Full Point Set from this Producer.
             * 
             * @param value Full Point Set flag
             */
            void setFullPointSet(bool value) { data[FULL_POINT_SET_BIT] = value; }
            
            friend PDUByteArray& operator<<(PDUByteArray &l, const options_t &r) {
                l << type(r.data.to_ulong());
                return l;
            }
            friend PDUByteArray& operator>>(PDUByteArray &l, options_t &r) {
                type temp;
                l >> temp;
                r.data = std::bitset<bitWidth>(temp);
                return l;
            }
        private:
            typedef quint8 type;
            static const quint8 bitWidth = sizeof(type) * 8;
            enum {
                FULL_POINT_SET_BIT = 7
            };
            std::bitset<bitWidth> data;
        } options_t;
        /**
         * @brief Reserved type
         * @details Octets are reserved for future use.
         */
        typedef quint32 reserved_t;
    }

    namespace OTPPointLayer {
        /**
         * @brief Priority number type
         *
         * A Consumer may receive Points with the same Address from multiple Producers,
         * or indeed from a single Producer, within an OTP Folio.
         * The priority field is used by Consumers in selecting between multiple Points with the same Address.
         * Priority increases with numerical value, e.g., 200 is a higher priority than 100. Consumers receiving
         * transform information for multiple Points with the same Address shall treat the Point with the highest
         * priority as definitive for that Address.
         * Priorities shall be in the decimal range 0 through 200.
         * Priorities 201 through 255 are reserved for futureuse. Consumers shall discard any Point with a reserved priority.
         * Producers that do not support variable priority shall transmit a priority of 100.
         */
        class OTP_LIB_EXPORT priority_t
        {
            typedef quint8 type;
        public:
            priority_t() : data(defaultValue) {}
            /**
             * @brief Construct a new Priority number object
             * 
             * @param value Priority number
             */
            priority_t(type value) : data(value) {}

            /**
             * @overload
             * @copydoc priority_t(type value)
             */
            explicit priority_t(quint64 value) : data(static_cast<type>(value)) {}
            
            /**
             * @overload
             * @copydoc priority_t(type value)
             */
            explicit priority_t(quint32 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc priority_t(type value)
             */
            explicit priority_t(qint64 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc priority_t(type value)
             */
            explicit priority_t(qint32 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc priority_t(type value)
             */
            explicit priority_t(qint16 value) : data(static_cast<type>(value)) {}

            /**
             * @brief Is the priority number valid
             * 
             * @return Validity
             */
            bool isValid() const;

            /**
             * @brief Maximum value allowed by Priority Number type
             * 
             * @return Maximum value
             */
            static priority_t getMin();

            /**
             * @brief Minimum value allowed by Priority Number type
             * 
             * @return Minimum value
             */
            static priority_t getMax();

            /**
             * @brief Get the nunber of bytes used by Priority number type
             * 
             * @return Bytes used 
             */
            size_t getSize() const { return sizeof(type); }
            
            /**
             * @brief Get Priority number
             * 
             * @return Priority number 
             */
            operator type() const { return data; }

            friend PDUByteArray& operator>>(PDUByteArray &l, priority_t &r);
            priority_t& operator++();
            priority_t operator++(int);
            priority_t& operator--();
            priority_t operator--(int);
        private:
            static const type defaultValue = 100;
            type data;
        };

        /**
         * @brief Group number type
         * @details
         * <b>Group:</b> A user-defined collection of one or more Points (OTPPointLayer::point_t).
         * A Group could describe a single Object, many Objects, or simply a set of Points with some logical association.
         *
         * <b>Group Number:</b> Each Group is assigned a Group Number,
         * identifying the Group within its System (OTPTransformLayer::system_t).
         * Group Numbers are unique within a System. That is, anything within a single System with the same Group
         * Number belongs to the same Group. In a multi-System implementation, there may be several Groups (a
         * maximum of one per System) with the same Group Number.
         */
        class OTP_LIB_EXPORT group_t
        {
            typedef quint16 type;
        public:
            group_t();

            /**
             * @brief Construct a new Group number object
             * 
             * @param value Group number
             */
            group_t(type value) : data(value) {}

            /**
             * @overload
             * @copydoc group_t(type value)
             */
            explicit group_t(quint64 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc group_t(type value)
             */
            explicit group_t(quint32 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc group_t(type value)
             */
            explicit group_t(qint64 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc group_t(type value)
             */
            explicit group_t(qint32 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc group_t(type value)
             */
            explicit group_t(qint16 value) : data(static_cast<type>(value)) {}

            /**
             * @brief Is the group number valid
             * 
             * @return Validity
             */
            bool isValid() const;

            /**
             * @brief Minimum value allowed by Module Number type
             * 
             * @return Minimum value
             */
            static group_t getMin();

            /**
             * @brief Maximum value allowed by Group Number type
             * 
             * @return Maximum value
             */
            static group_t getMax();

            /**
             * @brief Get the nunber of bytes used by Group number type
             * 
             * @return Bytes used 
             */
            size_t getSize() const { return sizeof(type); }

            /**
             * @brief Get Group number
             * 
             * @return Group number 
             */
            operator type() const { return data; }

            friend PDUByteArray& operator>>(PDUByteArray &l, group_t &r);
            friend OTPModuleLayer::additional_t& operator>>(OTPModuleLayer::additional_t &l, group_t &r);
            group_t& operator++();
            group_t operator++(int);
            group_t& operator--();
            group_t operator--(int);
        private:
            type data;
        };

        /**
         * @brief Point number type
         * @details
         * <b>Point:</b> The smallest, indivisible component having properties of motion.
         * This may be the center of a complex Object, or merely one of many Points on it.
         *
         * <b>Point Number:</b> Each Point is assigned a Point Number, identifying the Point.
         * A Point Number is unique within a Group (OTPPointLayer::group_t).
         * In a multi-Group System (OTPTransformLayer::system_t), or a multi-System implementation,
         * there may be many Points with the same Point Number.
         */
        class OTP_LIB_EXPORT point_t
        {
            typedef quint32 type;
        public:
            point_t();

            /**
             * @brief Construct a new Point number object
             * 
             * @param value Point number
             */
            point_t(type value) : data(value) {}

            /**
             * @overload
             * @copydoc point_t(type value)
             */
            explicit point_t(quint64 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc point_t(type value)
             */
            explicit point_t(qint64 value) : data(static_cast<type>(value)) {}

            /**
             * @overload
             * @copydoc point_t(type value)
             */
            explicit point_t(qint32 value) : data(static_cast<type>(value)) {}

            /**
             * @brief Is the point number valid
             * 
             * @return Validity
             */
            bool isValid() const;

            /**
             * @brief Minimum value allowed by Group number type
             * 
             * @return Minimum value
             */
            static point_t getMin();
            
            /**
             * @brief Maximum value allowed by Group number type
             * 
             * @return Maximum value
             */
            static point_t getMax();

            /**
             * @brief Get the nunber of bytes used by Point number type
             * 
             * @return Bytes used 
             */
            size_t getSize() const { return sizeof(type); }

            /**
             * @brief Get Point number
             * 
             * @return Point number 
             */
            operator type() const { return data; }

            friend PDUByteArray& operator>>(PDUByteArray &l, point_t &r);
            friend OTPModuleLayer::additional_t& operator>>(OTPModuleLayer::additional_t &l, point_t &r);
            point_t& operator++();
            point_t operator++(int);
            point_t& operator--();
            point_t operator--(int);
        private:
            type data;
        };

        /**
         * @brief Timestamp type
         * @details 
         * The Timestamp is the time, in microseconds since the Time Origin, 
         * that the information in this message was sampled from the Point. 
         * Unless identical to the generation time, 
         * it shall not be the same as time that this message was sent
         * 
         */
        typedef quint64 timestamp_t;

        /**
         * @brief Options type
         * @details Octets are reserved for future use.
         */
        typedef quint8 options_t;

        /**
         * @brief Reserved type
         * @details Octets are reserved for future use.
         */
        typedef quint32 reserved_t;
    }

    /**
     * @internal
     * @brief Layer containing module data for a point, nomically this would be locational data from standard modules, but can be manufacturer specfic
     * @details Module data only appears in OTP Transform Messages and shall not be included in OTP Advertisement Messages.
     * Producers shall not include more than one Module of the same type within a single Point PDU.
     * 
     */
    namespace OTPModuleLayer {
        /**
         * @brief Manufacturer ID type
         * @details Identifies the Manufacturer ID of an encapsulated module
         * 
         */
        class manufacturerID_t
        {
            typedef quint16 type;
        public:
            manufacturerID_t() : data(0xFFFF) {}

            /**
             * @brief Construct a new Manufacturer ID object
             * 
             * @param value Manufacturer ID
             */
            manufacturerID_t(type value) : data(value) {}

            /**
             * @copydoc manufacturerID_t(type value)
             */
            explicit manufacturerID_t(quint8 value) : data(static_cast<type>(value)) {}

            /**
             * @brief Is the Manufacturer ID valid
             * @details 
             * Checks that the Manufacturer ID is within range
             * and optionally allows Prototype and unregistered/unknown
             * n.b. A Manufacturer ID could have been registered with ESTA after the release of this build
             * 
             * @param allowPrototype Allow Prototype values
             * @param allowUnkown Allow unregistered/unknown values
             * @return true Manufacturer ID is valid
             * @return false Manufacturer ID is not valid
             */
            bool isValid(bool allowPrototype = true, bool allowUnkown = true) const;

            /**
             * @brief Minimum value allowed by Manufacturer ID type
             * 
             * @return Minimum value
             */
            const static manufacturerID_t getMin() { return std::numeric_limits<type>::min(); };

            /**
             * @brief Maximum value allowed by Manufacturer ID type
             * 
             * @return Maximum value
             */
            const static manufacturerID_t getMax() { return std::numeric_limits<type>::max(); };

            /**
             * @brief Get the nunber of bytes used by Manufacturer ID type
             * 
             * @return Bytes used 
             */
            size_t getSize() const { return sizeof(type); }

            /**
             * @brief Get the Manufacturer ID
             * 
             * @return Manufacturer ID 
             */
            operator type() const { return data; }

            friend PDUByteArray& operator>>(PDUByteArray &l, manufacturerID_t &r);
        private:
            type data;
        };

        /**
         * @brief Module Number type
         * @details The Module Number field is a 16-bit field containing the Module Number identifying the type of module contained.
         * 
         */
        class moduleNumber_t
        {
            typedef quint16 type;
        public:
            constexpr moduleNumber_t() : data(0xFFFF) {}

            /**
             * @brief Construct a new Module Number object
             * 
             * @param value Module Number
             * 
             */
            constexpr moduleNumber_t(type value) : data(value) {}

            /**
             * @brief Construct a new Module Number object
             * 
             * @param value Module Number
             */
            explicit moduleNumber_t(quint8 value) : data(static_cast<type>(value)) {}

            /**
             * @brief Is the Module Number valid
             * 
             * @return Validity
             */
            bool isValid() const;

            /**
             * @brief Minimum value allowed by Module Number type
             * 
             * @return Minimum value
             */
            const static manufacturerID_t getMin() { return std::numeric_limits<type>::min(); };

            /**
             * @brief Maximum value allowed by Module Number type
             * 
             * @return Maximum value
             */
            const static manufacturerID_t getMax() { return std::numeric_limits<type>::max(); };

            /**
             * @brief Get the nunber of bytes used by Module number type
             * 
             * @return Bytes used 
             */
            size_t getSize() const { return sizeof(type); }

            /**
             * @brief Get Module number
             * 
             * @return Module number 
             */
            constexpr operator type() const { return data; }

            friend PDUByteArray& operator>>(PDUByteArray &l, moduleNumber_t &r);
        private:
            type data;
        };

        /**
         * @brief Module idenifier type 
         * @details Combination of Manufacturer ID and Module Number
         * 
         */
        typedef struct ident_t
        {
            ident_t() :
                ManufacturerID(),
                ModuleNumber() {}

            /**
             * @brief Construct a new Module idenifier object
             * 
             * @param ManufacturerID Manufacturer ID of module
             * @param ModuleNumber Module Idenitifer
             */
            ident_t(manufacturerID_t ManufacturerID, moduleNumber_t ModuleNumber) :
                ManufacturerID(ManufacturerID),
                ModuleNumber(ModuleNumber) {}

            /**
             * @brief Is the Module idenifier valid
             * 
             * @return Validity
             */
            bool isValid() const;

            /**
             * @brief Get the nunber of bytes used by Module idenifier type 
             * 
             * @return Bytes used 
             */
            size_t getSize() const;

            manufacturerID_t ManufacturerID; /**< Identifies the Manufacturer ID of an encapsulated module */
            moduleNumber_t ModuleNumber; /**< Identifies the type of the module*/
        } ident_t;
        PDUByteArray& operator<<(PDUByteArray &l, const ident_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, ident_t &r);
        inline bool operator==(const ident_t &l, const ident_t &r) {
            return (
                (l.ModuleNumber == r.ModuleNumber) &&
                (l.ManufacturerID == r.ManufacturerID));
        }
        inline bool operator!=(const ident_t &l, const ident_t &r) { return !(l == r); };
        inline bool operator< (const ident_t& l, const ident_t& r) {
            if (l.ManufacturerID == r.ManufacturerID)
                return l.ModuleNumber < r.ModuleNumber;
            return (l.ManufacturerID < r.ManufacturerID);
        }

        /**
         * @internal
         * @brief Additional Fields Determined by Vector and Module Number
         * @details Packed byte array reprecenting the tail of OTP Module Layer 
         * 
         */
        class additional_t : public QByteArray
        {
        public:
            additional_t() : QByteArray() {}

            /**
             * @brief Construct a new Additional Fields object
             * 
             * @param ba Additional Fields ByteArray
             */
            additional_t(const QByteArray &ba) : QByteArray(ba) {}
        };
        PDUByteArray& operator<<(PDUByteArray &l, const additional_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, additional_t &r);
        additional_t& operator<<(additional_t &l, const quint8 &r);
        additional_t& operator<<(additional_t &l, const quint16 &r);
        additional_t& operator<<(additional_t &l, const quint32 &r);
        additional_t& operator<<(additional_t &l, const quint64 &r);
        additional_t& operator<<(additional_t &l, const qint8 &r);
        additional_t& operator<<(additional_t &l, const qint16 &r);
        additional_t& operator<<(additional_t &l, const qint32 &r);
        additional_t& operator<<(additional_t &l, const qint64 &r);
        additional_t& operator>>(additional_t &l, quint8 &r);
        additional_t& operator>>(additional_t &l, quint16 &r);
        additional_t& operator>>(additional_t &l, quint32 &r);
        additional_t& operator>>(additional_t &l, quint64 &r);
        additional_t& operator>>(additional_t &l, qint8 &r);
        additional_t& operator>>(additional_t &l, qint16 &r);
        additional_t& operator>>(additional_t &l, qint32 &r);
        additional_t& operator>>(additional_t &l, qint64 &r);
    }

    /**
     * @internal
     * @brief Parent Layer containing other advertisement layers
     * @details Advertisement data only appears in OTP Advertisement Messages and shall not be included in OTP Transform Messages.
     * 
     */
    namespace OTPAdvertisementLayer {
        /**
         * @brief Reserved type
         * @details Octets are reserved for future use.
         */
        typedef quint32 reserved_t;
    }

    /**
     * @internal
     * @brief Layer containing module advertisements
     * @details Module Advertisement data only appears in OTP Module Advertisement Messages 
     * and shall not be included in OTP Transform Messages, OTP Name Advertisement Messages, or OTP System Advertisement Messages.
     * 
     */
    namespace OTPModuleAdvertisementLayer {
        /**
         * @brief Reserved type
         * @details Octets are reserved for future use.
         */
        typedef quint32 reserved_t;

        /*! List item type */ 
        typedef OTPModuleLayer::ident_t item_t;

        /*! List type */
        typedef QList<item_t> list_t;

        PDUByteArray& operator<<(PDUByteArray &l, const list_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, list_t &r);
    }

    /**
     * @internal
     * @brief Layer containing name advertisements
     * @details Name Advertisement data only appears in OTP Name Advertisement Messages 
     * and shall not be included in OTP Transform Messages, OTP Module Advertisement Messages, or OTP System Advertisement Messages.
     * 
     */
    namespace OTPNameAdvertisementLayer {
        /**
         * @internal
         * @brief Options type
         * @details This bit-oriented field is used to encode flags indicating how to process the remaining data in the PDU.
         */
        typedef struct options_t
        {
            options_t() : data(0) {}
            /**
             * @brief Indicates that this is a request for Point Names from a Consumer
             * 
             * @return true Is a Consumer request
             * @return false Is not a Consumer request
             */
            bool isRequest() const { return data[REQUEST_RESPONSE_BIT] == REQUEST; }

            /**
             * @brief Set Consumer request bit
             * 
             */
            void setRequest() { data[REQUEST_RESPONSE_BIT] = REQUEST; }

            /**
             * @brief Indicates this is a response to an OTP Name Advertisement Message
             * 
             * @return true Is a response to an Advertisement
             * @return false Is not a response to an Advertisement
             */
            bool isResponse() const { return data[REQUEST_RESPONSE_BIT] == RESPONSE;}

            /**
             * @brief Set OTP Name Advertisement Message response bit
             * 
             */
            void setResponse() { data[REQUEST_RESPONSE_BIT] = RESPONSE; }
            friend PDUByteArray& operator<<(PDUByteArray &l, const options_t &r) {
                l << type(r.data.to_ulong());
                return l;
            }
            friend PDUByteArray& operator>>(PDUByteArray &l, options_t &r) {
                type temp;
                l >> temp;
                r.data = std::bitset<bitWidth>(temp);
                return l;
            }
            friend bool operator==(const options_t &l, const options_t &r) {
                return l.data == r.data;
            }
        private:
            typedef quint8 type;
            static const quint8 bitWidth = sizeof(type) * 8;
            enum {
                REQUEST_RESPONSE_BIT = 7
            };
            enum
            {
                REQUEST = 0,
                RESPONSE = 1
            };
            std::bitset<bitWidth> data;
        } options_t;

        /**
         * @brief Reserved type
         * @details Octets are reserved for future use.
         */
        typedef quint32 reserved_t;

        /**
         * @brief Address Point Descriptions type
         * @details Each Address Point Description shall contain four subfields: System Number, Group Number, Point Number, and Point Name.
         * 
         */
        typedef struct addressPointDescriptions_t
        {
            addressPointDescriptions_t() :
                System(0),
                Group(0),
                Point(0),
                PointName(QString("")) {}

            /**
             * @brief Construct a new Address Point Descriptions object
             * 
             * @param System System number of Group
             * @param Group Group number of Point
             * @param Point Point Number of Name
             * @param PointName Point Name
             */
            addressPointDescriptions_t(
                    OTPTransformLayer::system_t System,
                    OTPPointLayer::group_t Group,
                    OTPPointLayer::point_t Point,
                    name_t PointName) :
                System(System),
                Group(Group),
                Point(Point),
                PointName(PointName) {}

            /**
             * @brief Is the Address Point Descriptions valid
             * 
             * @return Validity
             */
            bool isValid() const;

            /**
             * @brief Get the nunber of bytes used by Address Point Descriptions type
             * 
             * @return Bytes used 
             */
            size_t getSize() const;

            OTPTransformLayer::system_t System; /**< System Number of Group */
            OTPPointLayer::group_t Group; /**< Group Number of Point */
            OTPPointLayer::point_t Point; /**< Point Number of Name */
            name_t PointName; /**< Point Name */
        } addressPointDescriptions_t;
        PDUByteArray& operator<<(PDUByteArray &l, const addressPointDescriptions_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, addressPointDescriptions_t &r);
        inline bool operator==(const addressPointDescriptions_t &l, const addressPointDescriptions_t &r) {
            PDUByteArray la, ra;
            la << l;
            ra << r;
            return la == ra;
        }
        inline bool operator!=(const addressPointDescriptions_t &l, const addressPointDescriptions_t &r) {
            return !(l == r);
        }
        inline bool operator<(const addressPointDescriptions_t &l, const addressPointDescriptions_t &r) {
            return (
              l.System < r.System &&
              l.Group < r.Group &&
              l.Point < r.Point
            );
        }
        inline bool operator>(const addressPointDescriptions_t &l, const addressPointDescriptions_t &r) {
            return (
              l.System > r.System &&
              l.Group > r.Group &&
              l.Point > r.Point
            );
        }

        /*! List item type */ 
        typedef addressPointDescriptions_t item_t;

        /*! List type*/
        typedef QList<item_t> list_t;

        PDUByteArray& operator<<(PDUByteArray &l, const list_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, list_t &r);
    }

    /**
     * @internal
     * @brief Layer containing system advertisements
     * @details System Advertisement data only appears in OTP System Advertisement Messages
     * and shall not be included in OTP Transform Messages, OTP Module Advertisement Messages, or OTP Name Advertisement Messages.
     * 
     */
    namespace OTPSystemAdvertisementLayer {
        /*! Option field type */
        typedef OTPNameAdvertisementLayer::options_t options_t;

        /**
         * @brief Reserved type
         * @details Octets are reserved for future use.
         */
        typedef quint32 reserved_t;

        /*! List item type */ 
        typedef OTPTransformLayer::system_t item_t;

        /*! List type*/
        typedef QList<item_t> list_t;

        PDUByteArray& operator<<(PDUByteArray &l, const list_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, list_t &r);
     }
};

#endif // PDU_TYPES_HPP
