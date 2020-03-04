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
#include "modules.hpp"

using namespace OTP;
using namespace OTP::PDU;
using namespace OTP::MODULES;

STANDARD::additional_t STANDARD::getAdditional(
        OTPModuleLayer::ident_t standardModule,
        pointDetails_t pointDetails)
{
    additional_t additional;
    if (standardModule.ManufacturerID != ESTA_MANUFACTURER_ID) return additional;
    switch (standardModule.ModuleNumber)
    {
        case STANDARD::POSITION:
            additional << pointDetails->standardModules.position;
            return additional;

        case STANDARD::POSITION_VELOCITY_ACCELERATION:
            additional << pointDetails->standardModules.positionVelAcc;
            return additional;

        case STANDARD::ROTATION:
            additional << pointDetails->standardModules.rotation;
            return additional;

        case STANDARD::ROTATION_VELOCITY_ACCELERATION:
            additional << pointDetails->standardModules.rotationVelAcc;
            return additional;

        case STANDARD::ORIENTATION: break;
        case STANDARD::ORIENTATION_VELOCITY_ACCELERATION: break;
    }
    return additional;
}

timestamp_t STANDARD::getTimestamp(
        OTPModuleLayer::ident_t standardModule,
        pointDetails_t pointDetails)
{
    switch (standardModule.ModuleNumber)
    {
        case STANDARD::POSITION: return pointDetails->standardModules.position.getTimestamp();
        case STANDARD::POSITION_VELOCITY_ACCELERATION: return pointDetails->standardModules.positionVelAcc.getTimestamp();
        case STANDARD::ROTATION: return pointDetails->standardModules.rotation.getTimestamp();
        case STANDARD::ROTATION_VELOCITY_ACCELERATION: return pointDetails->standardModules.rotationVelAcc.getTimestamp();
        case STANDARD::ORIENTATION: break;
        case STANDARD::ORIENTATION_VELOCITY_ACCELERATION: break;
    }
    return 0;
}

