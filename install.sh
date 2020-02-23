#!/bin/bash
set -e

echo -e "[+] Installing dependencies...\n"
#sudo apt-get install libcurl4 libcurl4-gnutls-dev python3-pip
pip3 install setuptools

echo -e "\n[+] Running extension setup...\n"
python3 setup.py build_ext --inplace

echo -e "\n[+] Installing module ...\n"
pip3 install .

echo -e "\n[+] Making cleanup ..."
rm -rf build/
rm *.so

if [ "$1" == "--notest" ]; then
    echo -e "\n[i] Done. Bye."
    exit
fi

echo -e "\n[+] Running tests ..."

for tfile in `ls tests/test_*.py`; do
    unit_name=$(echo "$tfile" | cut -c 12- | rev | cut -c 4- | rev)
    echo "==> Unit <$unit_name>"
    python3 "$tfile"
done

echo -e "\n[i] Done. Bye."
