#!/usr/bin/env python

# Live copy of ESTA Control Protocols Working Group Manufacturer IDs
url = 'https://tsp.esta.org/tsp/working_groups/CP/mfctrIDs.php'
# Location of C++ header file output
filename = '../src/esta_mfcrids.hpp'

# Download the live list
def getESTAmfctrIDs():
    import unicodedata
    import urllib.request
    print('Reading' , url)
    user_agent = 'Mozilla/5.0 (Windows NT 6.1; Win64; x64)' # Fake user agent to prevent 403 Forbidden
    headers = {'User-Agent': user_agent}
    req = urllib.request.Request(url, {}, headers)
    with urllib.request.urlopen(req) as response:
       return response.read()

def main():
    from datetime import timezone, datetime
    from bs4 import BeautifulSoup
    soup = BeautifulSoup(getESTAmfctrIDs(), 'html.parser')
    table = soup.find("table", attrs={"id":"main_table"})

    headers = [th.get_text() for th in table.find("tr").find_all("th")]

    # Begin c++ header
    print('Writing', filename)
    headerFile = open(filename, 'w')
    headerFile.write('/*' + '\r\n')
    headerFile.write(' * Auto generated' + '\r\n')
    headerFile.write(' * from: ' + url + '\r\n')
    headerFile.write(' * at: ' + datetime.now().astimezone(timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ') + '\r\n')
    headerFile.write('*/' + '\r\n')
    headerFile.write('\r\n')
    headerFile.write('#ifndef ESTA_MFCTRIDS_HPP' + '\r\n')
    headerFile.write('#define ESTA_MFCTRIDS_HPP' + '\r\n')
    headerFile.write('\r\n')
    headerFile.write('#include <map>' + '\r\n')
    headerFile.write('\r\n')
    headerFile.write('namespace ESTA {' + '\r\n')
    headerFile.write('\t' + 'namespace ManufacturerIDs {' + '\r\n')

    # Prototyping and experimental
    headerFile.write('\t\t' + 'static const uint16_t PROTOTYPE_BEGIN = 0x7FF0;' + '\r\n')
    headerFile.write('\t\t' + 'static const uint16_t PROTOTYPE_END = 0x7FFF;' + '\r\n')
    headerFile.write('\r\n')

    # std::map of <Manufacturer ID, Company String>
    headerFile.write('\t\t' + 'static const std::map<uint16_t, std::string> Manufacturers = {' + '\r\n')
    for row in table.find_all("tr")[1:]:
        data = [td.get_text() for td in row.find_all("td")]

        mfctrId = data[0].strip()
        mfctrId = '0x' + mfctrId[:-1] # Remove trailing 'h', and prepend '0x'

        company = data[2].strip()
        company = company.replace('"', '\'')

        headerFile.write('\t\t\t' + '{' + mfctrId + ', "' + company + '"},' + '\r\n')

    headerFile.write('\t\t' + '}; // std::map' + '\r\n')

    headerFile.write('\t' + '} // ManufacturerIDs' + '\r\n')
    headerFile.write('} // ESTA' + '\r\n')
    headerFile.write('#endif // ESTA_MFCTRIDS_HPP' + '\r\n')
    headerFile.close()
    # End c++ header

    print('Finished')

if __name__ == "__main__":
    main()
