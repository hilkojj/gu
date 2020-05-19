
#include "AsepriteLoader.h"
#include <zlib.h>

namespace aseprite
{

Loader::Loader(const char *filePath, Sprite &output) : sprite(output), filePath(filePath), FileReader(filePath)
{
    output.name = filePath;

    loadHeader();
    loadFrames();

    for (auto &tag : output.tags)
        for (int frameI = tag.from; frameI <= tag.to; frameI++)
            tag.duration += output.frames[frameI].duration;
}

std::string Loader::loadString()
{
    if (!hasNMoreBytes(sizeof(WORD)))
        throw gu_err("no string here");
    int length = read<WORD>();
    std::string str(length, ' ');
    if (!hasNMoreBytes(length))
        throw gu_err("no string here");
    memcpy(&str[0], &data[pos], length);
    skip(str.size());
    return str;
}

ColorRGBA Loader::loadColorRGBA()
{
    BYTE
        r = read<BYTE>(),
        g = read<BYTE>(),
        b = read<BYTE>(),
        a = read<BYTE>();
    return ColorRGBA(r, g, b, a);
}

ColorRGB Loader::loadColorRGB()
{
    BYTE
        r = read<BYTE>(),
        g = read<BYTE>(),
        b = read<BYTE>();
    return ColorRGB(r, g, b);
}

void Loader::loadHeader()
{
    // file size:
    read<DWORD>();

    // Magic number (0xA5E0)
    if (read<WORD>() != 0xA5E0)
        throw gu_err(std::string(filePath) + " is not in .ase format!");

    // Frames / Width / Height / Color Mode
    sprite.frameCount = read<WORD>();
    sprite.width = read<WORD>();
    sprite.height = read<WORD>();
    sprite.mode = Mode(read<WORD>() / 8);

    if (sprite.mode == Mode::grayscale || sprite.mode == Mode::rgba)
        throw gu_err("rgba & grayscale mode not supported, for now only indexed mode is supported");

    // Other Info, Ignored
    skip<DWORD>();       // Flags
    skip<WORD>();        // Speed (deprecated)
    skip<DWORD>();       // Set be 0
    skip<DWORD>();       // Set be 0
    skip<BYTE>();        // Palette entry
    skip(3);             // Ignore these bytes
    skip<WORD>();        // Number of colors (0 means 256 for old sprites)
    skip<BYTE>();        // Pixel width
    skip<BYTE>();        // Pixel height
    skip(8 + 84);            // For Future
}

void Loader::loadFrames()
{
    sprite.frames.reserve(sprite.frameCount);
    for (int i = 0; i < sprite.frameCount; i++)
    {
        sprite.frames.emplace_back();
        Frame &frame = sprite.frames.back();

        frame.pixels.resize(sprite.width * sprite.height, 0);

        int
            frameStart = currentPosition(),
            frameEnd = frameStart + int(read<DWORD>());

        if (read<WORD>() != 0xF1FA) // magic number
            throw gu_err("Frame does not have magic number in header.");

        int chunkCount = read<WORD>();  // Number of "chunks" in this frame
        frame.duration = float(read<WORD>()) / 1000.f;  // Frame duration (in milliseconds -> seconds)
        skip(2);                        // For future (set to zero)
        int biggerChunkCount = read<DWORD>();
        if (biggerChunkCount != 0)
            chunkCount = biggerChunkCount;

        // chunks:
        for (int j = 0; j < chunkCount; j++)
        {
            int
                chunkStart = currentPosition(),
                chunkEnd = chunkStart + read<DWORD>();

            ChunkType type = ChunkType(read<WORD>());

            switch (type)
            {
                case layer:
                    loadLayer();
                    break;
                case cel:
                    loadCel(frame, chunkEnd);
                    break;
                case palette:
                    loadPalette();
                    break;
                case userData:
                    loadUserData();
                    break;
                case frameTags:
                    loadFrameTags();
                    break;
                case slice:
                    loadSlices();
                    break;
                case oldPaletteA:
                case oldPaletteB:
                case celExtra:
                case mask:
                case path:
                default:
                    break;
            }
            skip(chunkEnd - currentPosition());
        }
        skip(frameEnd - currentPosition());
    }
}

void Loader::loadLayer()
{
    sprite.layers.emplace_back();
    Layer &layer = sprite.layers.back();

    WORD flags = read<WORD>();
    layer.visible = flags & 0x1u;
    layer.type = Layer::Type(read<WORD>());
    layer.childLevel = read<WORD>();
    skip<WORD>(); // width (unused)
    skip<WORD>(); // height (unused)
    layer.blendMode = read<WORD>();
    layer.alpha = read<BYTE>() / 255.f;
    skip(3); // for future
    layer.name = loadString();

    lastUserData = &layer;
}

void Loader::loadCel(Frame &frame, int celEnd)
{
    frame.cels.emplace_back();
    Cel &cel = frame.cels.back();

    cel.layer = read<WORD>();
    cel.x = read<SHORT>();
    cel.y = read<SHORT>();
    cel.alpha = read<BYTE>() / 255.f;

    int celType = read<WORD>();

    skip(7);

    if (celType == 1)
        throw gu_err("Linked Cels are not supported.");
    // else:

    cel.width = read<WORD>();
    cel.height = read<WORD>();

    int count = cel.width * cel.height * sprite.mode;
    assert(sprite.mode == 1 && 1 == Mode::indexed);

    cel.pixels.resize(count, 0);

    if (celType == 0) // RAW pixels
    {
        read(count, cel.pixels);
    }
    else if (celType == 2) // ZLIB compressed pixels
    {
        int compressedPixelsBegin = currentPosition();
        int compressedPixelsEnd = celEnd;

        int size = compressedPixelsEnd - compressedPixelsBegin;
        std::vector<char> compressedPixels;
        read(size, compressedPixels);

        z_stream infstream;
        infstream.zalloc = Z_NULL;
        infstream.zfree = Z_NULL;
        infstream.opaque = Z_NULL;

        infstream.avail_in = size; // size of input
        infstream.next_in = (Bytef *)&compressedPixels[0]; // input char array
        infstream.avail_out = count; // size of output
        infstream.next_out = (Bytef *)&cel.pixels[0]; // output char array

        // the actual DE-compression work.
        inflateInit(&infstream);
        inflate(&infstream, Z_NO_FLUSH);
        inflateEnd(&infstream);
    }
    if (currentPosition() != celEnd)
        throw gu_err("file corrupt.");

    celToFrame(frame, cel);
    lastUserData = &cel;
}

void Loader::loadPalette()
{
    int size = read<DWORD>();
    int start = read<DWORD>();
    int end = read<DWORD>();
    skip(8); // for future

    for (int p = 0; p < (end - start) + 1; p++)
    {
        bool hasName = read<WORD>();

        sprite.palette[start + p] = loadColorRGBA();

        if (hasName)
            loadString();
    }
}

void Loader::loadUserData()
{
    DWORD flags = read<DWORD>();
    std::string text;
    ColorRGBA color;

    if (flags & 0x1u) // has text
        text = loadString();
    if (flags & (0x1u << 1u)) // has color
        color = loadColorRGBA();

    if (lastUserData)
    {
        lastUserData->text = text;
        lastUserData->color = color;
    }
}

void Loader::loadFrameTags()
{
    int count = read<WORD>();
    skip(8);
    sprite.tags.reserve(count);

    for (int i = 0; i < count; i++)
    {
        sprite.tags.emplace_back();
        Tag &tag = sprite.tags.back();

        tag.from = read<WORD>();
        tag.to = read<WORD>();
        tag.loopDirection = Tag::LoopDirection(read<BYTE>());
        skip(8);
        tag.color = loadColorRGB();
        skip(1);
        tag.name = loadString();
        tag.duration = 0;
    }
}

void Loader::loadSlices()
{
    int count = read<DWORD>();
    DWORD flags = read<DWORD>();
    skip<DWORD>(); // reserved
    std::string name = loadString();

    for (int i = 0; i < count; i++)
    {
        sprite.slices.emplace_back();
        Slice &slice = sprite.slices.back();

        slice.name = name;
        slice.frame = read<DWORD>();
        slice.originX = read<LONG>();
        slice.originY = read<LONG>();
        slice.width = read<DWORD>();
        slice.height = read<DWORD>();

        // 9 slice (todo, useful for GUIs):
        if (flags & 0x1u)
        {
            std::cerr << "9-slice found in " << filePath << " ignored.\n";
            skip<LONG>();
            skip<LONG>();
            skip<DWORD>();
            skip<DWORD>();
        }
        // pivot point
        if (flags & (0x1u << 1u))
            slice.pivot.emplace(read<DWORD>(), read<DWORD>());
        lastUserData = &slice;
    }
}

void Loader::celToFrame(Frame &frame, Cel &cel)
{
    Layer &layer = sprite.layers.at(cel.layer);
    if (!layer.visible)
        return;

    for (int sx = 0; sx < cel.width; sx++)
    {
        int dx = cel.x + sx;
        int dy = cel.y * sprite.width;

        for (int i = 0, sy = 0; i < cel.height; i++, sy += cel.width, dy += sprite.width)
        {
            // todo: custom blending
            Pixel &p = cel.pixels[sx + sy];
            if (p != 0)
                frame.pixels[dx + dy] = p;
        }
    }
}

}
