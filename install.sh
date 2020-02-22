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

echo "[i] Done. Bye."
