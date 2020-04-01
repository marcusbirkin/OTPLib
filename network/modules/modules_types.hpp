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

#include "bugs.hpp"
#include <QDateTime>
#include <bitset>
#include "../messages/message_types.hpp"
#include "../pdu/pdu_types.hpp"

using namespace OTP::PDU;
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
        PDU::OTPModuleLayer::ident_t ident;
    } module_t;
    typedef PDU::OTPModuleLayer::ident_t::moduleNumber_t moduleNumber_t;

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
            typedef struct options_s
            {
                options_s() : data(0) {}
                bool isScaling() const { return data[SCALING_BIT]; }
                void setScaling(bool value) { data[SCALING_BIT] = value; }
                friend additional_t& operator<<(additional_t &l, const options_s &r) {
                    l << type(r.data.to_ulong());
                    return l;
                }
                friend additional_t& operator>>(additional_t &l, options_s &r) {
                    type temp;
                    l >> temp;
                    r.data = std::bitset<bitWidth>(temp);
                    return l;
                }
                options_s& operator=(const options_s& r) {this->data = r.data; return *this; }
                options_s& operator=(const unsigned int& r) {this->data = static_cast<type>(r); return *this; }
            private:
                typedef quint8 type;
                static const quint8 bitWidth = sizeof(type) * 8;
                enum {
                    SCALING_BIT = 7
                };
                std::bitset<bitWidth> data;
            } options_t;
            typedef qint32 position_t;
            typedef enum scale_e : quint8
            {
                mm = 0, // Millimeters (mm)
                um = 1 // Micrometers (μm)
            } scale_t;

            PositionModule_t() : options(), timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(position), std::end(position), 0);
            }

            PositionModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                PositionModule_t temp;
                additional >> temp;
                options = temp.options;
                std::copy(std::begin(temp.position), std::end(temp.position), std::begin(position));
            }

            timestamp_t getTimestamp() const { return timestamp; }
            QDateTime getLastSeen() const { return lastSeen; }

            void setScaling(scale_t scale) {
                switch (scale)
                {
                    case mm: options.setScaling(true); break;
                    case um: options.setScaling(false); break;
                }
                updateLastSeen();
            }
            scale_t getScaling() const { return options.isScaling() ? mm : um; }
            bool isScalingMM() const { return getScaling() == mm; }
            bool isScalingUM() const { return getScaling() == um; }

            position_t getPosition(axis_t axis) const { return position[std::clamp(axis, axis_t::first, axis_t::last)]; }
            void setPosition(axis_t axis, position_t value, timestamp_t time = 0)
            {
                position[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                timestamp = time;
                updateLastSeen();
            }

            friend additional_t& operator<<(additional_t &l, const PositionModule_t &r)
            {
                return l << r.options << r.position[axis_e::X] << r.position[axis_e::Y] << r.position[axis_e::Z];
            }
            friend additional_t& operator>>(additional_t &l, PositionModule_t &r)
            {
                l >> r.options >> r.position[axis_e::X] >> r.position[axis_e::Y] >> r.position[axis_e::Z];
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
            friend PositionModule_t& operator+=(PositionModule_t& l, const PositionModule_t& r) {
                for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                {
                    if (l.getScaling() == r.getScaling())
                    {
                        l.position[axis] += r.position[axis];
                    } else {
                        switch (l.getScaling())
                        {
                            case mm: l.position[axis] += (r.position[axis] * 1000); break;
                            case um: l.position[axis] += (r.position[axis] / 1000); break;
                        }
                    }
                }
                return l;
            }
        private:
            options_t options;
            position_t position[axis_t::count];
            timestamp_t timestamp = 0;
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
            friend PositionVelAccModule_t& operator+=(PositionVelAccModule_t& l, const PositionVelAccModule_t& r) {
                for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                {
                    l.velocity[axis] += r.velocity[axis];
                    l.acceleration[axis] += r.acceleration[axis];
                }
                return l;
            }

        private:
            velocity_t velocity[axis_t::count];
            acceleration_t acceleration[axis_t::count];
            timestamp_t timestamp = 0;
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
            friend RotationModule_t& operator+=(RotationModule_t& l, const RotationModule_t& r) {
                for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                    l.rotation[axis] += r.rotation[axis];
                return l;
            }

        private:
            rotation_t rotation[axis_t::count];
            timestamp_t timestamp = 0;
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
            friend RotationVelAccModule_t& operator+=(RotationVelAccModule_t& l, const RotationVelAccModule_t& r) {
                for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                {
                    l.velocity[axis] += r.velocity[axis];
                    l.acceleration[axis] += r.acceleration[axis];
                }
                return l;
            }

        private:
            velocity_t velocity[axis_t::count];
            acceleration_t acceleration[axis_t::count];
            timestamp_t timestamp = 0;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };

        // 16.5 Scale
        class ScaleModule_t {
        public:
            typedef qint32 scale_t;

            ScaleModule_t() : timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(scale), std::end(scale), 1);
            }

            ScaleModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                ScaleModule_t temp;
                additional >> temp;
                std::copy(std::begin(temp.scale), std::end(temp.scale), std::begin(scale));
            }

            timestamp_t getTimestamp() const { return timestamp; }
            QDateTime getLastSeen() const { return lastSeen; }

            scale_t getScale(axis_t axis) const { return scale[std::clamp(axis, axis_t::first, axis_t::last)]; }
            void setScale(axis_t axis, scale_t value, timestamp_t time = 0)
            {
                scale[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                timestamp = time;
                updateLastSeen();
            }

            friend additional_t& operator<<(additional_t &l, const ScaleModule_t &r)
            {
                l << r.scale[axis_e::X] << r.scale[axis_e::Y] << r.scale[axis_e::Z];
                return l;
            }
            friend additional_t& operator>>(additional_t &l, ScaleModule_t &r)
            {
                l >> r.scale[axis_e::X] >> r.scale[axis_e::Y] >> r.scale[axis_e::Z];
                return l;
            }
            friend bool operator==(const ScaleModule_t& l, const ScaleModule_t& r)
            {
                additional_t la, ra;
                la << l;
                ra << r;
                return (ra == la);
            }
            friend bool operator!=(const ScaleModule_t& l, const ScaleModule_t& r) { return !(l == r); }

        private:
            scale_t scale[axis_t::count];
            timestamp_t timestamp = 0;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };

        // 16.6 Parent Module
        class ParentModule_t {
        public:
            typedef struct options_s
            {
                options_s() : data(0) {}
                bool isRelative() const { return data[RELATIVE_BIT]; }
                void setRelative(bool value) { data[RELATIVE_BIT] = value; }
                friend additional_t& operator<<(additional_t &l, const options_s &r) {
                    l << type(r.data.to_ulong());
                    return l;
                }
                friend additional_t& operator>>(additional_t &l, options_s &r) {
                    type temp;
                    l >> temp;
                    r.data = std::bitset<bitWidth>(temp);
                    return l;
                }
                options_s& operator=(const options_s& r) {this->data = r.data; return *this; }
                options_s& operator=(const unsigned int& r) {this->data = static_cast<type>(r); return *this; }
            private:
                typedef quint8 type;
                static const quint8 bitWidth = sizeof(type) * 8;
                enum {
                    RELATIVE_BIT = 7
                };
                std::bitset<bitWidth> data;
            } options_t;
            typedef OTPTransformLayer::system_t system_t;
            typedef OTPPointLayer::point_t point_t;
            typedef OTPPointLayer::group_t group_t;

            ParentModule_t() : timestamp(0), lastSeen(QDateTime()) { }

            ParentModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                ParentModule_t temp;
                additional >> temp;
                options = temp.options;
                system = temp.system;
                group = temp.group;
                point = temp.point;
            }

            timestamp_t getTimestamp() const { return timestamp; }
            QDateTime getLastSeen() const { return lastSeen; }

            void setRelative(bool value, timestamp_t time) {
                options.setRelative(value);
                timestamp = time;
                updateLastSeen();
            }
            bool isRelative() const { return options.isRelative(); }

            system_t getSystem() const { return system; }
            void setSystem(system_t value, timestamp_t time)
            {
                system = value;
                timestamp = time;
                updateLastSeen();
            }

            group_t getGroup() const { return group; }
            void setGroup(group_t value, timestamp_t time)
            {
                group = value;
                timestamp = time;
                updateLastSeen();
            }

            point_t getPoint() const { return point; }
            void setPoint(point_t value, timestamp_t time)
            {
                point = value;
                timestamp = time;
                updateLastSeen();
            }

            friend additional_t& operator<<(additional_t &l, const ParentModule_t &r)
            {
                return l << r.options << r.system << r.group << r.point;
            }
            friend additional_t& operator>>(additional_t &l, ParentModule_t &r)
            {
                l >> r.options >> r.system >> r.group >> r.point;
                return l;
            }
            friend bool operator==(const ParentModule_t& l, const ParentModule_t& r)
            {
                additional_t la, ra;
                la << l;
                ra << r;
                return (ra == la);
            }
            friend bool operator!=(const ParentModule_t& l, const ParentModule_t& r) { return !(l == r); }

        private:
            options_t options;
            system_t system;
            group_t group;
            point_t point;
            timestamp_t timestamp = 0;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };
    }
}

#endif // MODULES_TYPES_HPP
