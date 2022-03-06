#ifndef __OID_H
#define __OID_H

class QString;
#include <QList>
#include <QMap>

typedef QList<int> NIDlist;

extern NIDlist extkeyuse_nid;
extern NIDlist distname_nid;

extern int first_additional_oid;
extern QMap<QString, const char*> oid_name_clash;
extern QMap<QString, int> oid_lower_map;

/* reads additional OIDs from a file: oid, sn, ln */

void initOIDs();

#endif
