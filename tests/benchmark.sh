#!/bin/bash
set -e

echo -e "[+] Installing dependencies...\n"

if [ `uname -s` == "Linux" ]; then
    sudo apt-get install python3-pip
fi

pip3 install requests

echo -e "\n[i] Starting basic benchmark tests\n"

echo -e "  => Running: time python3 -c \"import requests; requests.get('https://www.facebook.com')\""
time python3 -c "import requests; requests.get('https://www.facebook.com')"

echo -e "\n  => Running: time python3 -c \"import fastrequest; fastrequest.http_get('https://www.facebook.com')\""
time python3 -c "import fastrequest; fastrequest.http_get('https://www.facebook.com')"

echo -e "\n[i] All tests run. Bye!"
