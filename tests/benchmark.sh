#!/bin/bash
set -e

echo -e "[+] Installing dependencies...\n"
#sudo apt-get install python3-pip
pip3 install requests

echo -e "\n[i] Starting basic benchmark tests\n"

echo -e "  => Running: time python3 -c \"import requests; requests.get('https://www.facebook.com')\""
time python3 -c "import requests; requests.get('https://www.facebook.com')"

echo -e "\n  => Running: time python3 -c \"import fastrequest; fastrequest.http_get('https://www.facebook.com')\""
time python3 -c "import fastrequest; fastrequest.http_get('https://www.facebook.com')"

echo -e "\n[i] All tests run. Bye!"
