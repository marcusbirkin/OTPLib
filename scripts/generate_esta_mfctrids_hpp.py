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
    headerFile = open(filename, 'w', encoding="utf-8-sig")
    headerFile.write('/*' + '\n')
    headerFile.write(' * Auto generated' + '\n')
    headerFile.write(' * from: ' + url + '\n')
    headerFile.write(' * at: ' + datetime.now().astimezone(timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ') + '\n')
    headerFile.write('*/' + '\n')
    headerFile.write('\n')
    headerFile.write('#ifndef ESTA_MFCTRIDS_HPP' + '\n')
    headerFile.write('#define ESTA_MFCTRIDS_HPP' + '\n')
    headerFile.write('\n')
    headerFile.write('#include <map>' + '\n')
    headerFile.write('#include <string>' + '\n')
    headerFile.write('\n')
    headerFile.write('namespace ESTA {' + '\n')
    headerFile.write('\t' + 'namespace ManufacturerIDs {' + '\n')

    # Prototyping and experimental
    headerFile.write('\t\t' + 'const uint16_t PROTOTYPE_BEGIN = 0x7FF0;' + '\n')
    headerFile.write('\t\t' + 'const uint16_t PROTOTYPE_END = 0x7FFF;' + '\n')
    headerFile.write('\n')

    # std::map of <Manufacturer ID, Company String>
    headerFile.write('\t\t' + 'using namespace std::string_literals;' + '\n')
    headerFile.write('\t\t' + 'const std::map<uint16_t, std::wstring> Manufacturers = {' + '\n')
    for row in table.find_all("tr")[1:]:
        data = [td.get_text() for td in row.find_all("td")]

        mfctrId = data[0].strip()
        mfctrId = '0x' + mfctrId[:-1] # Remove trailing 'h', and prepend '0x'

        company = data[2].strip()
        company = company.replace('"', '\'')

        headerFile.write('\t\t\t' + '{' + mfctrId + ', L"' + company + '"s},' + '\n')

    headerFile.write('\t\t' + '}; // std::map' + '\n')

    headerFile.write('\t' + '} // ManufacturerIDs' + '\n')
    headerFile.write('} // ESTA' + '\n')
    headerFile.write('#endif // ESTA_MFCTRIDS_HPP' + '\n')
    headerFile.close()
    # End c++ header

    print('Finished')

if __name__ == "__main__":
    main()
