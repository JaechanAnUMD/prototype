#!/bin/bash
sudo apt update
sudo apt upgrade
sudo apt install postgresql postgresql-contrib
sudo apt-get install libpq-dev
sudo service postgresql start

# Setup postgres password
sudo -i -u postgres psql
# ALTER USER postgres PASSWORD 'postgres'
