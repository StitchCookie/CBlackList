#ifndef HTTPDEFINE
#define HTTPDEFINE
#include <QString>

/*HTTP Request Context*/
 struct HTTP_Request_{
    int checkType;
    QString cpcCardId;
    QString etcCardId;
    int laneId;
    QString license;
    QString oubid;
    int providl;
    int statinid;
};
/*HTTP Response Context*/
struct HTTP_Response_{

};

namespace HTTPAccept_Encoding{
const  QString gzip    ("gzip");
const  QString compress    ("compress");
const  QString deflate    ("deflate");
const  QString br    ("br");
const  QString identity    ("identity");
const  QString allotherCodine    ("*");
};
#endif // HTTPDEFINE

