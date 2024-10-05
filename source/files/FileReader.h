
#ifndef GAME_FILEREADER_H
#define GAME_FILEREADER_H

#include <vector>

class FileReader
{
  public:

    FileReader(const char *path);

    template<typename type>
    type read()
    {
        type out;
        if (!hasNMoreBytes(sizeof(type)))
        {
            return out;
        }
        copy(sizeof(type), (char *) &out);
        readPos += sizeof(type);
        return out;
    }

    template<typename container>
    void readInto(int n, container &out)
    {
        if (!hasNMoreBytes(n))
        {
            return;
        }
        out.insert(out.end(), n, 0);
        copy(n, (char *) &out[out.size() - n]);
        readPos += n;
    }

    void copy(int n, char *out);

    void skip(int n);

    template<typename type>
    void skip()
    {
        skip(sizeof(type));
    }

    int currentReadPosition() const;

    bool hasNMoreBytes(int n) const;

    bool reachedEnd() const;

  protected:
    int readPos = 0;
    std::vector<unsigned char> data;
};


#endif
