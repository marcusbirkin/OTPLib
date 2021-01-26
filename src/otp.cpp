/*
    OTPLib
    A QT interface for E1.59 (Entertainment Technology Object Transform Protocol (OTP))
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
#include "otp.hpp"

namespace OTPLib {
    QString getVersion()
    {
        return QString("%1 (%2 %3 %4)")
            .arg(OTPLib_GIT_CURRENT_SHA1, OTPLib_GIT_DATE_DATE, OTPLib_GIT_DATE_MONTH, OTPLib_GIT_DATE_YEAR);
    }

    QString getAbout()
    {
        return QString(
                   "<style> h1,h2,h3,h4 { text-align: center; }</style>"
                   "<p><h1>%1</h1>"
                   "<h2>Version: %2</h2>"
                   "<h3>A QT interface for E1.59 (Entertainment Technology Object Transform Protocol (OTP))</h3>"
                   "<h4>Marcus Birkin 2019-2020</h4>"
                   "<h4><a href=\"https://github.com/marcusbirkin/OTPLib\">https://github.com/marcusbirkin/OTPLib</a></h4></p>")
               .arg(QString("OTPLib"))
               .arg(getVersion())
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
