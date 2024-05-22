#pragma once
#pragma once

/* Distributed under the CC-wiki license.
 * user contributions licensed under cc by-sa 3.0 with attribution required: https://creativecommons.org/licenses/by-sa/3.0/
 * Originally taken from the answer by @rodrigo, found here: http://stackoverflow.com/a/12486703/850326
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string>

typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;

#define READ_BYTE(p) (((unsigned char*)(p))[0])
#define READ_WORD(p) ((((unsigned char*)(p))[0]) | ((((unsigned char*)(p))[1]) << 8))
#define READ_DWORD(p) ((((unsigned char*)(p))[0]) | ((((unsigned char*)(p))[1]) << 8) | \
    ((((unsigned char*)(p))[2]) << 16) | ((((unsigned char*)(p))[3]) << 24))

#define PAD(x) (((x) + 3) & 0xFFFFFFFC)


static const char* FindVersion(const char* buf)
{
    //buf is a IMAGE_DOS_HEADER
    if (READ_WORD(buf) != 0x5A4D) //MZ signature
        return NULL;
    //pe is a IMAGE_NT_HEADERS32
    const char* pe = buf + READ_DWORD(buf + 0x3C);
    if (READ_WORD(pe) != 0x4550) //PE signature
        return NULL;
    //coff is a IMAGE_FILE_HEADER
    const char* coff = pe + 4;

    WORD numSections = READ_WORD(coff + 2);
    WORD optHeaderSize = READ_WORD(coff + 16);
    if (numSections == 0 || optHeaderSize == 0)
        return NULL;
    //optHeader is a IMAGE_OPTIONAL_HEADER32
    const char* optHeader = coff + 20;
    WORD magic = READ_WORD(optHeader);
    if (magic != 0x10b && magic != 0x20b)
        return NULL;

    //dataDir is an array of IMAGE_DATA_DIRECTORY
    const char* dataDir = optHeader + (magic == 0x10b ? 96 : 112);
    DWORD vaRes = READ_DWORD(dataDir + 8 * 2);


    //secTable is an array of IMAGE_SECTION_HEADER
    const char* secTable = optHeader + optHeaderSize;
    int i;
    for (i = 0; i < numSections; ++i)
    {
        //sec is a IMAGE_SECTION_HEADER*
        const char* sec = secTable + 40 * i;
        char secName[9];
        memcpy(secName, sec, 8);
        secName[8] = 0;

        if (strcmp(secName, ".rsrc") != 0)
            continue;
        DWORD vaSec = READ_DWORD(sec + 12);
        const char* raw = buf + READ_DWORD(sec + 20);
        const char* resSec = raw + (vaRes - vaSec);
        WORD numNamed = READ_WORD(resSec + 12);
        WORD numId = READ_WORD(resSec + 14);

        int j;
        for (j = 0; j < numNamed + numId; ++j)
        {
            //resSec is a IMAGE_RESOURCE_DIRECTORY followed by an array
            // of IMAGE_RESOURCE_DIRECTORY_ENTRY
            const char* res = resSec + 16 + 8 * j;
            DWORD name = READ_DWORD(res);
            if (name != 16) //RT_VERSION
                continue;
            DWORD offs = READ_DWORD(res + 4);
            if ((offs & 0x80000000) == 0) //is a dir resource?
                return NULL;
            //verDir is another IMAGE_RESOURCE_DIRECTORY and 
            // IMAGE_RESOURCE_DIRECTORY_ENTRY array
            const char* verDir = resSec + (offs & 0x7FFFFFFF);
            numNamed = READ_WORD(verDir + 12);
            numId = READ_WORD(verDir + 14);
            if (numNamed == 0 && numId == 0)
                return NULL;
            res = verDir + 16;
            offs = READ_DWORD(res + 4);
            if ((offs & 0x80000000) == 0) //is a dir resource?
                return NULL;
            //and yet another IMAGE_RESOURCE_DIRECTORY, etc.
            verDir = resSec + (offs & 0x7FFFFFFF);
            numNamed = READ_WORD(verDir + 12);
            numId = READ_WORD(verDir + 14);
            if (numNamed == 0 && numId == 0)
                return NULL;
            res = verDir + 16;
            offs = READ_DWORD(res + 4);
            if ((offs & 0x80000000) != 0) //is a dir resource?
                return NULL;
            verDir = resSec + offs;


            DWORD verVa = READ_DWORD(verDir);
            const char* verPtr = raw + (verVa - vaSec);
            return verPtr;
        }
        return NULL;
    }
    return NULL;
}

static int PrintVersion(const char* version, std::string& tgt, int offs)
{
    offs = PAD(offs);
    WORD len = READ_WORD(version + offs);
    offs += 2;
    WORD valLen = READ_WORD(version + offs);
    offs += 2;
    WORD type = READ_WORD(version + offs);
    offs += 2;
    char info[200];
    int i;
    for (i = 0; i < 200; ++i)
    {
        WORD c = READ_WORD(version + offs);
        offs += 2;

        info[i] = c;
        if (!c)
            break;
    }
    offs = PAD(offs);
    if (type != 0) //TEXT
    {
        char value[200];
        for (i = 0; i < valLen; ++i)
        {
            WORD c = READ_WORD(version + offs);
            offs += 2;
            value[i] = c;
        }
        value[i] = 0;
        if (info == std::string("ProductVersion"))
        {
            tgt = std::string(value);
        }
    }
    else
    {
        if (strcmp(info, "VS_VERSION_INFO") == 0)
        {
            //fixed is a VS_FIXEDFILEINFO
            const char* fixed = version + offs;
            WORD fileA = READ_WORD(fixed + 10);
            WORD fileB = READ_WORD(fixed + 8);
            WORD fileC = READ_WORD(fixed + 14);
            WORD fileD = READ_WORD(fixed + 12);
            WORD prodA = READ_WORD(fixed + 18);
            WORD prodB = READ_WORD(fixed + 16);
            WORD prodC = READ_WORD(fixed + 22);
            WORD prodD = READ_WORD(fixed + 20);
            tgt = "v" + std::to_string(fileA) + "." + std::to_string(fileB) + "." + std::to_string(fileC);
            printf("\tFile: %d.%d.%d.%d\n", fileA, fileB, fileC, fileD);
        }
        offs += valLen;
    }
    while (offs < len)
        offs = PrintVersion(version, tgt, offs);
    return PAD(offs);
}
static std::string GetPEVersion(const char* file)
{
    struct stat st;
    if (stat(file, &st) < 0)
    {
        perror(file);
        return "Unknown";
    }

    char* buf = (char*)malloc(st.st_size);

    if (!buf)
    {
        return "Unknown";
    }

    FILE* f = fopen(file, "r");
    if (!f)
    {
        perror(file);
        return "Unable to read";
    }

    fread(buf, 1, st.st_size, f);
    fclose(f);
    std::string str;
    const char* version = FindVersion(buf);
    if (!version)
        str = "Unknown";
    else
        PrintVersion(version, str, 0);
    return str;
}
