-- Connect to the postgres database
\c postgres

-- Create the "vns" database owned by "postgres"
DO $$
BEGIN
    IF NOT EXISTS (SELECT FROM pg_database WHERE datname = 'vns') THEN
        CREATE DATABASE vns OWNER postgres;
    END IF;
END $$;

-- Connect to the "vns" database
\c vns

-- Create the "iptable" table
CREATE TABLE IF NOT EXISTS iptable (
    node_id SERIAL PRIMARY KEY,  -- Incremental integer, unique by default
    ip VARCHAR(15) NOT NULL UNIQUE,  -- IP address string, must be unique
    port INT NOT NULL  -- Integer for port
);

-- Insert the node ips, order matters (node_id 1 is master node)
INSERT INTO iptable (ip, port) VALUES ('20.121.137.95', 11000);
INSERT INTO iptable (ip, port) VALUES ('172.210.11.93', 11000);
INSERT INTO iptable (ip, port) VALUES ('48.217.241.39', 11000);

SELECT * FROM iptable;
