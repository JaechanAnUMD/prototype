#include "postgres.h"

#include <cassert>
#include <iostream>

namespace VNS {

Postgres::Postgres() {}

Postgres::~Postgres() {}

void Postgres::Open() {
    const char* conninfo = "dbname=vns user=postgres hostaddr=127.0.0.1 port=5432";

    PGconn* conn = PQconnectdb(conninfo);

    m_conn = conn;

    CheckConnection();
}

void Postgres::Close() {
    if (m_conn != NULL) {
        PQfinish(m_conn);
    }
}

void Postgres::CheckConnection() {
    assert(m_conn != NULL);

    if (PQstatus(m_conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(m_conn) << std::endl;
        PQfinish(m_conn);

        assert(true);  // should never reach
    }
}

PGresult* Postgres::Exec(const std::string& query) {
    assert(m_conn != NULL);
    return PQexec(m_conn, query.c_str());
}

}  // namespace VNS
