DO $$
BEGIN
    IF NOT EXISTS (SELECT FROM pg_roles WHERE rolname = 'jaechan') THEN
        CREATE ROLE jaechan WITH LOGIN PASSWORD '1234';
    END IF;
END $$;

-- Grant the CREATEDB privilege to the user
ALTER ROLE jaechan CREATEDB;

-- Check if the database exists and create it if it does not
SELECT 'CREATE DATABASE vns OWNER jaechan'
WHERE NOT EXISTS (
    SELECT FROM pg_database WHERE datname = 'vns'
) \gexec

-- Connect to the database
\c vns

-- Grant all privileges on the "public" schema to the user
GRANT ALL ON SCHEMA public TO jaechan;

-- Create the tables with ownership assigned to "jaechan"
CREATE TABLE IF NOT EXISTS node_ips (
    node_id SERIAL PRIMARY KEY,
    ip VARCHAR(15) NOT NULL UNIQUE,
    port INT NOT NULL
);

CREATE TABLE IF NOT EXISTS node_connections (
    from_node_id INTEGER NOT NULL REFERENCES node_ips(node_id) ON DELETE CASCADE,
    to_node_id INTEGER NOT NULL REFERENCES node_ips(node_id) ON DELETE CASCADE,
    PRIMARY KEY (from_node_id, to_node_id)
);

-- Change the owner of the tables to "jaechan"
ALTER TABLE node_ips OWNER TO jaechan;
ALTER TABLE node_connections OWNER TO jaechan;

-- Insert initial data into "node_ips"
INSERT INTO node_ips (node_id, ip, port) VALUES
    (0, '20.121.137.95', 11000), -- Master node
    (1, '172.210.11.93', 11000), -- node 1 (jaechan-node-1)
    (2, '48.217.241.39', 11000), -- node 2 (jaechan-node-2)
    (3, '20.62.166.184', 11000)  -- node 3 (jaechan-node-3)
ON CONFLICT DO NOTHING; -- Prevent duplicate insertion if the script is rerun

-- Insert initial data into "node_connections"
INSERT INTO node_connections (from_node_id, to_node_id) -- For master node
SELECT 0, node_id
FROM node_ips
WHERE node_id != 0
ON CONFLICT DO NOTHING;

INSERT INTO node_connections (from_node_id, to_node_id) VALUES
    (1, 3), -- Node 1 connected to Node 3
    (3, 1), -- Node 3 connected to Node 1
    (2, 3), -- Node 2 connected to Node 3
    (3, 2)  -- Node 3 connected to Node 2
ON CONFLICT DO NOTHING;

SELECT * FROM node_ips;

SELECT * FROM node_connections;
