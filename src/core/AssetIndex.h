#ifndef BC_ASSETINDEX_H
#define BC_ASSETINDEX_H

#include "Version.h"

typedef struct bc_assetindex_asset {
    char objectid[128];
    char hash[128];
    char baseUrl[256]; // Betacraft exclusive
    long size;
} bc_assetindex_asset;

typedef struct bc_assetindex {
    bc_assetindex_asset *objects;
    int len;
    // int virtual;
    int mapToResources;
} bc_assetindex;

bc_assetindex *bc_assetindex_load(bc_version_assetIndexData *data);

#endif
