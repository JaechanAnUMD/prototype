#include <libpq-fe.h>

#include <string>

namespace VNS {

class Postgres {
private:
    PGconn* m_conn;

public:
    PGconn* conn() { return m_conn; }
    const PGconn* conn() const { return m_conn; }

public:
    Postgres();
    ~Postgres();

    void Open();
    void Close();

    PGresult* Exec(const std::string& query);

private:
    void CheckConnection();
};

}  // namespace VNS
