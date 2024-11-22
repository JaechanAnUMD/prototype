#!/bin/bash

# Variables
DB_HOST="localhost"  # Change to your Postgres server address if needed
DB_PORT="5432"       # Default Postgres port
ADMIN_USER="postgres"  # Postgres admin user
ADMIN_PASSWORD="postgres"  # Admin password
NEW_DB="vns"
NEW_USER="jaechan"
NEW_USER_PASSWORD="1234"
SQL_FILE="setup.sql"

# Export admin credentials for non-interactive authentication
export PGPASSWORD=$ADMIN_PASSWORD

# Step 1: Run setup.sql to create user and database
psql -h $DB_HOST -p $DB_PORT -U $ADMIN_USER -f "$SQL_FILE"

# Step 2: Connect to the new database and execute table creation
export PGPASSWORD=$NEW_USER_PASSWORD
psql -h $DB_HOST -p $DB_PORT -U $NEW_USER -d $NEW_DB << EOF
-- Grant all privileges on the "public" schema to the user
GRANT ALL ON SCHEMA public TO $NEW_USER;

-- Create the "node_ips" table with ownership assigned to "$NEW_USER"
CREATE TABLE IF NOT EXISTS node_ips (
    node_id SERIAL PRIMARY KEY,
    ip VARCHAR(15) NOT NULL UNIQUE,
    port INT NOT NULL
);

-- Change the owner of the table to "$NEW_USER"
ALTER TABLE node_ips OWNER TO $NEW_USER;
EOF

# Check for errors and clean up
if [ $? -eq 0 ]; then
    echo "Database setup completed successfully!"
else
    echo "Error: Failed to execute database setup."
    exit 2
fi

# Unset password variables for security
unset PGPASSWORD
