/*
    OTPLib
    A QT interface for E1.59  (Entertainment  Technology  Object  Transform  Protocol  (OTP))
    Copyright (C) 2019  Marcus Birkin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef MODULES_TYPES_HPP
#define MODULES_TYPES_HPP

#include <QDateTime>
#include "../messages/message_types.hpp"
#include "../pdu/pdu_types.hpp"

namespace OTP::MODULES {
    typedef OTP::PDU::OTPTransformLayer::timestamp_t timestamp_t;

    typedef struct
    {
        typedef struct
        {
            QString Manufactuer;
            QString Name;
        } moduleDescription_t;
        moduleDescription_t description;
        PDU::OTPModuleLayer::vector_t vector;
    } module_t;
    typedef PDU::OTPModuleLayer::vector_t::moduleNumber_t moduleNumber_t;

    /* Section 16 Standard Modules */
    namespace STANDARD {
        typedef OTP::PDU::OTPModuleLayer::additional_t additional_t;

        typedef enum axis_e
        {
            first,
            X = first,
            Y,
            Z,
            last = Z,
            count
        } axis_t;
        inline axis_e operator++(axis_e& a,int){
            axis_e temp=a;
            a = std::clamp(static_cast<axis_t>(static_cast<unsigned int>(a) + 1), axis_t::first, axis_t::count);
            return temp;
        }
        inline axis_e operator++ (axis_e& a) {
            return a++;
        }

        // 16.1 Position Module
        class PositionModule_t {
        public:
            typedef quint8 options_t;
            typedef qint32 location_t;
            typedef enum scale_e : quint8
            {
                mm = 0, // Millimeters (mm)
                um = 1 // Micrometers (μm)
            } scale_t;

            PositionModule_t() : options(0), timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(location), std::end(location), 0);
            }

            PositionModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                PositionModule_t temp;
                additional >> temp;
                options = temp.options;
                std::copy(std::begin(temp.location), std::end(temp.location), std::begin(location));
            }

            timestamp_t getTimestamp() const { return timestamp; }
            QDateTime getLastSeen() const { return lastSeen; }

            void setScaling(scale_t scale) {
                switch (scale)
                {
                    case mm: options &= ~SCALING_MASK; break;
                    case um: options |= SCALING_MASK; break;
                }
                updateLastSeen();
            }
            scale_t getScaling() const { return isScalingMM() ? mm : um; }
            bool isScalingMM() const { return ((options & SCALING_MASK) >> SCALING_SHIFT) == mm; }
            bool isScaleingUM() const { return ((options & SCALING_MASK) >> SCALING_SHIFT) == um; }

            location_t getLocation(axis_t axis) const { return location[std::clamp(axis, axis_t::first, axis_t::last)]; }
            void setLocation(axis_t axis, location_t value, timestamp_t time = 0)
                {
                    location[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                    timestamp = time;
                    updateLastSeen();
                }

            friend additional_t& operator<<(additional_t &l, const PositionModule_t &r)
            {
                return l << r.options << r.location[axis_e::X] << r.location[axis_e::Y] << r.location[axis_e::Z];
            }
            friend additional_t& operator>>(additional_t &l, PositionModule_t &r)
            {
                l >> r.options >> r.location[axis_e::X] >> r.location[axis_e::Y] >> r.location[axis_e::Z];
                return l;
            }
            friend bool operator==(const PositionModule_t& l, const PositionModule_t& r)
                {
                    additional_t la, ra;
                    la << l;
                    ra << r;
                    return (ra == la);
                }
            friend bool operator!=(const PositionModule_t& l, const PositionModule_t& r) { return !(l == r); }

        private:
            static constexpr quint8 SCALING_SHIFT = 7;
            static constexpr quint8 SCALING_MASK = 1 << SCALING_SHIFT;
            options_t options;
            location_t location[axis_t::count];
            timestamp_t timestamp;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };

        // 16.2 Position Velocity/Acceleration Module
        class PositionVelAccModule_t {
        public:
            typedef qint32 velocity_t;
            typedef qint32 acceleration_t;

            PositionVelAccModule_t() : timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(velocity), std::end(velocity), 0);
                std::fill(std::begin(acceleration), std::end(acceleration), 0);
            }

            PositionVelAccModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                PositionVelAccModule_t temp;
                additional >> temp;
                std::copy(std::begin(temp.velocity), std::end(temp.velocity), std::begin(velocity));
                std::copy(std::begin(temp.acceleration), std::end(temp.acceleration), std::begin(acceleration));
            }

            timestamp_t getTimestamp() const { return timestamp; }
            QDateTime getLastSeen() const { return lastSeen; }

            velocity_t getVelocity(axis_t axis) const { return velocity[std::clamp(axis, axis_t::first, axis_t::last)]; }
            void setVelocity(axis_t axis, velocity_t value, timestamp_t time = 0)
                {
                    velocity[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                    timestamp = time;
                    updateLastSeen();
                }

            acceleration_t getAcceleration(axis_t axis) const { return acceleration[std::clamp(axis, axis_t::first, axis_t::last)]; }
            void setAcceleration(axis_t axis, acceleration_t value, timestamp_t time = 0)
                {
                    acceleration[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                    timestamp = time;
                    updateLastSeen();
                }

            friend additional_t& operator<<(additional_t &l, const PositionVelAccModule_t &r)
            {
                l << r.velocity[axis_e::X] << r.velocity[axis_e::Y] << r.velocity[axis_e::Z];
                return l << r.acceleration[axis_e::X] << r.acceleration[axis_e::Y] << r.acceleration[axis_e::Z];
            }
            friend additional_t& operator>>(additional_t &l, PositionVelAccModule_t &r)
            {
                l >> r.velocity[axis_e::X] >> r.velocity[axis_e::Y] >> r.velocity[axis_e::Z];
                l >> r.acceleration[axis_e::X] >> r.acceleration[axis_e::Y] >> r.acceleration[axis_e::Z];
                return l;
            }
            friend bool operator==(const PositionVelAccModule_t& l, const PositionVelAccModule_t& r)
                {
                    additional_t la, ra;
                    la << l;
                    ra << r;
                    return (ra == la);
                }
            friend bool operator!=(const PositionVelAccModule_t& l, const PositionVelAccModule_t& r) { return !(l == r); }

        private:
            velocity_t velocity[axis_t::count];
            acceleration_t acceleration[axis_t::count];
            timestamp_t timestamp;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };

        // 16.3 Rotation Module
        class RotationModule_t {
        public:
            typedef quint32 rotation_t;

            RotationModule_t() : timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(rotation), std::end(rotation), 0);
            }

            RotationModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                RotationModule_t temp;
                additional >> temp;
                std::copy(std::begin(temp.rotation), std::end(temp.rotation), std::begin(rotation));
            }

            timestamp_t getTimestamp() const { return timestamp; }
            QDateTime getLastSeen() const { return lastSeen; }

            rotation_t getRotation(axis_t axis) const { return rotation[std::clamp(axis, axis_t::first, axis_t::last)]; }
            void setRotation(axis_t axis, rotation_t value, timestamp_t time = 0)
                {
                    rotation[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                    timestamp = time;
                    updateLastSeen();
                }

            friend additional_t& operator<<(additional_t &l, const RotationModule_t &r)
            {
                return l << r.rotation[axis_e::X] << r.rotation[axis_e::Y] << r.rotation[axis_e::Z];
            }
            friend additional_t& operator>>(additional_t &l, RotationModule_t &r)
            {
                l >> r.rotation[axis_e::X] >> r.rotation[axis_e::Y] >> r.rotation[axis_e::Z];
                return l;
            }
            friend bool operator==(const RotationModule_t& l, const RotationModule_t& r)
                {
                    additional_t la, ra;
                    la << l;
                    ra << r;
                    return (ra == la);
                }
            friend bool operator!=(const RotationModule_t& l, const RotationModule_t& r) { return !(l == r); }

        private:
            rotation_t rotation[axis_t::count];
            timestamp_t timestamp;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };

        // 16.4 Rotation Velocity/Acceleration Module
        class RotationVelAccModule_t {
        public:
            typedef qint32 velocity_t;
            typedef qint32 acceleration_t;

            RotationVelAccModule_t() : timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(velocity), std::end(velocity), 0);
                std::fill(std::begin(acceleration), std::end(acceleration), 0);
            }

            RotationVelAccModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                RotationVelAccModule_t temp;
                additional >> temp;
                std::copy(std::begin(temp.velocity), std::end(temp.velocity), std::begin(velocity));
                std::copy(std::begin(temp.acceleration), std::end(temp.acceleration), std::begin(acceleration));
            }

            timestamp_t getTimestamp() const { return timestamp; }
            QDateTime getLastSeen() const { return lastSeen; }

            velocity_t getVelocity(axis_t axis) const { return velocity[std::clamp(axis, axis_t::first, axis_t::last)]; }
            void setVelocity(axis_t axis, velocity_t value, timestamp_t time = 0)
                {
                    velocity[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                    timestamp = time;
                    updateLastSeen();
                }

            acceleration_t getAcceleration(axis_t axis) const { return acceleration[std::clamp(axis, axis_t::first, axis_t::last)]; }
            void setAcceleration(axis_t axis, acceleration_t value, timestamp_t time = 0)
                {
                    acceleration[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                    timestamp = time;
                    updateLastSeen();
                }

            friend additional_t& operator<<(additional_t &l, const RotationVelAccModule_t &r)
            {
                l << r.velocity[axis_e::X] << r.velocity[axis_e::Y] << r.velocity[axis_e::Z];
                return l << r.acceleration[axis_e::X] << r.acceleration[axis_e::Y] << r.acceleration[axis_e::Z];
            }
            friend additional_t& operator>>(additional_t &l, RotationVelAccModule_t &r)
            {
                l >> r.velocity[axis_e::X] >> r.velocity[axis_e::Y] >> r.velocity[axis_e::Z];
                l >> r.acceleration[axis_e::X] >> r.acceleration[axis_e::Y] >> r.acceleration[axis_e::Z];
                return l;
            }
            friend bool operator==(const RotationVelAccModule_t& l, const RotationVelAccModule_t& r)
                {
                    additional_t la, ra;
                    la << l;
                    ra << r;
                    return (ra == la);
                }
            friend bool operator!=(const RotationVelAccModule_t& l, const RotationVelAccModule_t& r) { return !(l == r); }

        private:
            velocity_t velocity[axis_t::count];
            acceleration_t acceleration[axis_t::count];
            timestamp_t timestamp;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };
    }
}

#endif // MODULES_TYPES_HPP
