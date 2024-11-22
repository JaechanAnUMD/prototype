sudo apt update
sudo apt install postgresql postgresql-contrib
sudo apt-get install libpq-dev
sudo service postgresql start

sudo -u postgres psql -U postgres -f setup.sql

