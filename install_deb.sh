#!/usr/bin/env bash
sudo apt install build-essential libgsl-dev libgtk-3-dev python-tk python-virtualenv python-pip
make -C src/
virtualenv venv
source venv/bin/activate
pip install -r requirements.txt