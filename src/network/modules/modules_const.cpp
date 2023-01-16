/**
 * @file        modules_const.cpp
 * @brief       Constants used relating to modules in the OTP standard
 * @details     Part of OTPLib - A QT interface for E1.59
 * @authors     Marcus Birkin
 * @copyright   Copyright (C) 2019 Marcus Birkin
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
#include "modules_const.hpp"
#include "esta_mfcrids.hpp"

namespace OTP::MODULES {

    namespace STANDARD {
        namespace VALUES {
            namespace UNITS {
                QString getUnitString(moduleValue_t value, bool html)
                {
                    QString ret = "";
                    if (html) {
                        if (html::baseUnits.contains(value)) ret = html::baseUnits.value(value);
                    } else {
                        if (unicode::baseUnits.contains(value)) ret = unicode::baseUnits.value(value);
                    }
                    return ret;
                }

                QString getScaleString(PositionModule_t::scale_t value, bool html)
                {
                    QString ret = "";
                    if (html) {
                        if (html::scale.contains(value)) ret = html::scale.value(value);
                    } else {
                        if (unicode::scale.contains(value)) ret = unicode::scale.value(value);
                    }
                    return ret;
                }
            }

            namespace RANGES {
                range_t getRange(moduleValue_t value)
                {
                    range_t ret;
                    if (ranges.contains(value)) ret = ranges.value(value);
                    return ret;
                }
            }
        }
    }

    MESSAGES::OTPModuleAdvertisementMessage::list_t const getSupportedModules()
    {
        MESSAGES::OTPModuleAdvertisementMessage::list_t ret;
        ret.append(STANDARD::modules.value(STANDARD::POSITION).ident);
        ret.append(STANDARD::modules.value(STANDARD::POSITION_VELOCITY_ACCELERATION).ident);
        ret.append(STANDARD::modules.value(STANDARD::ROTATION).ident);
        ret.append(STANDARD::modules.value(STANDARD::ROTATION_VELOCITY_ACCELERATION).ident);
        ret.append(STANDARD::modules.value(STANDARD::SCALE).ident);
        ret.append(STANDARD::modules.value(STANDARD::REFERENCE_FRAME).ident);
        return ret;
    }

    module_t::moduleDescription_t const getModuleDescription(PDU::OTPModuleLayer::ident_t ident)
    {
        module_t::moduleDescription_t ret;
        ret.Name = QObject::tr("Unknown");
        ret.Manufactuer = QObject::tr("Unknown");

        using namespace ESTA::ManufacturerIDs;
        const auto it = Manufacturers.find(ident.ManufacturerID);
        if (it != Manufacturers.end())
            ret.Manufactuer = QString::fromStdWString(it->second);

        switch (ident.ManufacturerID)
        {
            case ESTA_MANUFACTURER_ID:
            {
                if (STANDARD::modules.contains(ident.ModuleNumber))
                    ret.Name = STANDARD::modules.value(ident.ModuleNumber).description.Name;
            } break;

            default: break;
        }

        return ret;
    }
}
