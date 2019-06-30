#include "stdout.h"
QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

QTextStream& qStdErr()
{
    static QTextStream ts( stderr );
    return ts;
}
