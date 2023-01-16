/**
 * @file        otp.cpp
 * @brief       OTPLib - A QT interface for E1.59
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
#include "otp.hpp"

namespace OTPLib {
    QString getVersion()
    {
        return QString(VER_PRODUCTVERSION_STR);
    }

    QString getAbout()
    {
        return QString(
                    "<style> h1,h2,h3,h4,h5 { text-align: center; }</style>"
                    "<p><h1>%1</h1>"
                    "<h2>Version: %2</h2>"
                    "<h3>A QT library for ANSI E1.59:2021</h3>"
                    "<h4>Entertainment Technology Object Transform Protocol (OTP)</h4>"
                    "<h5>%3<br>"
                    "<a href=\"https://github.com/marcusbirkin/OTPView\">https://github.com/marcusbirkin/OTPView</a></h5></p>")
                .arg(QString("OTPLib"), getVersion(), VER_LEGALCOPYRIGHT_STR)
                .append(
                   "<p>This program is free software: you can redistribute it and/or modify "
                   "it under the terms of the GNU Lesser General Public License as published by "
                   "the Free Software Foundation, either version 3 of the License, or "
                   "(at your option) any later version.</p>"
                   "<p>This program is distributed in the hope that it will be useful, "
                   "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
                   "GNU General Public License for more details."
                   "<p>You should have received a copy of the GNU Lesser General Public License "
                   "along with this program. If not, see <a href=\"https://www.gnu.org/licenses/\">https://www.gnu.org/licenses/</a>.</p>"
                   );
    }
}
